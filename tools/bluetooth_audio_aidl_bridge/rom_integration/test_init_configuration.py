#!/usr/bin/env python3

from __future__ import annotations

import unittest
from pathlib import Path


BRIDGE_DIR = Path(__file__).resolve().parent.parent
ROM_INTEGRATION_DIR = Path(__file__).resolve().parent


class InitConfigurationTest(unittest.TestCase):
    def test_apex_patch_uses_platform_ready_property(self) -> None:
        config = (
            ROM_INTEGRATION_DIR / "bluetooth_audio_aidl_apex_patch.rc"
        ).read_text()

        self.assertIn("on property:apex.all.ready=true", config)
        self.assertNotIn("apexd.status", config)
        self.assertIn(
            "mount none /system/lib64/libbluetooth_jni.marble.so "
            "/apex/com.android.bt/lib64/libbluetooth_jni.so bind",
            config,
        )

    def test_bridge_starts_with_hal_class(self) -> None:
        service_config = (
            BRIDGE_DIR / "android.hardware.bluetooth.audio-service.marble.rc"
        ).read_text()

        self.assertIn("    class hal", service_config)
        self.assertNotIn("    disabled", service_config)
        self.assertNotIn("    oneshot", service_config)

    def test_init_can_bind_mount_patched_apex_library(self) -> None:
        policy_fragment = (
            ROM_INTEGRATION_DIR / "vendor.bluetooth-audio-aidl-bridge.cil"
        ).read_text()

        self.assertIn(
            "(allow init system_lib_file (file (mounton)))",
            policy_fragment,
        )

    def test_rom_disables_a2dp_offload_at_capability(self) -> None:
        service_config = (
            BRIDGE_DIR / "android.hardware.bluetooth.audio-service.marble.rc"
        ).read_text()
        install_script = (ROM_INTEGRATION_DIR / "install.sh").read_text()

        self.assertNotIn("persist.bluetooth.a2dp_offload.disabled", service_config)
        self.assertIn(
            "ro.bluetooth.a2dp_offload.supported false", install_script
        )
        self.assertNotIn(
            "persist.bluetooth.a2dp_offload.disabled true", install_script
        )


if __name__ == "__main__":
    unittest.main()
