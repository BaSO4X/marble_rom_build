#!/usr/bin/env python3

import argparse
import sys
from pathlib import Path


TOOLS_DIR = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(TOOLS_DIR))

from vendor_sepolicy import patch_vendor_policy_fragment, read_rules  # noqa: E402


EXPECTED_RULES = [
    "(allow hal_audio_default hal_audio_default (binder (call transfer)))",
    "(allow init system_lib_file (file (mounton)))",
]


def read_rule(rule_file: Path) -> list[str]:
    return read_rules(rule_file, EXPECTED_RULES, "Bluetooth Audio")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Add Bluetooth Audio Binder and APEX mount rules and invalidate stale split policy"
    )
    parser.add_argument("vendor_policy", type=Path)
    parser.add_argument("rule_file", type=Path)
    parser.add_argument("selinux_roots", nargs="+", type=Path)
    args = parser.parse_args()

    changed, removed = patch_vendor_policy_fragment(
        args.vendor_policy,
        args.rule_file,
        args.selinux_roots,
        EXPECTED_RULES,
        "Bluetooth Audio",
    )
    print(f"bluetooth_audio_sepolicy_rule={'added' if changed else 'present'}")
    print(f"removed_precompiled_files={len(removed)}")


if __name__ == "__main__":
    main()
