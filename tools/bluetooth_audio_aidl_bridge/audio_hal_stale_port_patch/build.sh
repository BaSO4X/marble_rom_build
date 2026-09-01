#!/bin/bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
repo_root=$(cd -- "$script_dir/../../.." && pwd)
usage='Usage: build.sh baseline|stale-port ANDROID_NDK VNDK32_ROOT PLATFORM_LIBCXX_ROOT AOSP_SYSTEM_BT_ROOT AOSP_HARDWARE_INTERFACES_ROOT GENERATED_2_1_HEADERS LIBHARDWARE_ROOT SYSTEM_MEDIA_ROOT DEVICE_LIBRARY_DIR OUTPUT_LIBRARY'
mode=${1:?$usage}
android_ndk=${2:?$usage}
vndk_root=${3:?$usage}
platform_libcxx_root=${4:?$usage}
aosp_system_bt_root=${5:?$usage}
aosp_interfaces_root=${6:?$usage}
generated_2_1_headers=${7:?$usage}
libhardware_root=${8:?$usage}
system_media_root=${9:?$usage}
device_library_dir=${10:?$usage}
output_library=${11:?$usage}

if [[ "$mode" != baseline && "$mode" != stale-port ]]; then
  printf 'Unknown build mode: %s\n' "$mode" >&2
  exit 1
fi

toolchain="$android_ndk/toolchains/llvm/prebuilt/linux-x86_64/bin"
compiler="$toolchain/aarch64-linux-android32-clang++"
readelf="$toolchain/llvm-readelf"
strip="$toolchain/llvm-strip"
vndk_arm64="$vndk_root/arm64"
vndk_sp="$vndk_arm64/arch-arm64-armv8-a/shared/vndk-sp"
vndk_core="$vndk_arm64/arch-arm64-armv8-a/shared/vndk-core"
llndk_stub="$vndk_arm64/arch-arm64-armv8-a/shared/llndk-stub"
aosp_audio_hal="$aosp_system_bt_root/audio_bluetooth_hw"
aosp_session="$aosp_interfaces_root/bluetooth/audio/utils/session"
generated_2_0_headers="$repo_root/tools/bluetooth_audio_aidl_bridge/generated/aosp"
work_dir="$output_library.work"
source_dir="$work_dir/audio_bluetooth_hw"
object_dir="$work_dir/objects"

for tool in "$compiler" "$readelf" "$strip"; do
  if [[ ! -x "$tool" ]]; then
    printf 'Required Android tool is missing: %s\n' "$tool" >&2
    exit 1
  fi
done
for path in \
  "$platform_libcxx_root/include/__config" \
  "$aosp_audio_hal/audio_bluetooth_hw.cc" \
  "$aosp_audio_hal/device_port_proxy.cc" \
  "$aosp_audio_hal/stream_apis.cc" \
  "$aosp_audio_hal/utils.cc" \
  "$aosp_session/BluetoothAudioSessionControl_2_1.h" \
  "$generated_2_0_headers/android/hardware/bluetooth/audio/2.0/types.h" \
  "$generated_2_1_headers/android/hardware/bluetooth/audio/2.1/types.h" \
  "$libhardware_root/include/hardware/audio.h" \
  "$system_media_root/audio/include/system/audio.h" \
  "$device_library_dir/android.hardware.bluetooth.audio@2.0.so" \
  "$device_library_dir/android.hardware.bluetooth.audio@2.1.so" \
  "$device_library_dir/libbluetooth_audio_session.so"; do
  if [[ ! -f "$path" ]]; then
    printf 'Required input is missing: %s\n' "$path" >&2
    exit 1
  fi
done
for library in libbase.so libcutils.so libhidlbase.so libutils.so libc++.so; do
  if [[ ! -f "$vndk_sp/$library" ]]; then
    printf 'Required VNDK 32 library is missing: %s\n' "$vndk_sp/$library" >&2
    exit 1
  fi
