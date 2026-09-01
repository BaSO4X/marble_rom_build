#!/bin/bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
repo_root=$(cd -- "$script_dir/../../.." && pwd)
usage='Usage: build.sh baseline|pcm192 ANDROID_NDK VNDK32_ROOT PLATFORM_LIBCXX_ROOT AOSP_HARDWARE_INTERFACES_ROOT GENERATED_2_1_HEADERS LIBHARDWARE_ROOT SYSTEM_MEDIA_ROOT DEVICE_LIBRARY_DIR OUTPUT_LIBRARY'
mode=${1:?$usage}
android_ndk=${2:?$usage}
vndk_root=${3:?$usage}
platform_libcxx_root=${4:?$usage}
aosp_interfaces_root=${5:?$usage}
generated_2_1_headers=${6:?$usage}
libhardware_root=${7:?$usage}
system_media_root=${8:?$usage}
device_library_dir=${9:?$usage}
output_library=${10:?$usage}

if [[ "$mode" != baseline && "$mode" != pcm192 ]]; then
  printf 'Unknown build mode: %s\n' "$mode" >&2
  exit 1
fi

toolchain="$android_ndk/toolchains/llvm/prebuilt/linux-x86_64/bin"
compiler="$toolchain/aarch64-linux-android31-clang++"
readelf="$toolchain/llvm-readelf"
strip="$toolchain/llvm-strip"
vndk_arm64="$vndk_root/arm64"
vndk_sp="$vndk_arm64/arch-arm64-armv8-a/shared/vndk-sp"
vndk_core="$vndk_arm64/arch-arm64-armv8-a/shared/vndk-core"
llndk_stub="$vndk_arm64/arch-arm64-armv8-a/shared/llndk-stub"
aosp_session="$aosp_interfaces_root/bluetooth/audio/utils/session"
generated_2_0_headers="$repo_root/tools/bluetooth_audio_aidl_bridge/generated/aosp"
work_dir="$output_library.work"
source_dir="$work_dir/session"
object_dir="$work_dir/objects"

for tool in "$compiler" "$readelf" "$strip"; do
  if [[ ! -x "$tool" ]]; then
    printf 'Required Android tool is missing: %s\n' "$tool" >&2
    exit 1
  fi
done
for path in \
  "$platform_libcxx_root/include/__config" \
  "$aosp_session/BluetoothAudioSession.cpp" \
  "$generated_2_0_headers/android/hardware/bluetooth/audio/2.0/types.h" \
  "$generated_2_1_headers/android/hardware/bluetooth/audio/2.1/types.h" \
  "$libhardware_root/include/hardware/audio.h" \
  "$system_media_root/audio/include/system/audio.h" \
  "$device_library_dir/android.hardware.bluetooth.audio@2.0.so" \
  "$device_library_dir/android.hardware.bluetooth.audio@2.1.so"; do
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
if [[ ! -f "$vndk_core/libfmq.so" || ! -f "$llndk_stub/liblog.so" ]]; then
  printf '%s\n' 'Required VNDK 32 FMQ or LL-NDK log library is missing' >&2
  exit 1
fi
if [[ -e "$output_library" || -e "$work_dir" ]]; then
  printf 'Refusing to overwrite existing output or work directory: %s\n' \
    "$output_library" >&2
  exit 1
fi

mkdir -p -- "$(dirname -- "$output_library")" "$source_dir" "$object_dir"
cp -a "$aosp_session/." "$source_dir/"
if [[ "$mode" == pcm192 ]]; then
  sed -i \
    's/SampleRate::RATE_16000 | SampleRate::RATE_24000),/SampleRate::RATE_16000 | SampleRate::RATE_24000 |\n        SampleRate::RATE_192000),/' \
    "$source_dir/BluetoothAudioSupportedCodecsDB.cpp"
  sed -i \
    '/pcm_config.sampleRate != SampleRate::RATE_16000 &&/a\       pcm_config.sampleRate != SampleRate::RATE_192000 &&' \
    "$source_dir/BluetoothAudioSupportedCodecsDB.cpp"
  sed -i \
    's/SampleRate_2_1::RATE_16000 | SampleRate_2_1::RATE_24000),/SampleRate_2_1::RATE_16000 | SampleRate_2_1::RATE_24000 |\n            SampleRate_2_1::RATE_192000),/' \
    "$source_dir/BluetoothAudioSupportedCodecsDB_2_1.cpp"
  sed -i \
    '/pcm_config.sampleRate != SampleRate_2_1::RATE_16000 &&/a\       pcm_config.sampleRate != SampleRate_2_1::RATE_192000 &&' \
    "$source_dir/BluetoothAudioSupportedCodecsDB_2_1.cpp"
  for source in \
    "$source_dir/BluetoothAudioSupportedCodecsDB.cpp" \
    "$source_dir/BluetoothAudioSupportedCodecsDB_2_1.cpp"; do
    if [[ $(grep -c 'RATE_192000' "$source") -ne 2 ]]; then
      printf 'PCM 192 source edit did not apply exactly once per site: %s\n' \
        "$source" >&2
      exit 1
    fi
  done
