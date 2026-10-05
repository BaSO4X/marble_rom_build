#!/bin/bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
repo_root=$(cd -- "$script_dir/../../.." && pwd)
usage='Usage: build_provider_probe.sh ANDROID_NDK VNDK32_ROOT PLATFORM_LIBCXX_ROOT DEVICE_LIBRARY_DIR OUTPUT_BINARY'
android_ndk=${1:?$usage}
vndk_root=${2:?$usage}
platform_libcxx_root=${3:?$usage}
device_library_dir=${4:?$usage}
output_binary=${5:?$usage}
toolchain="$android_ndk/toolchains/llvm/prebuilt/linux-x86_64/bin"
compiler="$toolchain/aarch64-linux-android31-clang++"
strip="$toolchain/llvm-strip"
readelf="$toolchain/llvm-readelf"
vndk_arm64="$vndk_root/arm64"
vndk_sp="$vndk_arm64/arch-arm64-armv8-a/shared/vndk-sp"
vndk_core="$vndk_arm64/arch-arm64-armv8-a/shared/vndk-core"
llndk_stub="$vndk_arm64/arch-arm64-armv8-a/shared/llndk-stub"

for tool in "$compiler" "$strip" "$readelf"; do
  if [[ ! -x "$tool" ]]; then
    printf 'Required Android tool is missing: %s\n' "$tool" >&2
    exit 1
  fi
done
if [[ -e "$output_binary" ]]; then
  printf 'Refusing to overwrite existing output: %s\n' "$output_binary" >&2
  exit 1
fi
mkdir -p -- "$(dirname -- "$output_binary")"

includes=(
  -I"$repo_root/tools/bluetooth_audio_aidl_bridge/generated/aosp"
  -I"$vndk_arm64/include/system/libhidl/base/include"
  -I"$vndk_arm64/include/system/libhidl/transport/include"
  -I"$vndk_arm64/include/system/libhwbinder/include"
  -I"$vndk_arm64/include/system/libfmq/base"
  -I"$vndk_arm64/include/system/libfmq/include"
  -I"$vndk_arm64/include/system/core/libutils/include"
  -I"$vndk_arm64/include/system/core/libcutils/include"
  -I"$vndk_arm64/include/system/libbase/include"
  -I"$vndk_arm64/include/system/logging/liblog/include_vndk"
  -I"$vndk_arm64/include/system/core/libsystem/include"
  -I"$vndk_arm64/include/frameworks/native/libs/nativebase/include"
  -I"$vndk_arm64/include/generated-headers/system/libhidl/transport/base/1.0/android.hidl.base@1.0_genc++_headers/gen"
  -I"$vndk_arm64/include/generated-headers/system/libhidl/transport/manager/1.0/android.hidl.manager@1.0_genc++_headers/gen"
)
compile_flags=(
  -std=c++17 -O2 -Wall -Wextra -Werror
  -Wno-error=deprecated-copy-with-user-provided-copy
  -D_FORTIFY_SOURCE=2
  -D_LIBCPP_SUPPORT_XLOCALE_POSIX_L_FALLBACK_H
  -fno-exceptions -fno-rtti -fstack-protector-strong
  -ffunction-sections -fdata-sections -fPIE
  -flto=thin -fsanitize=cfi -fsanitize-cfi-cross-dso -fvisibility=default
  -nostdinc++ -isystem "$platform_libcxx_root/include"
  -nostdlib++
)

"$compiler" "${compile_flags[@]}" "${includes[@]}" \
  -pie -flto=thin -fsanitize=cfi -fsanitize-cfi-cross-dso \
  -fvisibility=default \
  -Wl,-z,relro,-z,now,--gc-sections \
  -Wl,-z,max-page-size=4096 \
  "$script_dir/provider_probe.cpp" \
  -L"$device_library_dir" -L"$llndk_stub" -L"$vndk_sp" -L"$vndk_core" \
  -Wl,-rpath-link,"$device_library_dir" \
  -Wl,-rpath-link,"$vndk_sp" -Wl,-rpath-link,"$vndk_core" \
  -Wl,--no-as-needed -l:android.hardware.bluetooth.audio@2.0.so \
  -lhidlbase -lutils -lcutils -lbase -lfmq -lc++ -ldl -llog \
  -o "$output_binary"
"$strip" --strip-unneeded "$output_binary"

elf_info=$("$readelf" -h -d "$output_binary")
grep -Eq 'Class:[[:space:]]+ELF64' <<<"$elf_info"
grep -Eq 'Machine:[[:space:]]+AArch64' <<<"$elf_info"
grep -Eq 'FLAGS_1.*PIE|Flags:.*PIE' <<<"$elf_info"
grep -Eq 'NEEDED.*\[android\.hardware\.bluetooth\.audio@2\.0\.so\]' \
  <<<"$elf_info"
grep -Eq 'NEEDED.*\[libc\+\+\.so\]' <<<"$elf_info"
if grep -Eq 'RPATH|RUNPATH' <<<"$elf_info"; then
  printf '%s\n' 'Provider session probe unexpectedly contains RPATH/RUNPATH' >&2
  exit 1
fi
printf 'Built provider session probe: %s\n' "$output_binary"
