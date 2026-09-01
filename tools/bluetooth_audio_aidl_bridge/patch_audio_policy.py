#!/usr/bin/env python3
"""Convert marble's QTI A2DP policy to the standard Bluetooth software HAL."""

from __future__ import annotations

import argparse
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path


DEVICE_TYPES = (
    "AUDIO_DEVICE_OUT_BLUETOOTH_A2DP",
    "AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES",
    "AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER",
)
DEVICE_TAGS = (
    "BT A2DP Out",
    "BT A2DP Headphones",
    "BT A2DP Speaker",
)
REQUIRED_RATES = (44100, 48000, 88200, 96000, 192000)


def fail(message: str) -> None:
    raise ValueError(message)


def parse_xml(text: str, source: Path) -> ET.Element:
    try:
        return ET.fromstring(text)
    except ET.ParseError as error:
        fail(f"invalid audio policy XML {source}: {error}")


def validate_xml(text: str, source: Path, *, allow_legacy: bool) -> None:
    root = parse_xml(text, source)
    primary_modules = [node for node in root.iter("module") if node.get("name") == "primary"]
    standard_modules = [
        node for node in root.iter("module") if node.get("name") == "bluetooth"
    ]
    legacy_modules = [
        node for node in root.iter("module") if node.get("name") == "bluetooth_qti"
    ]
    if len(primary_modules) != 1:
        fail(f"expected one primary module, found {len(primary_modules)}")
    if allow_legacy:
        if (len(standard_modules), len(legacy_modules)) not in ((1, 0), (0, 1)):
            fail(
                "expected one bluetooth or bluetooth_qti module, found "
                f"{len(standard_modules)} and {len(legacy_modules)}"
            )
    elif len(standard_modules) != 1 or legacy_modules:
        fail(
            "expected one bluetooth module and no bluetooth_qti module, found "
            f"{len(standard_modules)} and {len(legacy_modules)}"
        )

    is_legacy = bool(legacy_modules)
    module = (legacy_modules or standard_modules)[0]
    primary = primary_modules[0]
    expected_primary_entries = 1 if is_legacy else 0

    for device_type, device_tag in zip(DEVICE_TYPES, DEVICE_TAGS):
        primary_ports = [
            node
            for node in primary.iter("devicePort")
            if node.get("type") == device_type and node.get("role") == "sink"
        ]
        primary_routes = [
            node for node in primary.iter("route") if node.get("sink") == device_tag
        ]
        if len(primary_ports) != expected_primary_entries:
            fail(
                f"expected {expected_primary_entries} primary {device_type} ports, "
                f"found {len(primary_ports)}"
            )
        if len(primary_routes) != expected_primary_entries:
            fail(
                f"expected {expected_primary_entries} primary {device_tag} routes, "
                f"found {len(primary_routes)}"
            )

        ports = [
            node
            for node in module.iter("devicePort")
            if node.get("type") == device_type and node.get("role") == "sink"
        ]
        if len(ports) != 1:
            fail(f"expected one sink {device_type}, found {len(ports)}")
        if ports[0].get("tagName") != device_tag:
            fail(f"unexpected tagName for {device_type}: {ports[0].get('tagName')}")
        encoded_formats = ports[0].get("encodedFormats")
        if is_legacy and encoded_formats != "AUDIO_FORMAT_FORCE_AOSP":
            fail(f"unexpected legacy encodedFormats for {device_type}: {encoded_formats}")
        if not is_legacy and encoded_formats is not None:
            fail(f"standard {device_type} must not restrict encodedFormats")
        profiles = [
            node
            for node in ports[0].findall("profile")
            if node.get("format") == "AUDIO_FORMAT_PCM_16_BIT"
        ]
        if len(profiles) != 1 or not profiles[0].get("samplingRates"):
            fail(f"expected one PCM profile with samplingRates for {device_type}")
        try:
            rates = {
                int(value)
                for value in re.split(r"[\s,]+", profiles[0].get("samplingRates", "").strip())
                if value
            }
        except ValueError as error:
            fail(f"invalid samplingRates for {device_type}: {error}")
        required_rates = {44100, 48000} if allow_legacy else set(REQUIRED_RATES)
        missing_rates = required_rates - rates
        if missing_rates:
            fail(f"{device_type} is missing required rates: {sorted(missing_rates)}")
        module_routes = [
            node for node in module.iter("route") if node.get("sink") == device_tag
        ]
        if len(module_routes) != 1:
            fail(f"expected one {device_tag} route, found {len(module_routes)}")


def module_pattern(name: str) -> re.Pattern[str]:
    return re.compile(
        rf'(<module\b(?=[^>]*\bname="{re.escape(name)}")[^>]*>)(.*?)(</module>)',
        re.DOTALL,
    )


def remove_element_line(text: str, match: re.Match[str]) -> str:
    start = match.start()
    end = match.end()
    line_start = text.rfind("\n", 0, start) + 1
    if not text[line_start:start].strip():
        start = line_start
        if text.startswith("\r\n", end):
            end += 2
        elif text.startswith("\n", end):
            end += 1
    return text[:start] + text[end:]


