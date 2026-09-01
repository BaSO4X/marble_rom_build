#!/bin/bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
bridge_dir=$(cd -- "$script_dir/.." && pwd)
usage='Usage: build_profile.sh ANDROID_NDK CARGO SEVEN_ZIP DEVICE_ROOT SYSTEM_ROOT OUTPUT_ROOT'
android_ndk=${1:?$usage}
cargo=${2:?$usage}
seven_zip=${3:?$usage}
device_root=${4:?$usage}
system_root=${5:?$usage}
output_root=${6:?$usage}
vendor_root="$device_root/vendor"
profile_root="$output_root/profile"
source_root="$output_root/sources"
work_root="$output_root/work"
binary_root="$work_root/output"
toolchain="$android_ndk/toolchains/llvm/prebuilt/linux-x86_64/bin"

fetch_archive() {
  local url=$1
  local destination=$2
  mkdir -p -- "$destination"
  curl --fail --location --silent --show-error --retry 3 "$url" | \
    tar -xz -C "$destination"
}

mkdir -p -- "$profile_root" "$source_root" "$binary_root"

# Pin the small Android 12L source slices used by the VNDK 32 vendor profile.
# Gitiles archives contain only the requested directory, without Git history.
fetch_archive \
  'https://android.googlesource.com/platform/prebuilts/vndk/v32/+archive/e1ba8043f8368220b412c18ee8392850ab009aae.tar.gz' \
  "$source_root/vndk32"
fetch_archive \
  'https://android.googlesource.com/platform/external/libcxx/+archive/55cdd8a9892c9faec91319944984daeb1579e84a/include.tar.gz' \
  "$source_root/libcxx/include"
fetch_archive \
  'https://android.googlesource.com/platform/hardware/interfaces/+archive/7fea2cf0a29212202cc31d9d8ec4c892f09d3c2d/bluetooth/audio/utils/session.tar.gz' \
  "$source_root/hardware-interfaces/bluetooth/audio/utils/session"
fetch_archive \
  'https://android.googlesource.com/platform/hardware/libhardware/+archive/c142f2613ef9982d59dca254d7a625bdf595f70c/include.tar.gz' \
  "$source_root/libhardware/include"
fetch_archive \
  'https://android.googlesource.com/platform/system/media/+archive/612d1707ddd53b341512a8de9b3255c314e7c0b2/audio/include.tar.gz' \
  "$source_root/system-media/audio/include"
fetch_archive \
  'https://android.googlesource.com/platform/system/media/+archive/612d1707ddd53b341512a8de9b3255c314e7c0b2/audio_utils/include.tar.gz' \
  "$source_root/system-media/audio_utils/include"
fetch_archive \
  'https://android.googlesource.com/platform/system/bt/+archive/f8f00629d484c70309df83210afeaee321654587/audio_bluetooth_hw.tar.gz' \
  "$source_root/system-bt/audio_bluetooth_hw"
fetch_archive \
  'https://android.googlesource.com/platform/packages/modules/Bluetooth/+archive/c77db469de80f86660bc053bec4dee0c5d4b947c/system/audio/codecs/lhdcv5.tar.gz' \
  "$source_root/lhdcv5"

# Android's Cargo manifest omits the android_logger dependency supplied by
# Soong. Add the equivalent dependency and use the reviewed lockfile.
sed -i '/^\[dependencies\]$/a android_logger = "0.13.3"' \
  "$source_root/lhdcv5/Cargo.toml"
cp -f "$bridge_dir/lhdcv5_patch/Cargo.lock" \
  "$source_root/lhdcv5/Cargo.lock"
cargo_dir=$(dirname -- "$cargo")
export PATH="$cargo_dir:$PATH"
CARGO_TARGET_AARCH64_LINUX_ANDROID_LINKER="$toolchain/aarch64-linux-android31-clang" \
CARGO_TARGET_AARCH64_LINUX_ANDROID_AR="$toolchain/llvm-ar" \
RUSTFLAGS='-C panic=abort' \
  "$cargo" rustc \
    --manifest-path "$source_root/lhdcv5/Cargo.toml" \
    --locked --target aarch64-linux-android --release --lib \
    -- --crate-type staticlib