done
for library in libaudioutils.so libfmq.so; do
  if [[ ! -f "$vndk_core/$library" ]]; then
    printf 'Required VNDK 32 library is missing: %s\n' "$vndk_core/$library" >&2
    exit 1
  fi
done
if [[ ! -f "$llndk_stub/liblog.so" ]]; then
  printf '%s\n' 'Required VNDK 32 LL-NDK log library is missing' >&2
  exit 1
fi
if [[ -e "$output_library" || -e "$work_dir" ]]; then
  printf 'Refusing to overwrite existing output or work directory: %s\n' \
    "$output_library" >&2
  exit 1
fi

mkdir -p -- "$(dirname -- "$output_library")" "$source_dir" "$object_dir"
cp -a "$aosp_audio_hal/." "$source_dir/"
if [[ "$mode" == stale-port ]]; then
  python3 - "$source_dir/device_port_proxy.cc" <<'PY'
from pathlib import Path
import sys

path = Path(sys.argv[1])
source = path.read_text()
old = """  if (previous_state != BluetoothStreamState::DISABLED) {
    state_ = BluetoothStreamState::DISABLED;
  } else {
    state_ = BluetoothStreamState::STANDBY;
  }
"""
new = """  if (session_type_ ==
      SessionType_2_1::A2DP_SOFTWARE_ENCODING_DATAPATH) {
    // A codec reconfiguration ends and immediately replaces the session.
    // Keep this existing output disabled so stale PCM cannot enter the new
    // session's FMQ. AudioPolicy opens a fresh output for the new format.
    state_ = BluetoothStreamState::DISABLED;
  } else if (previous_state != BluetoothStreamState::DISABLED) {
    state_ = BluetoothStreamState::DISABLED;
  } else {
    state_ = BluetoothStreamState::STANDBY;
  }
"""
if source.count(old) != 1:
    raise SystemExit("stock SessionChangedHandler block was not found exactly once")
path.write_text(source.replace(old, new))
PY
fi

sources=(
  "$source_dir/audio_bluetooth_hw.cc"
  "$source_dir/stream_apis.cc"
  "$source_dir/device_port_proxy.cc"
  "$source_dir/utils.cc"
)
includes=(
  -I"$source_dir"
  -I"$aosp_session"
  -I"$generated_2_1_headers"
  -I"$generated_2_0_headers"
  -I"$libhardware_root/include"
  -I"$system_media_root/audio/include"
  -I"$system_media_root/audio_utils/include"
  -I"$vndk_arm64/include/system/libhidl/base/include"
  -I"$vndk_arm64/include/system/libhidl/transport/include"
  -I"$vndk_arm64/include/system/libhwbinder/include"
  -I"$vndk_arm64/include/system/libfmq/base"
  -I"$vndk_arm64/include/system/libfmq/include"
  -I"$vndk_arm64/include/system/core/libutils/include"
  -I"$vndk_arm64/include/system/core/libcutils/include"
  -I"$vndk_arm64/include/system/libbase/include"
  -I"$vndk_arm64/include/system/logging/liblog/include_vndk"
  -I"$vndk_arm64/include/system/media/audio_utils/include"
  -I"$vndk_arm64/include/frameworks/native/libs/binder/include"
  -I"$vndk_arm64/include/system/core/libsystem/include"
  -I"$vndk_arm64/include/frameworks/native/libs/nativebase/include"
  -I"$vndk_arm64/include/generated-headers/system/libhidl/transport/base/1.0/android.hidl.base@1.0_genc++_headers/gen"
  -I"$vndk_arm64/include/generated-headers/system/libhidl/transport/manager/1.0/android.hidl.manager@1.0_genc++_headers/gen"
)
compile_flags=(
  -std=c++17 -O2 -Wall -Wextra -Werror -Wno-unused-parameter -pthread
  -Wno-error=inconsistent-missing-override
  -Wno-error=deprecated-declarations
  -Wno-error=deprecated-copy-with-user-provided-copy
  -Wno-error=missing-field-initializers
  -Wno-error=sign-compare
  -D_FORTIFY_SOURCE=2
  -D_LIBCPP_SUPPORT_XLOCALE_POSIX_L_FALLBACK_H
  -fno-exceptions -fno-rtti -fstack-protector-strong
  -ffunction-sections -fdata-sections -fPIC
  -ffile-prefix-map="$source_dir"=system/bt/audio_bluetooth_hw
  -fmacro-prefix-map="$source_dir"=system/bt/audio_bluetooth_hw
  -fdebug-prefix-map="$source_dir"=system/bt/audio_bluetooth_hw
  -flto=thin -fsanitize=cfi -fsanitize-cfi-cross-dso -fvisibility=default
  -mbranch-protection=standard
  -nostdinc++ -isystem "$platform_libcxx_root/include"
  -nostdlib++
)

