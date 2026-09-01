#!/usr/bin/env python3

from __future__ import annotations

import copy
import hashlib
import json
from pathlib import Path
import tempfile
import unittest
from unittest import mock

from tools.ac4_compat import apply as ac4_apply
from tools.ac4_compat.apply import CompatError, apply_compat


SOURCE_SONAME = b"libstagefright_soft_ac4dec.so"
TARGET_SONAME = b"libstagefright_soft_ac4src.so"
SONAME_OFFSET = 32


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def commented_xml() -> bytes:
    return b"""<?xml version="1.0" encoding="utf-8"?>
<Included>
  <Decoders>
    <!-- DOLBY_AC4 -->
    <!-- MediaCodec type="audio/ac4" name="OMX.dolby.ac4.decoder">
      <Limit name="channel-count" max="2" />
    </MediaCodec -->
    <!-- DOLBY_AC4 END -->
  </Decoders>
</Included>
"""


class ApplyCompatTest(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary.name)
        self.vendor = self.root / "vendor"
        (self.vendor / "lib").mkdir(parents=True)
        (self.vendor / "etc").mkdir()
        (self.vendor / "build.prop").write_text(
            "ro.vendor.build.version.sdk=32\n", encoding="utf-8"
        )
        self.source = b"\x7fELF" + bytes(SONAME_OFFSET - 4) + SOURCE_SONAME + b"fixture"
        renamed = bytearray(self.source)
        renamed[SONAME_OFFSET : SONAME_OFFSET + len(SOURCE_SONAME)] = TARGET_SONAME
        self.renamed = bytes(renamed)
        self.wrapper = b"verified-wrapper-fixture"
        self.wrapper_path = self.root / "wrapper.so"
        self.wrapper_path.write_bytes(self.wrapper)
        self.profile = {
            "id": "fixture",
            "device": "marble",
            "vendor_sdk": 32,
            "runtime_sdk_min": 35,
            "runtime_sdk_max": None,
            "decoder_path": "lib/libstagefright_soft_ac4dec.so",
            "renamed_path": "lib/libstagefright_soft_ac4src.so",
            "xml_path": "etc/media_codecs_dolby_audio.xml",
            "source_sha256": digest(self.source),
            "renamed_sha256": digest(self.renamed),
            "source_soname": SOURCE_SONAME.decode(),
            "target_soname": TARGET_SONAME.decode(),
            "soname_offset": SONAME_OFFSET,
            "private_abi": "splitsec-v1",
            "table_id": "marble-splitsec-tables-v1",
            "wrapper_sha256": digest(self.wrapper),
        }
        self.profile_path = self.root / "profiles.json"
        self.write_profiles([self.profile])
        self.decoder_path = self.vendor / self.profile["decoder_path"]
        self.renamed_path = self.vendor / self.profile["renamed_path"]
        self.xml_path = self.vendor / self.profile["xml_path"]
        self.decoder_path.write_bytes(self.source)
        self.xml_path.write_bytes(commented_xml())

    def tearDown(self) -> None:
        self.temporary.cleanup()

    def write_profiles(self, profiles: list[dict[str, object]]) -> None:
        self.profile_path.write_text(
            json.dumps({"schema": 1, "profiles": profiles}), encoding="utf-8"
        )

    def apply(self, runtime_sdk: int = 35) -> str:
        return apply_compat(
            self.vendor, self.wrapper_path, self.profile_path, runtime_sdk, "marble"
        )

    def test_applies_source_blob_and_commented_xml(self) -> None:
        result = self.apply()
        self.assertIn("source-path:source", result)
        self.assertEqual(self.decoder_path.read_bytes(), self.wrapper)
        self.assertEqual(self.renamed_path.read_bytes(), self.renamed)
        xml = self.xml_path.read_text(encoding="utf-8")
        self.assertIn('<MediaCodec type="audio/ac4" name="OMX.dolby.ac4.decoder">', xml)
        self.assertNotIn("</MediaCodec -->", xml)

    def test_is_idempotent_with_renamed_blob_and_active_xml(self) -> None:
        self.apply()
        result = self.apply(runtime_sdk=99)
        self.assertIn("renamed-path:renamed", result)
        self.assertEqual(self.decoder_path.read_bytes(), self.wrapper)
        self.assertEqual(self.renamed_path.read_bytes(), self.renamed)

    def test_rejects_unknown_blob(self) -> None:
        self.decoder_path.write_bytes(b"unknown")
        with self.assertRaisesRegex(CompatError, "found 0"):
            self.apply()

    def test_rejects_wrapper_mismatch(self) -> None:
        self.wrapper_path.write_bytes(b"wrong-wrapper")
        with self.assertRaisesRegex(CompatError, "wrapper hash"):
            self.apply()

    def test_rejects_runtime_below_profile(self) -> None:
        with self.assertRaisesRegex(CompatError, "found 0"):
            self.apply(runtime_sdk=34)

    def test_rejects_wrong_vendor_sdk(self) -> None:
        (self.vendor / "build.prop").write_text(
            "ro.vendor.build.version.sdk=33\n", encoding="utf-8"
        )
        with self.assertRaisesRegex(CompatError, "found 0"):
            self.apply()

    def test_rejects_wrong_device(self) -> None:
        with self.assertRaisesRegex(CompatError, "found 0"):
            apply_compat(
                self.vendor,
                self.wrapper_path,
                self.profile_path,
                35,
                "other-device",
            )

    def test_rejects_duplicate_profile_match(self) -> None:
        duplicate = copy.deepcopy(self.profile)
        duplicate["id"] = "fixture-duplicate"
        self.write_profiles([self.profile, duplicate])
        with self.assertRaisesRegex(CompatError, "found 2"):
            self.apply()

    def test_rejects_duplicate_xml_declaration(self) -> None:
        self.xml_path.write_bytes(
            commented_xml().replace(
                b"  </Decoders>",
                b'    <MediaCodec name="OMX.dolby.ac4.decoder" type="audio/ac4">\n'
                b"    </MediaCodec>\n  </Decoders>",
            )
        )
        with self.assertRaisesRegex(CompatError, "duplicate or ambiguous"):
            self.apply()

    def test_rejects_unclosed_xml_block(self) -> None:
        self.xml_path.write_bytes(commented_xml().replace(b"</MediaCodec -->", b""))
        with self.assertRaisesRegex(CompatError, "not uniquely closed"):
            self.apply()

    def test_rolls_back_if_install_is_interrupted(self) -> None:
        original_xml = self.xml_path.read_bytes()
        original_write = ac4_apply.atomic_write
        calls = 0

        def fail_second_write(path: Path, data: bytes, mode: int) -> None:
            nonlocal calls
            calls += 1
            if calls == 2:
                raise CompatError("injected write failure")
            original_write(path, data, mode)

        with mock.patch.object(ac4_apply, "atomic_write", side_effect=fail_second_write):
            with self.assertRaisesRegex(CompatError, "injected write failure"):
                self.apply()
        self.assertEqual(self.decoder_path.read_bytes(), self.source)
        self.assertFalse(self.renamed_path.exists())
        self.assertEqual(self.xml_path.read_bytes(), original_xml)


if __name__ == "__main__":
    unittest.main()