fi

sources=(
  "$source_dir/BluetoothAudioSession.cpp"
  "$source_dir/BluetoothAudioSession_2_1.cpp"
  "$source_dir/BluetoothAudioSupportedCodecsDB.cpp"
  "$source_dir/BluetoothAudioSupportedCodecsDB_2_1.cpp"
)
includes=(
  -I"$source_dir"
  -I"$generated_2_1_headers"
  -I"$generated_2_0_headers"
  -I"$libhardware_root/include"
  -I"$system_media_root/audio/include"
  -I"$vndk_arm64/include/system/libhidl/base/include"
  -I"$vndk_arm64/include/system/libhidl/transport/include"
  -I"$vndk_arm64/include/system/libhwbinder/include"
  -I"$vndk_arm64/include/system/libfmq/base"
  -I"$vndk_arm64/include/system/libfmq/include"
  -I"$vndk_arm64/include/system/core/libutils/include"
  -I"$vndk_arm64/include/system/core/libcutils/include"
  -I"$vndk_arm64/include/system/libbase/include"
  -I"$vndk_arm64/include/system/logging/liblog/include_vndk"
  -I"$vndk_arm64/include/frameworks/native/libs/binder/include"
  -I"$vndk_arm64/include/system/core/libsystem/include"
  -I"$vndk_arm64/include/frameworks/native/libs/nativebase/include"
  -I"$vndk_arm64/include/generated-headers/system/libhidl/transport/base/1.0/android.hidl.base@1.0_genc++_headers/gen"
  -I"$vndk_arm64/include/generated-headers/system/libhidl/transport/manager/1.0/android.hidl.manager@1.0_genc++_headers/gen"
)
compile_flags=(
  -std=c++17 -O2 -Wall -Wextra -Werror -pthread
  -Wno-error=inconsistent-missing-override
  -Wno-error=deprecated-declarations
  -Wno-error=deprecated-copy-with-user-provided-copy
  -Wno-error=missing-field-initializers
  -D_FORTIFY_SOURCE=2
  -D_LIBCPP_SUPPORT_XLOCALE_POSIX_L_FALLBACK_H
  -fno-exceptions -fno-rtti -fstack-protector-strong
  -ffunction-sections -fdata-sections -fPIC
  -flto=thin -fsanitize=cfi -fsanitize-cfi-cross-dso -fvisibility=default
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
  -fsanitize-cfi-cross-dso -fvisibility=default \
  -Wl,-soname,libbluetooth_audio_session.so \
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
  -lbase -lcutils -lfmq -lhidlbase -llog -lutils -lc++ -lc -lm -ldl
"$strip" --strip-unneeded "$output_library"

elf_info=$("$readelf" -h -d "$output_library")
check_elf() {
  local pattern=$1
  local description=$2
  if ! grep -Eq "$pattern" <<<"$elf_info"; then
    printf 'Session library validation failed: %s\n' "$description" >&2
    exit 1
  fi
}

check_elf 'Class:[[:space:]]+ELF64' 'not ELF64'
check_elf 'Machine:[[:space:]]+AArch64' 'not AArch64'
check_elf 'Type:[[:space:]]+DYN([[:space:]]|$)' 'not a shared object'
check_elf 'SONAME.*\[libbluetooth_audio_session\.so\]' 'incorrect SONAME'
check_elf 'FLAGS.*BIND_NOW' 'missing BIND_NOW'
for library in \
  android.hardware.bluetooth.audio@2.0.so \
  android.hardware.bluetooth.audio@2.1.so \
  libbase.so libcutils.so libfmq.so libhidlbase.so liblog.so libutils.so \
  libc++.so libc.so libm.so libdl.so; do
  check_elf "NEEDED.*\\[$library\\]" "missing $library dependency"
done
if grep -Eq 'RPATH|RUNPATH' <<<"$elf_info"; then
  printf '%s\n' 'Session library unexpectedly contains RPATH/RUNPATH' >&2
  exit 1
fi

printf 'Built %s session library: %s\n' "$mode" "$output_library"