def patch_primary_module(text: str, *, remove_legacy_routes: bool) -> str:
    matches = list(module_pattern("primary").finditer(text))
    if len(matches) != 1:
        fail(f"expected one textual primary module, found {len(matches)}")
    if not remove_legacy_routes:
        return text

    match = matches[0]
    body = match.group(2)
    for device_type, device_tag in zip(DEVICE_TYPES, DEVICE_TAGS):
        port_pattern = re.compile(
            rf'<devicePort\b(?=[^>]*\btype="{re.escape(device_type)}")'
            r'(?=[^>]*\brole="sink")(?:[^>]*/>|[^>]*>.*?</devicePort>)',
            re.DOTALL,
        )
        port_matches = list(port_pattern.finditer(body))
        if len(port_matches) != 1:
            fail(f"expected one textual primary {device_type}, found {len(port_matches)}")
        body = remove_element_line(body, port_matches[0])

        route_pattern = re.compile(
            rf'<route\b(?=[^>]*\bsink="{re.escape(device_tag)}")'
            r'(?:[^>]*/>|[^>]*>.*?</route>)',
            re.DOTALL,
        )
        route_matches = list(route_pattern.finditer(body))
        if len(route_matches) != 1:
            fail(f"expected one textual primary {device_tag} route, found {len(route_matches)}")
        body = remove_element_line(body, route_matches[0])

    replacement = match.group(1) + body + match.group(3)
    return text[: match.start()] + replacement + text[match.end() :]


def patch_policy(text: str) -> str:
    standard_matches = list(module_pattern("bluetooth").finditer(text))
    legacy_matches = list(module_pattern("bluetooth_qti").finditer(text))
    if (len(standard_matches), len(legacy_matches)) not in ((1, 0), (0, 1)):
        fail(
            "expected one textual bluetooth or bluetooth_qti module, found "
            f"{len(standard_matches)} and {len(legacy_matches)}"
        )
    is_legacy = bool(legacy_matches)
    match = (legacy_matches or standard_matches)[0]
    opening = match.group(1)
    body = match.group(2)
    if is_legacy:
        opening, count = re.subn(
            r'(\bname=")bluetooth_qti(")', r'\1bluetooth\2', opening, count=1
        )
        if count != 1:
            fail("failed to rename bluetooth_qti module")

    for device_type, device_tag in zip(DEVICE_TYPES, DEVICE_TAGS):
        port_pattern = re.compile(
            rf'(<devicePort\b(?=[^>]*\btype="{re.escape(device_type)}")'
            rf'(?=[^>]*\btagName="{re.escape(device_tag)}")'
            r'(?=[^>]*\brole="sink")[^>]*>)'
            r'(.*?)'
            r'(</devicePort>)',
            re.DOTALL,
        )
        port_matches = list(port_pattern.finditer(body))
        if len(port_matches) != 1:
            fail(f"expected one textual {device_type}, found {len(port_matches)}")
        port_match = port_matches[0]
        port = port_match.group(0)
        if is_legacy:
            tag_end = port.find(">") + 1
            port_opening, count = re.subn(
                r'\s+encodedFormats="AUDIO_FORMAT_FORCE_AOSP"',
                "",
                port[:tag_end],
                count=1,
            )
            if count != 1:
                fail(f"expected legacy encodedFormats for {device_type}")
            port = port_opening + port[tag_end:]
        rates_pattern = re.compile(r'(\bsamplingRates=")([^"]+)(")')
        rates_matches = list(rates_pattern.finditer(port))
        if len(rates_matches) != 1:
            fail(f"expected one samplingRates attribute for {device_type}")
        rates_match = rates_matches[0]
        try:
            existing = {
                int(value)
                for value in re.split(r"[\s,]+", rates_match.group(2).strip())
                if value
            }
        except ValueError as error:
            fail(f"invalid samplingRates for {device_type}: {error}")
        missing_baseline = {44100, 48000} - existing
        if missing_baseline:
            fail(f"{device_type} is missing baseline rates: {sorted(missing_baseline)}")
        updated = sorted(existing | set(REQUIRED_RATES))
        replacement = rates_match.group(1) + ",".join(map(str, updated)) + rates_match.group(3)
        patched_port = port[: rates_match.start()] + replacement + port[rates_match.end() :]
        body = body[: port_match.start()] + patched_port + body[port_match.end() :]

    replacement = opening + body + match.group(3)
    text = text[: match.start()] + replacement + text[match.end() :]
    return patch_primary_module(text, remove_legacy_routes=is_legacy)


def transform_policy(text: str, source: Path) -> str:
    validate_xml(text, source, allow_legacy=True)
    patched = patch_policy(text)
    validate_xml(patched, source, allow_legacy=False)
    return patched


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path, help="extracted audio_policy_configuration.xml")
    parser.add_argument("output", type=Path, help="patched XML to package")
    args = parser.parse_args()

    raw = args.input.read_bytes()
    if raw.startswith(b"\xef\xbb\xbf"):
        fail("UTF-8 BOM is not supported for the audio policy input")
    text = raw.decode("utf-8")
    patched = transform_policy(text, args.input)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(patched.encode("utf-8"))
    print(f"Patched A2DP rates {','.join(map(str, REQUIRED_RATES))}: {args.output}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, UnicodeError, ValueError) as error:
        print(error, file=sys.stderr)
        raise SystemExit(1) from error
