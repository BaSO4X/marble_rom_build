#!/usr/bin/env python3
"""Derive the build-specific LHDC anchors from a libbluetooth_jni.so build.

Xiaomi rebuilds libbluetooth_jni.so for every ROM release, so every absolute
address the LHDC patches rely on moves.  Instead of pinning one build by hash,
locate each anchor structurally:

  * the GNU build ID, so the runtime companion still refuses foreign libraries;
  * A2dpCodecConfig::copyOutOtaCodecConfig(), read from the minidebuginfo
    symbol table the platform keeps in .gnu_debugdata;
  * the LHDCv5 and LHDCv3 encoder interface tables, read out of the
    A2DP_VendorGetEncoderInterfaceLhdcV{5,3}() ADRP/ADD pair;
  * the A2DP_GetPacketTimestamp() codec switch: the LHDCv5 key comparison, the
    unsupported-codec logging block the compatibility patch overwrites, the
    generic timestamp path, and the shared "return false" path.

Only AArch64 little-endian builds are supported, which is what the device ships.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import lzma
import struct
import sys

# A2DP_GetPacketTimestamp() rebuilds a 64-bit key from the vendor codec info:
# media codec type 0xFF, the low 16 bits of the Savitech vendor ID 0x053A, and
# the 16-bit codec ID.  LHDCv5 is 0x4C35; the historical v2/v3 IDs the
# compatibility patch re-admits are 0x4C32 and 0x4C33.
LHDCV5_SWITCH_KEY = 0x0000004C35053AFF

COPY_OUT_OTA_CODEC_CONFIG = "_ZN15A2dpCodecConfig21copyOutOtaCodecConfigEPh"
GET_PACKET_TIMESTAMP = "_Z23A2DP_GetPacketTimestampPKhS0_Pj"
GET_ENCODER_INTERFACE_LHDCV5 = "_Z36A2DP_VendorGetEncoderInterfaceLhdcV5PKh"
GET_ENCODER_INTERFACE_LHDCV3 = "_Z36A2DP_VendorGetEncoderInterfaceLhdcV3PKh"

# The compatibility thunk assembled by patch_bluetooth_jni.sh is seven
# instructions long.
TIMESTAMP_PATCH_SIZE = 28

SHT_SYMTAB = 2
SHT_NOTE = 7
NT_GNU_BUILD_ID = 3
STT_FUNC = 2
EM_AARCH64 = 183


class DeriveError(Exception):
    """A structural anchor could not be located."""


class Elf:
    def __init__(self, data, source):
        self.data = data
        self.source = source
        if data[:4] != b"\x7fELF":
            raise DeriveError("%s: not an ELF file" % source)
        if data[4] != 2 or data[5] != 1:
            raise DeriveError("%s: not a 64-bit little-endian ELF file" % source)
        if struct.unpack_from("<H", data, 18)[0] != EM_AARCH64:
            raise DeriveError("%s: not an AArch64 ELF file" % source)
        shoff = struct.unpack_from("<Q", data, 0x28)[0]
        shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x3A)
        self.sections = []
        for index in range(shnum):
            offset = shoff + index * shentsize
            fields = struct.unpack_from("<IIQQQQIIQQ", data, offset)
            self.sections.append({
                "name_offset": fields[0],
                "type": fields[1],
                "addr": fields[3],
                "offset": fields[4],
                "size": fields[5],
                "link": fields[6],
                "entsize": fields[9],
            })
        names = self.sections[shstrndx]
        for section in self.sections:
            section["name"] = self._string(names, section["name_offset"])

    def _string(self, strtab, offset):
        start = strtab["offset"] + offset
        end = self.data.index(b"\0", start)
        return self.data[start:end].decode("utf-8", "replace")

    def section(self, name):
        for section in self.sections:
            if section["name"] == name:
                return section
        return None

    def section_bytes(self, section):
        return self.data[section["offset"]:section["offset"] + section["size"]]

    def build_id(self):
        for section in self.sections:
            if section["type"] != SHT_NOTE:
                continue
            blob = self.section_bytes(section)
            cursor = 0
            while cursor + 12 <= len(blob):
                namesz, descsz, kind = struct.unpack_from("<III", blob, cursor)
                cursor += 12
                name = blob[cursor:cursor + namesz]
                cursor += (namesz + 3) & ~3
                desc = blob[cursor:cursor + descsz]
                cursor += (descsz + 3) & ~3
                if kind == NT_GNU_BUILD_ID and name.startswith(b"GNU\0"):
                    return desc
        raise DeriveError("%s: no GNU build ID note" % self.source)

    def function_symbols(self):
        """Map function name to (address, size).

        Platform libraries are stripped down to .dynsym plus an xz compressed
        minidebuginfo image in .gnu_debugdata; the local symbols these anchors
        need only exist in the latter.
        """
        symbols = {}
        for elf in self._symbol_sources():
            for section in elf.sections:
                if section["type"] != SHT_SYMTAB or section["entsize"] == 0:
                    continue
                strtab = elf.sections[section["link"]]
                blob = elf.section_bytes(section)
                for offset in range(0, len(blob), section["entsize"]):
                    fields = struct.unpack_from("<IBBHQQ", blob, offset)
                    name, info, shndx, value, size = (
                        fields[0], fields[1], fields[3], fields[4], fields[5])
                    if name == 0 or shndx == 0 or (info & 0xF) != STT_FUNC:
                        continue
                    symbols.setdefault(elf._string(strtab, name), (value, size))
        return symbols

    def _symbol_sources(self):
        yield self
        debugdata = self.section(".gnu_debugdata")
        if debugdata is None:
            return
        try:
            embedded = lzma.decompress(self.section_bytes(debugdata))
        except lzma.LZMAError as error:
            raise DeriveError("%s: .gnu_debugdata is not valid xz data: %s"
                              % (self.source, error))
        yield Elf(embedded, "%s (.gnu_debugdata)" % self.source)

    def virtual_range(self, address, size):
        for section in self.sections:
            if section["addr"] == 0 or section["size"] == 0:
                continue
            start = section["addr"]
            if start <= address and address + size <= start + section["size"]:
                offset = section["offset"] + address - start
                return self.data[offset:offset + size]
        raise DeriveError("%s: address 0x%x is unmapped" % (self.source, address))


# --- Minimal AArch64 decoding ------------------------------------------------


def _signed(value, bits):
    return value - (1 << bits) if value & (1 << (bits - 1)) else value


_WIDE_64 = {0xD2800000: "movz", 0xF2800000: "movk", 0x92800000: "movn"}
_WIDE_32 = {0x52800000: "movz", 0x72800000: "movk", 0x12800000: "movn"}


def decode(address, word):
    """Decode the handful of instruction forms the anchors are expressed in."""
    wide = word & 0xFF800000
    if wide in _WIDE_64 or wide in _WIDE_32:
        table = _WIDE_64 if wide in _WIDE_64 else _WIDE_32
        return {"kind": table[wide], "rd": word & 0x1F,
                "shift": ((word >> 21) & 3) * 16, "imm": (word >> 5) & 0xFFFF,
                "width": 64 if wide in _WIDE_64 else 32}
    if word & 0x9F000000 == 0x90000000:
        immlo = (word >> 29) & 3
        immhi = (word >> 5) & 0x7FFFF
        page = (address & ~0xFFF) + (_signed((immhi << 2) | immlo, 21) << 12)
        return {"kind": "adrp", "rd": word & 0x1F, "page": page}
    if word & 0xFF800000 == 0x91000000:
        imm = (word >> 10) & 0xFFF
        if (word >> 22) & 1:
            imm <<= 12
        return {"kind": "add", "rd": word & 0x1F, "rn": (word >> 5) & 0x1F,
                "imm": imm}
    if word & 0xFF200000 == 0xEB000000:
        return {"kind": "subs", "rd": word & 0x1F, "rn": (word >> 5) & 0x1F,
                "rm": (word >> 16) & 0x1F, "shift": (word >> 10) & 0x3F}
    if word & 0xFC000000 == 0x14000000:
        return {"kind": "b",
                "target": address + (_signed(word & 0x3FFFFFF, 26) << 2)}
    if word & 0xFF000010 == 0x54000000:
        return {"kind": "b.cond", "cond": word & 0xF,
                "target": address + (_signed((word >> 5) & 0x7FFFF, 19) << 2)}
    if word & 0x7F000000 in (0x34000000, 0x35000000):
        return {"kind": "cbz" if word & 0x01000000 == 0 else "cbnz",
                "target": address + (_signed((word >> 5) & 0x7FFFF, 19) << 2)}
    if word & 0x7E000000 == 0x36000000:
        return {"kind": "tbz" if word & 0x01000000 == 0 else "tbnz",
                "target": address + (_signed((word >> 5) & 0x3FFF, 14) << 2)}
    if word & 0xFFC00000 == 0xB9400000:
        return {"kind": "ldr32", "rt": word & 0x1F, "rn": (word >> 5) & 0x1F,
                "imm": ((word >> 10) & 0xFFF) * 4}
    if word & 0xFFC00000 == 0xB9000000:
        return {"kind": "str32", "rt": word & 0x1F, "rn": (word >> 5) & 0x1F,
                "imm": ((word >> 10) & 0xFFF) * 4}
    if word & 0xFFFFFC1F == 0xD65F0000:
        return {"kind": "ret"}
    if word & 0xFFFFFC1F == 0xD61F0000:
        return {"kind": "br"}
    if word == 0x2A1F03E0:
        return {"kind": "mov_w0_wzr"}
    return {"kind": "other"}


B_EQ = 0x0
B_NE = 0x1
TERMINATORS = ("b", "ret", "br")


def disassemble(elf, address, size):
    if size == 0 or size % 4 != 0:
        raise DeriveError("unexpected function size %d at 0x%x" % (size, address))
    blob = elf.virtual_range(address, size)
    listing = []
    for index in range(0, size, 4):
        word = struct.unpack_from("<I", blob, index)[0]
        instruction = decode(address + index, word)
        instruction["address"] = address + index
        instruction["word"] = word
        listing.append(instruction)
    return listing


def _wide_immediate(listing, start):
    """Fold a MOVZ/MOVK chain starting at `start` into (value, register, end)."""
    head = listing[start]
    register = head["rd"]
    value = head["imm"] << head["shift"]
    index = start + 1
    while (index < len(listing) and listing[index]["kind"] == "movk"
           and listing[index]["rd"] == register):
        value |= listing[index]["imm"] << listing[index]["shift"]
        index += 1
    return value, register, index


# --- Anchor derivation -------------------------------------------------------


def derive_encoder_table(elf, symbols, name):
    """Read the encoder interface table an accessor returns in x0."""
    if name not in symbols:
        raise DeriveError("missing symbol: %s" % name)
    address, size = symbols[name]
    pages = {}
    candidates = []
    for instruction in disassemble(elf, address, size):
        if instruction["kind"] == "adrp":
            pages[instruction["rd"]] = instruction["page"]
        elif (instruction["kind"] == "add" and instruction["rd"] == 0
              and instruction["rn"] == 0 and 0 in pages):
            candidates.append(pages.pop(0) + instruction["imm"])
    if len(candidates) != 1:
        raise DeriveError(
            "%s: expected exactly one ADRP/ADD pair returning a table, found %d"
            % (name, len(candidates)))
    return candidates[0]


def _find_patch_site(listing):
    """Return the unsupported-codec path the LHDCv5 key comparison branches to."""
    patch_va = None
    for index, instruction in enumerate(listing):
        if instruction["kind"] != "movz" or instruction["width"] != 64:
            continue
        value, register, after = _wide_immediate(listing, index)
        if value != LHDCV5_SWITCH_KEY or after + 1 >= len(listing):
            continue
        compare = listing[after]
        if (compare["kind"] != "subs" or compare["rd"] != 31
                or compare["rm"] != register or compare["shift"] != 0):
            continue
        branch = listing[after + 1]
        if branch["kind"] != "b.cond" or branch["cond"] != B_NE:
            raise DeriveError(
                "A2DP_GetPacketTimestamp: the LHDCv5 key comparison is no "
                "longer followed by B.NE to the unsupported-codec path")
        if patch_va is not None:
            raise DeriveError(
                "A2DP_GetPacketTimestamp: the LHDCv5 key is compared more "
                "than once")
        patch_va = branch["target"]
    if patch_va is None:
        raise DeriveError(
            "A2DP_GetPacketTimestamp: no comparison against the LHDCv5 codec "
            "key 0x%x" % LHDCV5_SWITCH_KEY)
    return patch_va


def _find_success_path(listing):
    """Return the generic `*timestamp = *p_ts; return true` block."""
    success_va = None
    for index in range(len(listing) - 2):
        window = listing[index:index + 3]
        loads = [item for item in window if item["kind"] == "ldr32"
                 and item["rn"] == 1 and item["imm"] == 0]
        stores = [item for item in window if item["kind"] == "str32"
                  and item["rn"] == 2 and item["imm"] == 0]
        ones = [item for item in window if item["kind"] == "movz"
                and item["width"] == 32 and item["rd"] == 0
                and item["imm"] == 1 and item["shift"] == 0]
        if len(loads) != 1 or len(stores) != 1 or len(ones) != 1:
            continue
        if (loads[0]["rt"] != stores[0]["rt"]
                or loads[0]["address"] > stores[0]["address"]):
            continue
        candidate = window[0]["address"]
        if success_va is not None and success_va != candidate:
            raise DeriveError(
                "A2DP_GetPacketTimestamp: more than one generic timestamp path")
        success_va = candidate
    if success_va is None:
        raise DeriveError(
            "A2DP_GetPacketTimestamp: no generic timestamp path of the form "
            "*timestamp = *p_ts; return true")
    if not any(item["kind"] == "b.cond" and item["cond"] == B_EQ
               and item["target"] == success_va for item in listing):
        raise DeriveError(
            "A2DP_GetPacketTimestamp: the generic timestamp path is not "
            "reached from the codec switch")
    return success_va


def derive_timestamp(elf, symbols):
    if GET_PACKET_TIMESTAMP not in symbols:
        raise DeriveError("missing symbol: %s" % GET_PACKET_TIMESTAMP)
    address, size = symbols[GET_PACKET_TIMESTAMP]
    listing = disassemble(elf, address, size)
    by_address = dict((item["address"], index)
                      for index, item in enumerate(listing))

    patch_va = _find_patch_site(listing)
    if patch_va not in by_address:
        raise DeriveError(
            "A2DP_GetPacketTimestamp: the unsupported-codec path is outside "
            "the function")
    start_index = by_address[patch_va]
    if start_index == 0 or listing[start_index - 1]["kind"] not in TERMINATORS:
        raise DeriveError(
            "A2DP_GetPacketTimestamp: the unsupported-codec path can be "
            "reached by falling through")

    failure_va = None
    block_end = None
    for instruction in listing[start_index:]:
        if instruction["kind"] == "b":
            failure_va = instruction["target"]
            block_end = instruction["address"] + 4
            break
        if instruction["kind"] in ("ret", "br"):
            break
    if failure_va is None:
        raise DeriveError(
            "A2DP_GetPacketTimestamp: the unsupported-codec path does not end "
            "in a branch to the shared failure path")
    if block_end - patch_va < TIMESTAMP_PATCH_SIZE:
        raise DeriveError(
            "A2DP_GetPacketTimestamp: the unsupported-codec path is only %d "
            "bytes, need %d" % (block_end - patch_va, TIMESTAMP_PATCH_SIZE))
    if (failure_va not in by_address
            or listing[by_address[failure_va]]["kind"] != "mov_w0_wzr"):
        raise DeriveError(
            "A2DP_GetPacketTimestamp: the failure path does not start by "
            "returning false")

    success_va = _find_success_path(listing)
    overwritten = range(patch_va + 4, patch_va + TIMESTAMP_PATCH_SIZE)
    for instruction in listing:
        target = instruction.get("target")
        if target is not None and target in overwritten:
            raise DeriveError(
                "A2DP_GetPacketTimestamp: 0x%x branches into the region the "
                "compatibility patch overwrites" % instruction["address"])
    for anchor in (success_va, failure_va):
        if patch_va <= anchor < patch_va + TIMESTAMP_PATCH_SIZE:
            raise DeriveError(
                "A2DP_GetPacketTimestamp: 0x%x lies inside the region the "
                "compatibility patch overwrites" % anchor)

    return {
        "function_va": address,
        "function_size": size,
        "patch_va": patch_va,
        "patch_size": TIMESTAMP_PATCH_SIZE,
        "expected_site": elf.virtual_range(patch_va, TIMESTAMP_PATCH_SIZE).hex(),
        "success_va": success_va,
        "failure_va": failure_va,
    }


def derive(path):
    with open(path, "rb") as handle:
        data = handle.read()
    elf = Elf(data, path)
    symbols = elf.function_symbols()
    if COPY_OUT_OTA_CODEC_CONFIG not in symbols:
        raise DeriveError("missing symbol: %s" % COPY_OUT_OTA_CODEC_CONFIG)
    text = elf.section(".text")
    if text is None:
        raise DeriveError("%s: no .text section" % path)
    return {
        "library": path,
        "sha256": hashlib.sha256(data).hexdigest(),
        "build_id": elf.build_id().hex(),
        "text_va": text["addr"],
        "text_offset": text["offset"],
        "copy_out_ota_codec_config_va": symbols[COPY_OUT_OTA_CODEC_CONFIG][0],
        "lhdcv5_encoder_table_va": derive_encoder_table(
            elf, symbols, GET_ENCODER_INTERFACE_LHDCV5),
        "lhdcv3_encoder_table_va": derive_encoder_table(
            elf, symbols, GET_ENCODER_INTERFACE_LHDCV3),
        "timestamp": derive_timestamp(elf, symbols),
    }


# --- Output ------------------------------------------------------------------


def as_shell(profile):
    timestamp = profile["timestamp"]
    pairs = (
        ("library_sha256", profile["sha256"]),
        ("build_id", profile["build_id"]),
        ("text_va", hex(profile["text_va"])),
        ("text_offset", hex(profile["text_offset"])),
        ("copy_out_ota_codec_config_va",
         hex(profile["copy_out_ota_codec_config_va"])),
        ("lhdcv5_encoder_table_va", hex(profile["lhdcv5_encoder_table_va"])),
        ("lhdcv3_encoder_table_va", hex(profile["lhdcv3_encoder_table_va"])),
        ("timestamp_patch_va", hex(timestamp["patch_va"])),
        ("timestamp_patch_size", str(timestamp["patch_size"])),
        ("timestamp_expected_site", timestamp["expected_site"]),
        ("timestamp_success_va", hex(timestamp["success_va"])),
        ("timestamp_failure_va", hex(timestamp["failure_va"])),
    )
    return "".join("%s=%s\n" % pair for pair in pairs)


def as_header(profile):
    build_id = bytes.fromhex(profile["build_id"])
    rows = "\n".join(
        "    " + " ".join("0x%02x," % byte for byte in build_id[index:index + 8])
        for index in range(0, len(build_id), 8))
    return (
        "// Generated by derive_profile.py. Do not edit.\n"
        "//\n"
        "// Anchors derived from %s\n"
        "// sha256 %s\n"
        "\n"
        "#pragma once\n"
        "\n"
        "#include <cstddef>\n"
        "#include <cstdint>\n"
        "\n"
        "namespace bluetooth_jni_profile {\n"
        "\n"
        "inline constexpr uint8_t kBuildId[] = {\n"
        "%s\n"
        "};\n"
        "inline constexpr uintptr_t kCopyOutOtaCodecConfigOffset = 0x%x;\n"
        "inline constexpr uintptr_t kLhdcV5EncoderTableOffset = 0x%x;\n"
        "inline constexpr uintptr_t kLhdcV3EncoderTableOffset = 0x%x;\n"
        "\n"
        "}  // namespace bluetooth_jni_profile\n"
        % (profile["library"], profile["sha256"], rows,
           profile["copy_out_ota_codec_config_va"],
           profile["lhdcv5_encoder_table_va"],
           profile["lhdcv3_encoder_table_va"]))


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("library", help="the stock libbluetooth_jni.so")
    parser.add_argument("--format", choices=("json", "shell", "header"),
                        default="json")
    parser.add_argument("--output", help="write here instead of stdout")
    arguments = parser.parse_args()
    try:
        profile = derive(arguments.library)
    except DeriveError as error:
        sys.stderr.write(
            "cannot derive the libbluetooth_jni.so profile: %s\n" % error)
        return 1
    if arguments.format == "json":
        rendered = json.dumps(profile, indent=2, sort_keys=True) + "\n"
    elif arguments.format == "shell":
        rendered = as_shell(profile)
    else:
        rendered = as_header(profile)
    if arguments.output:
        with open(arguments.output, "w", encoding="utf-8") as handle:
            handle.write(rendered)
    else:
        sys.stdout.write(rendered)
    return 0


if __name__ == "__main__":
    sys.exit(main())
