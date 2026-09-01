# Bluetooth Audio AIDL compatibility bridge

This service exposes the stable Android Bluetooth Audio AIDL V3 factory and
forwards sessions to legacy Bluetooth Audio backends. A2DP software encoding
and software Hearing Aid sessions use the standard Android Bluetooth Audio
HIDL 2.0 provider. When present, the Qualcomm Bluetooth Audio HIDL 2.1 provider
supplies A2DP hardware offload; its absence no longer disables software audio.
Software sessions translate PCM configuration and duplicate the standard HIDL
FMQ descriptor into the AIDL FMQ descriptor consumed by the Android Bluetooth
stack.

The offload backend deliberately advertises only SBC and AAC. New or vendor
codecs supported by Android's software encoder, including native LHDC, use
the codec-agnostic standard PCM path and do not require a Qualcomm codec
mapping. Unsupported session types return no capabilities and a non-startable
placeholder provider so newer strict clients do not abort merely because an
older V3 service cannot open the session.

Providers are cached because the old HIDL factories do not reliably allow a
second open. A newer AIDL client generation therefore reclaims a stale active
HIDL session before starting, which keeps Bluetooth process restarts from
failing with `EX_ILLEGAL_STATE`.

Software PCM configuration updates are accepted and tracked in-place. The FMQ
remains valid while Android rebuilds its A2DP output for a new sample rate, so
the AIDL and HIDL sides do not temporarily disagree about the active format.

Build inside Linux with NDK r27d and the arm64 AOSP VNDK 32 prebuilts:

```sh
./build.sh \
  /opt/android-ndk-r27d \
  /opt/aosp/vndk32 \
  /opt/aosp/libcxx \
  out/android.hardware.bluetooth.audio-service.marble
```

The binary has device-side diagnostics that do not register the AIDL
service:

```sh
android.hardware.bluetooth.audio-service.marble --check
android.hardware.bluetooth.audio-service.marble --check-software-session
android.hardware.bluetooth.audio-service.marble --check-standard-software-session
android.hardware.bluetooth.audio-service.marble --check-standard-software-session-192
```

The first prints the real QTI offload and PCM capabilities. The other checks
open a stereo, 16-bit software session, verify that the converted AIDL FMQ is
valid, and close the selected QTI or standard session immediately. The final
mode explicitly checks the rebuilt standard session library's 192 kHz
capability and session validation.

Build the LHDC companion from an explicit AOSP core archive rather than an
ignored local cache:

```sh
./lhdcv5_patch/build.sh ANDROID_NDK LHDCV5_CORE_ARCHIVE OUTPUT_DIR
```

For the current profile, the core comes from Android 17's
`system/audio/codecs/lhdcv5` source. A ROM build may pass its own reproducible
static-library output; the companion script does not download source or select
a platform revision implicitly.

## ROM integration boundary

The formal ROM build compiles the AIDL service and the open device
compatibility components during each ROM build:

- The core service is reusable when the target exposes the standard HIDL 2.0
  software provider. Qualcomm HIDL 2.1 is an optional offload adapter.
- The stale-port HAL, PCM 192 kHz session library, audio policy rates, and LHDC
  JNI/encoder patches are the marble VNDK 32 profile.
- The Xiaomi JNI is extracted from the current port ROM's Bluetooth APEX and
  patched during the build. The patch itself rejects a different Bluetooth
  build or instruction site instead of installing an incompatible result.
- Settings Qigsaw assets remain a build-time overlay input and are not a
  runtime dependency of the Bluetooth service.

`make.sh` invokes `rom_integration/build_and_install.sh` after the target images
have been unpacked. The build downloads commit-pinned AOSP source slices,
compiles the bridge, Android 17 LHDCv5 companion, PCM 192 kHz session library,
and stale-output Audio HAL, then installs them directly into the image trees.
There is no checked-in Bluetooth Audio payload ZIP.

The two LHDC v3/v4 encoder libraries are proprietary Xiaomi device inputs;
their source is not part of AOSP. They are kept separately under
`proprietary/marble/` and receive only the reviewed linker edits during the ROM
build. The original Xiaomi-signed Bluetooth APEX stays intact. The bridge
starts with init's HAL class so its provider exists before Bluetooth. Once
APEXes are ready, a system init rule with the narrow `system_lib_file` mount
permission bind-mounts the newly patched JNI over its unversioned library path
before Bluetooth starts; no KernelSU service or polling loop is used. The same
init rule releases that mount on `userspace-reboot-requested`, since a bind
mount pins the Bluetooth APEX loop device and would obstruct an APEX teardown.
This is hardening only — it does not fix the "soft reboot hangs on the second
boot screen" symptom, which was reproduced with the mount already removed and
traced to netd aborting in `libnetd_updatable_init`.

The marble ROM profile also sets `ro.bluetooth.a2dp_offload.supported=false`
at image build time. This selects the standard software path before Bluetooth
starts, without a mutable `persist` property hook or a process restart.

Run the complete profile build and image installation with:

```sh
bash ./rom_integration/build_and_install.sh \
  ANDROID_NDK CARGO SEVEN_ZIP DEVICE_ROOT SYSTEM_ROOT \
  IMAGE_CONFIG_ROOT marble
```