lhdcv5_archive="$source_root/lhdcv5/target/aarch64-linux-android/release/deps/liblhdcv5.a"

bridge_output="$binary_root/android.hardware.bluetooth.audio-service.marble"
bash "$bridge_dir/build.sh" \
  "$android_ndk" "$source_root/vndk32" "$source_root/libcxx" \
  "$bridge_output"
install -D -m 0755 "$bridge_output" \
  "$profile_root/vendor/bin/hw/android.hardware.bluetooth.audio-service.marble"
bash "$bridge_dir/lhdcv5_patch/build.sh" \
  "$android_ndk" "$lhdcv5_archive" "$binary_root"
install -D -m 0644 "$binary_root/liblhdcv5_native_patch.so" \
  "$profile_root/system/lib64/liblhdcv5_native_patch.so"

session_output="$binary_root/libbluetooth_audio_session.so"
bash "$bridge_dir/session_192_patch/build.sh" pcm192 \
  "$android_ndk" \
  "$source_root/vndk32" \
  "$source_root/libcxx" \
  "$source_root/hardware-interfaces" \
  "$bridge_dir/generated/aosp" \
  "$source_root/libhardware" \
  "$source_root/system-media" \
  "$vendor_root/lib64" \
  "$session_output"
install -D -m 0644 "$session_output" \
  "$profile_root/vendor/lib64/libbluetooth_audio_session.so"

hal_link_root="$work_root/vendor-lib64"
mkdir -p -- "$hal_link_root"
for library in \
  android.hardware.audio.common@5.0.so \
  android.hardware.bluetooth.audio@2.0.so \
  android.hardware.bluetooth.audio@2.1.so \
  android.hidl.safe_union@1.0.so; do
  ln -s "$vendor_root/lib64/$library" "$hal_link_root/$library"
done
ln -s "$session_output" "$hal_link_root/libbluetooth_audio_session.so"
hal_output="$binary_root/audio.bluetooth.default.so"
bash "$bridge_dir/audio_hal_stale_port_patch/build.sh" stale-port \
  "$android_ndk" \
  "$source_root/vndk32" \
  "$source_root/libcxx" \
  "$source_root/system-bt" \
  "$source_root/hardware-interfaces" \
  "$bridge_dir/generated/aosp" \
  "$source_root/libhardware" \
  "$source_root/system-media" \
  "$hal_link_root" \
  "$hal_output"
install -D -m 0644 "$hal_output" \
  "$profile_root/vendor/lib64/hw/audio.bluetooth.default.so"

apex_root="$work_root/bluetooth-apex"
mkdir -p -- "$apex_root"
"$seven_zip" x -y -bd -o"$apex_root" \
  "$system_root/apex/com.android.bt.apex" apex_payload.img >/dev/null
"$seven_zip" x -y -bd -o"$apex_root/root" \
  "$apex_root/apex_payload.img" lib64/libbluetooth_jni.so >/dev/null

export AARCH64_CLANG="$toolchain/aarch64-linux-android31-clang"
export LD_LLD="$toolchain/ld.lld"
export LLVM_OBJCOPY="$toolchain/llvm-objcopy"
export LLVM_READELF="$toolchain/llvm-readelf"
bash "$bridge_dir/lhdcv5_patch/patch_bluetooth_jni.sh" \
  "$apex_root/root/lib64/libbluetooth_jni.so" \
  "$profile_root/system/lib64/libbluetooth_jni.marble.so"
bash "$bridge_dir/lhdcv5_patch/patch_legacy_core.sh" \
  "$bridge_dir/proprietary/marble/liblhdc.so" \
  "$profile_root/system/lib64/liblhdc.so"
bash "$bridge_dir/lhdcv5_patch/patch_legacy_encoder.sh" \
  "$bridge_dir/proprietary/marble/liblhdcBT_enc.so" \
  "$profile_root/system/lib64/liblhdcBT_enc.so"

printf 'Built Bluetooth Audio ROM profile from source: %s\n' "$profile_root"
