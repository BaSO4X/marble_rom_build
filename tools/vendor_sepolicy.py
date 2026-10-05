#!/usr/bin/env python3

from collections.abc import Sequence
from pathlib import Path


def validate_selinux_root(path: Path) -> None:
    if path.name != "selinux" or path.parent.name != "etc":
        raise ValueError(f"refusing unexpected SELinux root: {path}")


def read_rules(
    rule_file: Path, expected_rules: Sequence[str], component: str
) -> list[str]:
    if not rule_file.is_file():
        raise FileNotFoundError(f"{component} policy fragment not found: {rule_file}")
    rules = [
        line.strip()
        for line in rule_file.read_text(encoding="utf-8").splitlines()
        if line.strip()
    ]
    if rules != list(expected_rules):
        raise ValueError(f"unexpected {component} policy fragment: {rule_file}")
    return rules


def read_single_rule(rule_file: Path, expected_rule: str, component: str) -> str:
    return read_rules(rule_file, [expected_rule], component)[0]


def append_rules(policy: Path, rules: Sequence[str]) -> bool:
    if not policy.is_file():
        raise FileNotFoundError(f"vendor policy not found: {policy}")

    text = policy.read_text(encoding="utf-8")
    existing_rules = {line.strip() for line in text.splitlines()}
    missing_rules = [rule for rule in rules if rule not in existing_rules]
    if not missing_rules:
        return False

    with policy.open("a", encoding="utf-8", newline="\n") as output:
        if text and not text.endswith("\n"):
            output.write("\n")
        for rule in missing_rules:
            output.write(f"{rule}\n")
    return True


def append_rule(policy: Path, rule: str) -> bool:
    return append_rules(policy, [rule])


def invalidate_precompiled_policy(root: Path) -> list[Path]:
    validate_selinux_root(root)
    if not root.exists():
        return []
    if not root.is_dir():
        raise NotADirectoryError(f"SELinux root is not a directory: {root}")

    candidates = [root / "precompiled_sepolicy"]
    candidates.extend(root.glob("precompiled_sepolicy.*.sha256"))
    removed = []
    for candidate in sorted(set(candidates)):
        if candidate.is_file() or candidate.is_symlink():
            candidate.unlink()
            removed.append(candidate)
    return removed


def patch_vendor_policy(
    vendor_policy: Path,
    rule_file: Path,
    selinux_roots: list[Path],
    expected_rule: str,
    component: str,
) -> tuple[bool, list[Path]]:
    return patch_vendor_policy_fragment(
        vendor_policy,
        rule_file,
        selinux_roots,
        [expected_rule],
        component,
    )


def patch_vendor_policy_fragment(
    vendor_policy: Path,
    rule_file: Path,
    selinux_roots: list[Path],
    expected_rules: Sequence[str],
    component: str,
) -> tuple[bool, list[Path]]:
    policy_parent = vendor_policy.parent
    validate_selinux_root(policy_parent)
    if not any(policy_parent.resolve() == root.resolve() for root in selinux_roots):
        raise ValueError("vendor policy parent is not one of the SELinux roots")

    rules = read_rules(rule_file, expected_rules, component)
    changed = append_rules(vendor_policy, rules)
    removed = []
    for root in selinux_roots:
        removed.extend(invalidate_precompiled_policy(root))

    policy_rules = {
        line.strip()
        for line in vendor_policy.read_text(encoding="utf-8").splitlines()
    }
    if any(rule not in policy_rules for rule in rules):
        raise RuntimeError(f"{component} SELinux rule validation failed")

    leftovers = []
    for root in selinux_roots:
        if root.is_dir():
            leftovers.extend(
                path
                for path in root.glob("precompiled_sepolicy*")
                if path.is_file()
            )
    if leftovers:
        raise RuntimeError(
            "precompiled SELinux policy was not fully invalidated: "
            + ", ".join(str(path) for path in leftovers)
        )
    return changed, removed