objects=()
for index in "${!sources[@]}"; do
  object="$object_dir/$index.o"
  objects+=("$object")
  "$compiler" "${compile_flags[@]}" "${includes[@]}" \
    -c "${sources[$index]}" -o "$object"
done

"$compiler" -shared -nostdlib++ -flto=thin -fsanitize=cfi \
  -fsanitize-cfi-cross-dso -fvisibility=default -mbranch-protection=standard \
  -Wl,-soname,audio.bluetooth.default.so \
  -Wl,-z,relro,-z,now,--gc-sections \
  -Wl,--pack-dyn-relocs=android+relr \
  -Wl,-z,max-page-size=4096 \
  "${objects[@]}" \
  -L"$device_library_dir" -L"$llndk_stub" -L"$vndk_sp" -L"$vndk_core" \
  -Wl,-rpath-link,"$device_library_dir" \
  -Wl,-rpath-link,"$vndk_sp" -Wl,-rpath-link,"$vndk_core" \
  -o "$output_library" \
  -Wl,--no-as-needed \
  -l:android.hardware.bluetooth.audio@2.0.so \
  -l:android.hardware.bluetooth.audio@2.1.so \
  -laudioutils -lbase -l:libbluetooth_audio_session.so -lcutils -lfmq \
  -lhidlbase -llog -lutils -lc++ -lc -lm -ldl
"$strip" --strip-unneeded "$output_library"

elf_info=$("$readelf" -h -d --notes --wide "$output_library")
check_elf() {
  local pattern=$1
  local description=$2
  if ! grep -Eq "$pattern" <<<"$elf_info"; then
    printf 'Bluetooth Audio HAL validation failed: %s\n' "$description" >&2
    exit 1
  fi
}

check_elf 'Class:[[:space:]]+ELF64' 'not ELF64'
check_elf 'Machine:[[:space:]]+AArch64' 'not AArch64'
check_elf 'Type:[[:space:]]+DYN([[:space:]]|$)' 'not a shared object'
check_elf 'SONAME.*\[audio\.bluetooth\.default\.so\]' 'incorrect SONAME'
check_elf 'FLAGS.*BIND_NOW' 'missing BIND_NOW'
check_elf 'ANDROID_RELA' 'missing Android packed RELA'
check_elf '\(RELR\)' 'missing standard RELR'
check_elf 'aarch64 feature: BTI, PAC' 'missing BTI/PAC property'
for library in \
  android.hardware.bluetooth.audio@2.0.so \
  android.hardware.bluetooth.audio@2.1.so \
  libaudioutils.so libbase.so libbluetooth_audio_session.so libcutils.so \
  libfmq.so libhidlbase.so liblog.so libutils.so libc++.so libc.so libm.so \
  libdl.so; do
  check_elf "NEEDED.*\\[$library\\]" "missing $library dependency"
done
if grep -Eq 'RPATH|RUNPATH|ANDROID_RELR' <<<"$elf_info"; then
  printf '%s\n' 'Bluetooth Audio HAL has an unexpected relocation tag or search path' >&2
  exit 1
fi

printf 'Built %s Bluetooth Audio HAL: %s\n' "$mode" "$output_library"
