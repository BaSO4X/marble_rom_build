#!/usr/bin/env python3
"""Apply the verified marble AC-4 OMX compatibility files to an unpacked vendor."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import sys
import tempfile
from typing import Any
from xml.etree import ElementTree


SUPPORTED_PRIVATE_ABI = "splitsec-v1"
SUPPORTED_TABLE_ID = "marble-splitsec-tables-v1"
HASH_RE = re.compile(r"[0-9a-f]{64}")
NAME_RE = re.compile(r"\bname\s*=\s*['\"]OMX[.]dolby[.]ac4[.]decoder['\"]")
TYPE_RE = re.compile(r"\btype\s*=\s*['\"]audio/ac4['\"]")
COMMENTED_OPEN_RE = re.compile(r"^([ \t]*)<!--[ \t]*<?MediaCodec\b")
ACTIVE_OPEN_RE = re.compile(r"^[ \t]*<MediaCodec\b")
COMMENTED_CLOSE_RE = re.compile(r"</MediaCodec[ \t]*-->")
ACTIVE_CLOSE_RE = re.compile(r"</MediaCodec[ \t]*>")


class CompatError(RuntimeError):
    """Raised when the input does not match one verified compatibility profile."""


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def checked_relative_path(root: Path, value: str, field: str) -> Path:
    relative = PurePosixPath(value)
    if relative.is_absolute() or ".." in relative.parts or not relative.parts:
        raise CompatError(f"profile field {field} is not a safe relative path: {value}")
    return root.joinpath(*relative.parts)


def require_hash(profile: dict[str, Any], field: str) -> None:
    value = profile.get(field)
    if not isinstance(value, str) or HASH_RE.fullmatch(value) is None:
        raise CompatError(f"profile {profile.get('id', '<unknown>')} has invalid {field}")


def validate_profile(profile: dict[str, Any]) -> None:
    profile_id = profile.get("id")
    if not isinstance(profile_id, str) or not profile_id:
        raise CompatError("profile id must be a non-empty string")
    if not isinstance(profile.get("device"), str) or not profile["device"]:
        raise CompatError(f"profile {profile_id} has invalid device")
    for field in ("source_sha256", "renamed_sha256", "wrapper_sha256"):
        require_hash(profile, field)
    if profile["source_sha256"] == profile["renamed_sha256"]:
        raise CompatError(f"profile {profile_id} source and renamed hashes are identical")
    for field in ("decoder_path", "renamed_path", "xml_path"):
        value = profile.get(field)
        if not isinstance(value, str):
            raise CompatError(f"profile {profile_id} has invalid {field}")
        checked_relative_path(Path("/profile-root"), value, field)
    if profile["decoder_path"] == profile["renamed_path"]:
        raise CompatError(f"profile {profile_id} decoder paths are identical")
    for field in ("source_soname", "target_soname"):
        value = profile.get(field)
        if not isinstance(value, str) or not value or not value.isascii():
            raise CompatError(f"profile {profile_id} has invalid {field}")
    if len(profile["source_soname"]) != len(profile["target_soname"]):
        raise CompatError(f"profile {profile_id} SONAME lengths differ")
    offset = profile.get("soname_offset")
    if not isinstance(offset, int) or isinstance(offset, bool) or offset < 0:
        raise CompatError(f"profile {profile_id} has invalid soname_offset")
    for field in ("vendor_sdk", "runtime_sdk_min"):
        value = profile.get(field)
        if not isinstance(value, int) or isinstance(value, bool) or value < 0:
            raise CompatError(f"profile {profile_id} has invalid {field}")
    maximum = profile.get("runtime_sdk_max")
    if maximum is not None and (
        not isinstance(maximum, int)
        or isinstance(maximum, bool)
        or maximum < profile["runtime_sdk_min"]
    ):
        raise CompatError(f"profile {profile_id} has invalid runtime_sdk_max")
    if profile.get("private_abi") != SUPPORTED_PRIVATE_ABI:
        raise CompatError(f"profile {profile_id} uses an unsupported private ABI")
    if profile.get("table_id") != SUPPORTED_TABLE_ID:
        raise CompatError(f"profile {profile_id} uses an unsupported table ID")


def load_profiles(path: Path) -> list[dict[str, Any]]:
    try:
        document = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise CompatError(f"cannot read compatibility profiles: {error}") from error
    if not isinstance(document, dict) or document.get("schema") != 1:
        raise CompatError("unsupported compatibility profile schema")
    profiles = document.get("profiles")
    if not isinstance(profiles, list) or not profiles:
        raise CompatError("compatibility profile list is empty")
    seen_ids: set[str] = set()
    for profile in profiles:
        if not isinstance(profile, dict):
            raise CompatError("compatibility profile entry is not an object")
        validate_profile(profile)
        profile_id = profile["id"]
        if profile_id in seen_ids:
            raise CompatError(f"duplicate compatibility profile id: {profile_id}")
        seen_ids.add(profile_id)
    return profiles


def read_property(path: Path, key: str) -> str:
    try:
        lines = path.read_text(encoding="utf-8").splitlines()
    except (OSError, UnicodeError) as error:
        raise CompatError(f"cannot read {path}: {error}") from error
    matches = [line.split("=", 1)[1] for line in lines if line.startswith(f"{key}=")]
    if len(matches) != 1 or not matches[0]:
        raise CompatError(f"expected exactly one {key} in {path}")
    return matches[0]


def find_profile(
    vendor_root: Path,
    profiles: list[dict[str, Any]],
    runtime_sdk: int,
    device: str,
) -> tuple[dict[str, Any], bytes, str]:
    try:
        vendor_sdk = int(
            read_property(vendor_root / "build.prop", "ro.vendor.build.version.sdk")
        )
    except ValueError as error:
        raise CompatError("vendor SDK property is not numeric") from error

    matches: list[tuple[dict[str, Any], bytes, str]] = []
    for profile in profiles:
        maximum = profile["runtime_sdk_max"]
        if profile["device"] != device or vendor_sdk != profile["vendor_sdk"]:
            continue
        if runtime_sdk < profile["runtime_sdk_min"]:
            continue
        if maximum is not None and runtime_sdk > maximum:
            continue
        for field, state in (("decoder_path", "source-path"), ("renamed_path", "renamed-path")):
            candidate = checked_relative_path(vendor_root, profile[field], field)
            if not candidate.is_file():
                continue
            try:
                data = candidate.read_bytes()
            except OSError as error:
                raise CompatError(f"cannot read decoder candidate {candidate}: {error}") from error
            digest = sha256_bytes(data)
            if digest == profile["source_sha256"]:
                matches.append((profile, data, f"{state}:source"))
            elif digest == profile["renamed_sha256"]:
                matches.append((profile, data, f"{state}:renamed"))
    if len(matches) != 1:
        raise CompatError(
            "expected exactly one verified AC-4 decoder/profile match, "
            f"found {len(matches)}"
        )
    return matches[0]


def prepare_decoder(profile: dict[str, Any], data: bytes, state: str) -> bytes:
    if state.endswith(":renamed"):
        prepared = data
    else:
        source = profile["source_soname"].encode("ascii")
        target = profile["target_soname"].encode("ascii")
        offset = profile["soname_offset"]
        end = offset + len(source)
        if end > len(data) or data[offset:end] != source:
            raise CompatError("decoder SONAME bytes do not match the selected profile offset")
        mutable = bytearray(data)
        mutable[offset:end] = target
        prepared = bytes(mutable)
    if sha256_bytes(prepared) != profile["renamed_sha256"]:
        raise CompatError("prepared decoder hash does not match the selected profile")
    return prepared


def is_ac4_codec_line(line: str) -> bool:
    return NAME_RE.search(line) is not None and TYPE_RE.search(line) is not None


def patch_codec_xml(data: bytes) -> bytes:
    try:
        text = data.decode("utf-8")
    except UnicodeDecodeError as error:
        raise CompatError(f"vendor codec XML is not UTF-8: {error}") from error
    lines = text.splitlines(keepends=True)
    candidates: list[tuple[int, str]] = []
    for index, line in enumerate(lines):
        if not is_ac4_codec_line(line):
            continue
        if COMMENTED_OPEN_RE.search(line):
            candidates.append((index, "commented"))
        elif ACTIVE_OPEN_RE.search(line):
            candidates.append((index, "active"))
        else:
            candidates.append((index, "ambiguous"))
    if len(candidates) != 1 or candidates[0][1] == "ambiguous":
        raise CompatError("vendor XML AC-4 codec declaration is missing, duplicate or ambiguous")

    opening_index, state = candidates[0]
    close_pattern = COMMENTED_CLOSE_RE if state == "commented" else ACTIVE_CLOSE_RE
    closing_indexes: list[int] = []
    for index in range(opening_index + 1, len(lines)):
        line = lines[index]
        if "DOLBY_AC4 END" in line:
            break
        if is_ac4_codec_line(line):
            break
        if close_pattern.search(line):
            closing_indexes.append(index)
    if len(closing_indexes) != 1:
        raise CompatError("vendor XML AC-4 codec block is not uniquely closed")

    if state == "commented":
        lines[opening_index] = COMMENTED_OPEN_RE.sub(
            lambda match: f"{match.group(1)}<MediaCodec",
            lines[opening_index],
            count=1,
        )
        closing_index = closing_indexes[0]
        lines[closing_index] = COMMENTED_CLOSE_RE.sub(
            "</MediaCodec>", lines[closing_index], count=1
        )

    patched = "".join(lines).encode("utf-8")
    try:
        root = ElementTree.fromstring(patched)
    except ElementTree.ParseError as error:
        raise CompatError(f"patched vendor codec XML is invalid: {error}") from error
    active = [
        element
        for element in root.iter("MediaCodec")
        if element.get("name") == "OMX.dolby.ac4.decoder"
        and element.get("type") == "audio/ac4"
    ]
    if len(active) != 1:
        raise CompatError(f"patched vendor XML has {len(active)} active AC-4 codecs")
    return patched


def atomic_write(path: Path, data: bytes, mode: int) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary_name: str | None = None
    try:
        with tempfile.NamedTemporaryFile(dir=path.parent, prefix=f".{path.name}.", delete=False) as file:
            temporary_name = file.name
            file.write(data)
            file.flush()
            os.fsync(file.fileno())
        os.chmod(temporary_name, mode)
        os.replace(temporary_name, path)
    except OSError as error:
        if temporary_name is not None:
            try:
                os.unlink(temporary_name)
            except OSError:
                pass
        raise CompatError(f"cannot install {path}: {error}") from error


def install_transaction(files: list[tuple[Path, bytes, int]]) -> None:
    backups: list[tuple[Path, bytes | None, int]] = []
    for path, _, mode in files:
        try:
            if path.exists():
                backups.append((path, path.read_bytes(), path.stat().st_mode & 0o7777))
            else:
                backups.append((path, None, mode))
        except OSError as error:
            raise CompatError(f"cannot back up {path}: {error}") from error
    try:
        for path, data, mode in files:
            atomic_write(path, data, mode)
    except CompatError as install_error:
        rollback_errors: list[str] = []
        for path, data, mode in reversed(backups):
            try:
                if data is None:
                    path.unlink(missing_ok=True)
                else:
                    atomic_write(path, data, mode)
            except (CompatError, OSError) as rollback_error:
                rollback_errors.append(f"{path}: {rollback_error}")
        if rollback_errors:
            raise CompatError(
                f"{install_error}; rollback also failed: {'; '.join(rollback_errors)}"
            ) from install_error
        raise


def apply_compat(
    vendor_root: Path,
    wrapper_path: Path,
    profile_path: Path,
    runtime_sdk: int,
    device: str,
) -> str:
    if runtime_sdk < 0:
        raise CompatError("runtime SDK must be non-negative")
    profiles = load_profiles(profile_path)
    try:
        wrapper = wrapper_path.read_bytes()
    except OSError as error:
        raise CompatError(f"cannot read AC-4 wrapper: {error}") from error
    if not device:
        raise CompatError("device must be non-empty")
    profile, decoder, state = find_profile(vendor_root, profiles, runtime_sdk, device)
    if sha256_bytes(wrapper) != profile["wrapper_sha256"]:
        raise CompatError("AC-4 wrapper hash does not match the selected profile")

    prepared_decoder = prepare_decoder(profile, decoder, state)
    xml_path = checked_relative_path(vendor_root, profile["xml_path"], "xml_path")
    try:
        xml_data = xml_path.read_bytes()
    except OSError as error:
        raise CompatError(f"cannot read vendor codec XML: {error}") from error
    patched_xml = patch_codec_xml(xml_data)

    renamed_path = checked_relative_path(vendor_root, profile["renamed_path"], "renamed_path")
    decoder_path = checked_relative_path(vendor_root, profile["decoder_path"], "decoder_path")
    install_transaction(
        [
            (renamed_path, prepared_decoder, 0o644),
            (decoder_path, wrapper, 0o644),
            (xml_path, patched_xml, 0o644),
        ]
    )
    return f"{profile['id']} ({state}, runtime SDK {runtime_sdk})"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--vendor-root", required=True, type=Path)
    parser.add_argument("--wrapper", required=True, type=Path)
    parser.add_argument("--profiles", required=True, type=Path)
    parser.add_argument("--runtime-sdk", required=True, type=int)
    parser.add_argument("--device", required=True)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        result = apply_compat(
            args.vendor_root.resolve(),
            args.wrapper.resolve(),
            args.profiles.resolve(),
            args.runtime_sdk,
            args.device,
        )
    except CompatError as error:
        print(f"AC-4 compatibility patch failed: {error}", file=sys.stderr)
        return 1
    print(f"Applied AC-4 compatibility profile: {result}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
