# marble Consumer IR AIDL adapter

This service exposes `android.hardware.ir.IConsumerIr/default` while reusing the
working legacy `/vendor/lib64/hw/consumerir.qcom.so` module. It replaces the old
HIDL service in place, so the existing `hal_ir_default` SELinux domain and file
metadata continue to apply.

`build.sh` builds for vendor API 31 with NDK r27d. The checked-in NDK Binder glue
was generated from the frozen AOSP V1 AIDL definitions with interface hash
`3e04aed366e96850c6164287eaf78a8e4ab071b0`. C++ runtime code is linked statically;
the result does not depend on `android.hardware.ir-V1-ndk.so` or
`libc++_shared.so` from a newer VNDK.

The public API 31 NDK headers do not expose service registration, thread-pool,
or VINTF-stability helpers. The adapter resolves those platform symbols from
the device's `libbinder_ndk.so` at runtime and fails before registration when a
required symbol is unavailable.
