# Marble vibrator compatibility proxy

This directory provides the Android 17 vibrator compatibility layer for the
marble Android 13 vendor (`ro.vendor.build.version.sdk=32`).

## Scope

The marble vendor already exposes an AIDL service at
`android.hardware.vibrator.IVibrator/vibratorfeature`. Android framework expects
`android.hardware.vibrator.IVibrator/default`.

This helper is an AIDL-to-AIDL Binder proxy. It is not the Rust AIDL-to-HIDL
bridge used by devices whose vibrator backend is still HIDL.

The proxy:

- registers `/default` while retaining `/vibratorfeature` as the source;
- forwards source-supported effects and all non-intercepted IVibrator calls;
- copies the Xiaomi Binder extension;
- maps the verified HyperOS task-clean effect `213` to marble vendor effect
  `90` (`90_taskCleanAll_P_RTP.bin`);
- falls back to `TICK`, `CLICK`, or `HEAVY_CLICK` by requested strength for
  otherwise unsupported effects;
- advertises effect IDs `0..1023`, plus larger source-supported IDs, so the
  framework does not discard current HyperOS private effects before the proxy.

Unknown private effects receive a safe generic vibration. Their original RTP
semantics are not guaranteed until an explicit old/new mapping is verified.

## SELinux

The proxy and source service run in `hal_vibrator_default`. Proxy transactions
therefore require the minimal same-domain Binder permissions:

```text
allow hal_vibrator_default hal_vibrator_default binder { call transfer }
```

For a ROM build, `make.sh` appends the equivalent CIL rule from
`vendor.vibrator-default-alias.cil` to `vendor_sepolicy.cil`. It then removes
only `precompiled_sepolicy` and its `.sha256` companions from vendor/odm, so
Android init recompiles the split CIL policy instead of loading a stale policy.

## Build

```bash
./tools/vibrator_alias/build.sh ANDROID_NDK OUTPUT_BINARY
```

The helper targets arm64 Android API 31 and is built as a PIE with RELRO and
immediate binding. The temporary KernelSU validation module and its packaging
files are intentionally not retained in the repository.

## Verified behavior

Verified on marble with vendor SDK 32 and Android 17:

- clean boot with both vibrator AIDL instances and `vibrator_manager` present;
- standard `CLICK` forwarded unchanged;
- Launcher recent-tasks clean button submitted effect `213`, completed through
  effect `90`, and opened `90_taskCleanAll_P_RTP.bin`;
- no remaining `ignored_unsupported` entries in the tested boot.
