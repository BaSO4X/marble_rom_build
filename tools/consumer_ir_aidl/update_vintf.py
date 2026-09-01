#!/usr/bin/env python3
"""Replace the active marble Consumer IR HIDL declaration with AIDL V1."""

from __future__ import annotations

import re
import sys
from pathlib import Path
from xml.etree import ElementTree


HAL_BLOCK = re.compile(
    r"(?ms)^(?P<indent>[ \t]*)<hal\b.*?</hal>[ \t]*(?:\r?\n|$)"
)
IR_NAME = re.compile(r"<name>\s*android\.hardware\.ir\s*</name>")


def fail(message: str) -> None:
    raise SystemExit(f"Consumer IR VINTF update failed: {message}")


def main() -> None:
    if len(sys.argv) != 2:
        fail("usage: update_vintf.py MANIFEST_XML")

    manifest = Path(sys.argv[1])
    text = manifest.read_text(encoding="utf-8")
    try:
        ElementTree.fromstring(text)
    except ElementTree.ParseError as error:
        fail(f"input is not valid XML: {error}")

    matches = [match for match in HAL_BLOCK.finditer(text) if IR_NAME.search(match.group())]
    if len(matches) != 1:
        fail(f"expected one android.hardware.ir HAL block, found {len(matches)}")

    match = matches[0]
    indent = match.group("indent")
    replacement = (
        f'{indent}<hal format="aidl">\n'
        f"{indent}    <name>android.hardware.ir</name>\n"
        f"{indent}    <version>1</version>\n"
        f"{indent}    <fqname>IConsumerIr/default</fqname>\n"
        f"{indent}</hal>\n"
    )
    updated = text[: match.start()] + replacement + text[match.end() :]

    try:
        root = ElementTree.fromstring(updated)
    except ElementTree.ParseError as error:
        fail(f"output is not valid XML: {error}")

    ir_hals = [hal for hal in root.findall("hal") if hal.findtext("name") == "android.hardware.ir"]
    if len(ir_hals) != 1:
        fail(f"output contains {len(ir_hals)} android.hardware.ir HAL blocks")
    ir_hal = ir_hals[0]
    if (
        ir_hal.get("format") != "aidl"
        or ir_hal.findtext("version") != "1"
        or ir_hal.findtext("fqname") != "IConsumerIr/default"
    ):
        fail("output AIDL declaration is incomplete")

    if updated != text:
        manifest.write_text(updated, encoding="utf-8", newline="\n")
        print(f"Updated {manifest}")
    else:
        print(f"Already updated {manifest}")


if __name__ == "__main__":
    main()
