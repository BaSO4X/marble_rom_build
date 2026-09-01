#!/usr/bin/env python3

import argparse
import sys
from pathlib import Path


TOOLS_DIR = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(TOOLS_DIR))

from vendor_sepolicy import (  # noqa: E402
    append_rule,
    invalidate_precompiled_policy,
    patch_vendor_policy,
    read_single_rule,
    validate_selinux_root,
)


EXPECTED_RULE = "(allow hal_vibrator_default hal_vibrator_default (binder (call transfer)))"


def read_rule(rule_file: Path) -> str:
    return read_single_rule(rule_file, EXPECTED_RULE, "vibrator")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Add the vibrator Binder rule and invalidate stale split policy"
    )
    parser.add_argument("vendor_policy", type=Path)
    parser.add_argument("rule_file", type=Path)
    parser.add_argument("selinux_roots", nargs="+", type=Path)
    args = parser.parse_args()

    changed, removed = patch_vendor_policy(
        args.vendor_policy,
        args.rule_file,
        args.selinux_roots,
        EXPECTED_RULE,
        "vibrator",
    )

    print(f"vibrator_sepolicy_rule={'added' if changed else 'present'}")
    print(f"removed_precompiled_files={len(removed)}")


if __name__ == "__main__":
    main()
