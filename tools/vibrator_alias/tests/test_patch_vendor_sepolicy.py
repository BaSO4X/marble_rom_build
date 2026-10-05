import sys
import tempfile
import unittest
from pathlib import Path


MODULE_DIR = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(MODULE_DIR))

from patch_vendor_sepolicy import (  # noqa: E402
    EXPECTED_RULE,
    append_rule,
    invalidate_precompiled_policy,
    read_rule,
    validate_selinux_root,
)


class PatchVendorSepolicyTest(unittest.TestCase):
    def test_rule_is_idempotent_and_precompiled_files_are_removed(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            vendor = root / "vendor/etc/selinux"
            odm = root / "odm/etc/selinux"
            vendor.mkdir(parents=True)
            odm.mkdir(parents=True)

            policy = vendor / "vendor_sepolicy.cil"
            rule_file = root / "vendor.vibrator-default-alias.cil"
            policy.write_text("(type test_type)\n", encoding="utf-8", newline="\n")
            rule_file.write_text(f"{EXPECTED_RULE}\n", encoding="utf-8", newline="\n")
            (vendor / "precompiled_sepolicy").write_bytes(b"vendor")
            (vendor / "precompiled_sepolicy.apex_sepolicy.sha256").write_text("hash\n")
            (odm / "precompiled_sepolicy").write_bytes(b"odm")
            (odm / "precompiled_sepolicy.plat_sepolicy_and_mapping.sha256").write_text("hash\n")

            rule = read_rule(rule_file)
            self.assertTrue(append_rule(policy, rule))
            self.assertFalse(append_rule(policy, rule))
            removed = invalidate_precompiled_policy(vendor)
            removed.extend(invalidate_precompiled_policy(odm))

            self.assertEqual(4, len(removed))
            self.assertEqual(
                1,
                policy.read_text(encoding="utf-8").splitlines().count(rule),
            )
            self.assertEqual([], list(vendor.glob("precompiled_sepolicy*")))
            self.assertEqual([], list(odm.glob("precompiled_sepolicy*")))

    def test_unexpected_root_is_rejected(self) -> None:
        with self.assertRaises(ValueError):
            validate_selinux_root(Path("/tmp/not-selinux"))


if __name__ == "__main__":
    unittest.main()
