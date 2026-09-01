#!/usr/bin/env python3

from __future__ import annotations

import unittest
import xml.etree.ElementTree as ET
from pathlib import Path

from patch_audio_policy import DEVICE_TAGS, DEVICE_TYPES, transform_policy


LEGACY_POLICY = """\
<?xml version="1.0" encoding="UTF-8"?>
<audioPolicyConfiguration>
    <modules>
        <module name="primary" halVersion="2.0">
            <devicePorts>
                <devicePort tagName="Speaker" type="AUDIO_DEVICE_OUT_SPEAKER" role="sink"/>
                <devicePort tagName="BT A2DP Out" type="AUDIO_DEVICE_OUT_BLUETOOTH_A2DP" role="sink"/>
                <devicePort tagName="BT A2DP Headphones" type="AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES" role="sink">
                    <profile name="" format="AUDIO_FORMAT_PCM_16_BIT" samplingRates="44100,48000,96000"/>
                </devicePort>
                <devicePort tagName="BT A2DP Speaker" type="AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER" role="sink">
                    <profile name="" format="AUDIO_FORMAT_PCM_16_BIT" samplingRates="44100,48000,96000"/>
                </devicePort>
                <devicePort tagName="A2DP In" type="AUDIO_DEVICE_IN_BLUETOOTH_A2DP" role="source"/>
            </devicePorts>
            <routes>
                <route type="mix" sink="Speaker" sources="primary output"/>
                <route type="mix" sink="BT A2DP Out" sources="primary output,deep_buffer"></route>
                <route type="mix" sink="BT A2DP Headphones" sources="primary output,deep_buffer"/>
                <route type="mix" sink="BT A2DP Speaker" sources="primary output,deep_buffer"/>
            </routes>
        </module>
        <module name="bluetooth_qti" halVersion="2.0">
            <devicePorts>
                <devicePort tagName="BT A2DP Out" type="AUDIO_DEVICE_OUT_BLUETOOTH_A2DP" role="sink"
                            encodedFormats="AUDIO_FORMAT_FORCE_AOSP">
                    <profile name="" format="AUDIO_FORMAT_PCM_16_BIT" samplingRates="44100,48000,96000"/>
                </devicePort>
                <devicePort tagName="BT A2DP Headphones" type="AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES" role="sink"
                            encodedFormats="AUDIO_FORMAT_FORCE_AOSP">
                    <profile name="" format="AUDIO_FORMAT_PCM_16_BIT" samplingRates="44100,48000,96000"/>
                </devicePort>
                <devicePort tagName="BT A2DP Speaker" type="AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER" role="sink"
                            encodedFormats="AUDIO_FORMAT_FORCE_AOSP">
                    <profile name="" format="AUDIO_FORMAT_PCM_16_BIT" samplingRates="44100,48000,96000"/>
                </devicePort>
            </devicePorts>
            <routes>
                <route type="mix" sink="BT A2DP Out" sources="a2dp output"/>
                <route type="mix" sink="BT A2DP Headphones" sources="a2dp output"/>
                <route type="mix" sink="BT A2DP Speaker" sources="a2dp output"/>
            </routes>
        </module>
    </modules>
</audioPolicyConfiguration>
"""


class PatchAudioPolicyTest(unittest.TestCase):
    def test_converts_marble_qti_policy(self) -> None:
        patched = transform_policy(LEGACY_POLICY, Path("legacy.xml"))
        root = ET.fromstring(patched)
        primary = root.find('.//module[@name="primary"]')
        standard = root.find('.//module[@name="bluetooth"]')

        self.assertIsNotNone(primary)
        self.assertIsNotNone(standard)
        self.assertIsNone(root.find('.//module[@name="bluetooth_qti"]'))
        self.assertIsNotNone(primary.find('.//devicePort[@type="AUDIO_DEVICE_IN_BLUETOOTH_A2DP"]'))
        self.assertIsNotNone(primary.find('.//devicePort[@type="AUDIO_DEVICE_OUT_SPEAKER"]'))

        for device_type, device_tag in zip(DEVICE_TYPES, DEVICE_TAGS):
            self.assertIsNone(primary.find(f'.//devicePort[@type="{device_type}"]'))
            self.assertIsNone(primary.find(f'.//route[@sink="{device_tag}"]'))
            port = standard.find(f'.//devicePort[@type="{device_type}"]')
            self.assertIsNotNone(port)
            self.assertNotIn("encodedFormats", port.attrib)
            self.assertEqual(
                port.find('profile[@format="AUDIO_FORMAT_PCM_16_BIT"]').get(
                    "samplingRates"
                ),
                "44100,48000,88200,96000,192000",
            )
            self.assertIsNotNone(standard.find(f'.//route[@sink="{device_tag}"]'))

    def test_already_converted_policy_is_unchanged(self) -> None:
        patched = transform_policy(LEGACY_POLICY, Path("legacy.xml"))
        self.assertEqual(transform_policy(patched, Path("patched.xml")), patched)

    def test_adds_rates_to_already_converted_policy(self) -> None:
        patched = transform_policy(LEGACY_POLICY, Path("legacy.xml"))
        missing_high_rates = patched.replace(
            "44100,48000,88200,96000,192000", "44100,48000,96000"
        )
        self.assertEqual(
            transform_policy(missing_high_rates, Path("converted.xml")), patched
        )

    def test_rejects_missing_bluetooth_module(self) -> None:
        invalid = LEGACY_POLICY.replace('name="bluetooth_qti"', 'name="unrelated"')
        with self.assertRaisesRegex(ValueError, "expected one bluetooth or bluetooth_qti"):
            transform_policy(invalid, Path("invalid.xml"))

    def test_rejects_partially_converted_policy(self) -> None:
        invalid = LEGACY_POLICY.replace('name="bluetooth_qti"', 'name="bluetooth"')
        with self.assertRaisesRegex(ValueError, "expected 0 primary"):
            transform_policy(invalid, Path("partial.xml"))


if __name__ == "__main__":
    unittest.main()
