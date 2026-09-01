import sys
import tempfile
import unittest
from pathlib import Path


MODULE_DIR = Path(__file__).resolve().parent
TOOLS_DIR = MODULE_DIR.parents[1]
sys.path.insert(0, str(MODULE_DIR))
sys.path.insert(0, str(TOOLS_DIR))

from patch_vendor_sepolicy import EXPECTED_RULES, read_rule  # noqa: E402
from vendor_sepolicy import patch_vendor_policy_fragment  # noqa: E402


class BluetoothAudioSepolicyPatchTest(unittest.TestCase):
    def test_rule_is_idempotent_and_precompiled_files_are_removed(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            vendor = root / "vendor/etc/selinux"
            odm = root / "odm/etc/selinux"
            vendor.mkdir(parents=True)
            odm.mkdir(parents=True)

            policy = vendor / "vendor_sepolicy.cil"
            rule_file = root / "vendor.bluetooth-audio-aidl-bridge.cil"
            policy.write_text("(type test_type)\n", encoding="utf-8", newline="\n")
            rule_file.write_text(
                "\n".join(EXPECTED_RULES) + "\n",
                encoding="utf-8",
                newline="\n",
            )
            (vendor / "precompiled_sepolicy").write_bytes(b"vendor")
            (odm / "precompiled_sepolicy.plat.sha256").write_text("hash\n")

            changed, removed = patch_vendor_policy_fragment(
                policy,
                rule_file,
                [vendor, odm],
                EXPECTED_RULES,
                "Bluetooth Audio",
            )
            self.assertTrue(changed)
            self.assertEqual(2, len(removed))

            changed, removed = patch_vendor_policy_fragment(
                policy,
                rule_file,
                [vendor, odm],
                EXPECTED_RULES,
                "Bluetooth Audio",
            )
            self.assertFalse(changed)
            self.assertEqual([], removed)
            policy_lines = policy.read_text(encoding="utf-8").splitlines()
            for rule in EXPECTED_RULES:
                self.assertEqual(1, policy_lines.count(rule))

    def test_unexpected_fragment_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            rule_file = Path(temporary) / "unexpected.cil"
            rule_file.write_text("(allow domain domain (binder (call)))\n")
            with self.assertRaises(ValueError):
                read_rule(rule_file)

    def test_unknown_precompiled_policy_file_fails_closed(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            vendor = root / "vendor/etc/selinux"
            vendor.mkdir(parents=True)

            policy = vendor / "vendor_sepolicy.cil"
            rule_file = root / "vendor.bluetooth-audio-aidl-bridge.cil"
            policy.write_text("(type test_type)\n", encoding="utf-8", newline="\n")
            rule_file.write_text(
                "\n".join(EXPECTED_RULES) + "\n",
                encoding="utf-8",
                newline="\n",
            )
            (vendor / "precompiled_sepolicy.unexpected").write_bytes(b"unknown")

            with self.assertRaises(RuntimeError):
                patch_vendor_policy_fragment(
                    policy,
                    rule_file,
                    [vendor],
                    EXPECTED_RULES,
                    "Bluetooth Audio",
                )


if __name__ == "__main__":
    unittest.main()