The stable AIDL bridge can survive framework upgrades while the device profile
remains tied to its vendor ABI. A target with a different vendor generation or
Bluetooth JNI must supply a matching device profile rather than silently
applying the marble patch.

## Device profile build notes

Patch the extracted marble `sku_ukee_qssi` audio policy with:

```sh
./patch_audio_policy.py EXTRACTED_AUDIO_POLICY_XML PATCHED_AUDIO_POLICY_XML
```

The patcher converts marble's `bluetooth_qti` A2DP module to the standard
`bluetooth` software HAL, removes the three duplicate primary A2DP output
ports and routes, and drops their hardware-codec restriction. It then adds
88.2 kHz and 192 kHz PCM routing. The known stock and already-converted policy
shapes are validated strictly; A2DP input, SCO, hearing aid, and unrelated
audio routes remain unchanged.

The historical `liblhdc.so` carries a redundant `libstdc++.so` dependency
even though it imports no C++ runtime symbols. Android 17's isolated Bluetooth
APEX namespace does not expose that compatibility library. Prepare and audit
the scoped legacy pair before packaging:

```sh
./lhdcv5_patch/patch_legacy_core.sh ORIGINAL_LIBLHDC PATCHED_LIBLHDC
./lhdcv5_patch/patch_legacy_encoder.sh ORIGINAL_LIBLHDCBT_ENC \
  PATCHED_LIBLHDCBT_ENC
./lhdcv5_patch/audit_legacy_encoder.sh ORIGINAL_LIBLHDC PATCHED_LIBLHDC \
  ORIGINAL_LIBLHDCBT_ENC PATCHED_LIBLHDCBT_ENC
```

The core patch removes only that unused `DT_NEEDED` entry. The audit requires
the original and patched undefined-symbol sets, `.text`, Build ID, SONAME, and
all other dependencies to remain identical.

The platform `A2DP_GetPacketTimestamp` switch in the audited marble Bluetooth
library recognizes LHDCv5 but omits the historical LHDC v2/v3 IDs. LHDCv4
shares the v3 codec ID. `lhdcv5_patch/patch_bluetooth_jni.sh` assembles a
28-byte, address-gated compatibility check that masks only the codec-ID byte's
v2/v3 bit (bit 24 of the reconstructed key) and sends those two IDs to the
stock generic timestamp path. Other unsupported codecs still return
false. `lhdcv5_patch/audit.sh` verifies the exact original bytes, instruction
targets, and that all other `.text` bytes remain identical.

The stock `android.hardware.bluetooth.audio@2.0-impl.so` provider is not
modified. `session_192_patch/build.sh` rebuilds the Android 12L
`libbluetooth_audio_session.so` ABI with CFI and Android packed relocations;
the `pcm192` mode changes only the V2.0/V2.1 software PCM capability and
validation tables. `session_192_patch/audit.sh` checks the stock provider's
imports, the original public session ABI, dependency order, relocation tags,
CFI, and the exact source delta before packaging.

`audio_hal_stale_port_patch/build.sh` rebuilds Android 12L's standard
`audio.bluetooth.default.so` against the marble VNDK 32 ABI. On an A2DP codec
or sample-rate change, an existing output now stays `DISABLED` across the
rapid session-end/session-start callbacks. Stock `out_write()` consumes and
drops that output's residual PCM, so it cannot enter the replacement FMQ; the
fresh AudioPolicy output still starts normally. Other Bluetooth session types
retain the stock state transition. The paired audit compares the original,
zero-change baseline, and patch for dependency order, dynamic ABI, CFI,
relocations, BTI/PAC, source scope, and the final AArch64 state instructions.

The reviewed JNI uses the full
`/system/lib64/liblhdcv5_native_patch.so` companion path permitted by the
Bluetooth APEX linker namespace. The ROM integration verifies the audited
Bluetooth Build ID and patch site before installing it, then bind-mounts only
the unversioned APEX JNI path before Bluetooth starts.

The ROM build keeps the complete device Settings overlay and lets mi_ext add
only missing APKs. This preserves unrelated Settings resources while retaining
the Qigsaw data used by the Redmi Buds page.

The checked-in generated bindings are frozen Android Bluetooth Audio AIDL V3
(hash `fead4df60244a5440283617064f184690414a685`) and Qualcomm Bluetooth Audio
HIDL 2.0/2.1 definitions matching the marble device interface hashes. Generated
code is linked into the executable, so the bridge does not depend on a matching
interface shared library being present at runtime.

Do not deploy the service without its VINTF declaration, init service, SELinux
labels, and a rollback path. The VINTF fragment must have the
`vendor_configs_file` label before `hwservicemanager` parses it.
The marble vendor policy also needs the narrowly scoped
`hal_audio_default` self-Binder `call` and `transfer` permissions installed by
`rom_integration`;
without it, the bridge cannot obtain the same-domain HIDL provider proxy and
the lazy AIDL service exits on every lookup.
Registering the AIDL factory changes the transport selected by the Android
Bluetooth stack for the whole boot.
