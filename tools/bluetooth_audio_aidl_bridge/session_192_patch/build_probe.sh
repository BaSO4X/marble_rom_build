#!/bin/bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
repo_root=$(cd -- "$script_dir/../../.." && pwd)
usage='Usage: build_probe.sh ANDROID_NDK VNDK32_ROOT PLATFORM_LIBCXX_ROOT GENERATED_2_1_HEADERS OUTPUT_BINARY'
android_ndk=${1:?$usage}
vndk_root=${2:?$usage}
platform_libcxx_root=${3:?$usage}
generated_2_1_headers=${4:?$usage}
output_binary=${5:?$usage}
toolchain="$android_ndk/toolchains/llvm/prebuilt/linux-x86_64/bin"
compiler="$toolchain/aarch64-linux-android31-clang++"
strip="$toolchain/llvm-strip"
readelf="$toolchain/llvm-readelf"
vndk_arm64="$vndk_root/arm64"
vndk_sp="$vndk_arm64/arch-arm64-armv8-a/shared/vndk-sp"

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

"$compiler" \
  -std=c++17 -O2 -Wall -Wextra -Werror \
  -D_FORTIFY_SOURCE=2 -D_LIBCPP_SUPPORT_XLOCALE_POSIX_L_FALLBACK_H \
  -fPIE -fvisibility=hidden -fno-exceptions -fno-rtti \
  -fstack-protector-strong -ffunction-sections -fdata-sections \
  -nostdinc++ -isystem "$platform_libcxx_root/include" -nostdlib++ \
  -I"$generated_2_1_headers" \
  -I"$repo_root/tools/bluetooth_audio_aidl_bridge/generated/aosp" \
  -I"$vndk_arm64/include/system/libhidl/base/include" \
  -I"$vndk_arm64/include/system/libhidl/transport/include" \
  -I"$vndk_arm64/include/system/libfmq/base" \
  -I"$vndk_arm64/include/system/libfmq/include" \
  -I"$vndk_arm64/include/system/core/libutils/include" \
  -I"$vndk_arm64/include/system/core/libcutils/include" \
  -I"$vndk_arm64/include/system/core/libsystem/include" \
  -I"$vndk_arm64/include/frameworks/native/libs/nativebase/include" \
  -I"$vndk_arm64/include/generated-headers/system/libhidl/transport/base/1.0/android.hidl.base@1.0_genc++_headers/gen" \
  -pie -Wl,-z,relro,-z,now,--as-needed,--gc-sections \
  -Wl,-z,max-page-size=4096 \
  "$script_dir/probe.cpp" \
  -L"$vndk_sp" -Wl,-rpath-link,"$vndk_sp" -lc++ -ldl \
  -o "$output_binary"
"$strip" --strip-unneeded "$output_binary"

elf_info=$("$readelf" -h -d "$output_binary")
grep -Eq 'Class:[[:space:]]+ELF64' <<<"$elf_info"
grep -Eq 'Machine:[[:space:]]+AArch64' <<<"$elf_info"
grep -Eq 'FLAGS_1.*PIE|Flags:.*PIE' <<<"$elf_info"
grep -Eq 'NEEDED.*\[libc\+\+\.so\]' <<<"$elf_info"
grep -Eq 'NEEDED.*\[libdl\.so\]' <<<"$elf_info"
if grep -Eq 'RPATH|RUNPATH' <<<"$elf_info"; then
  printf '%s\n' 'Session PCM probe unexpectedly contains RPATH/RUNPATH' >&2
  exit 1
fi
printf 'Built session PCM probe: %s\n' "$output_binary"
