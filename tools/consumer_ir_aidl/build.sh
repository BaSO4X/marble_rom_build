#!/bin/bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
android_ndk=${1:?Usage: build.sh ANDROID_NDK OUTPUT_BINARY}
output_binary=${2:?Usage: build.sh ANDROID_NDK OUTPUT_BINARY}
toolchain="$android_ndk/toolchains/llvm/prebuilt/linux-x86_64/bin"
compiler="$toolchain/aarch64-linux-android31-clang++"
readelf="$toolchain/llvm-readelf"
strip="$toolchain/llvm-strip"

if [[ ! -x "$compiler" || ! -x "$readelf" || ! -x "$strip" ]]; then
  printf 'Android NDK toolchain is incomplete: %s\n' "$toolchain" >&2
  exit 1
fi

mkdir -p -- "$(dirname -- "$output_binary")"
"$compiler" \
  -std=c++17 -O2 -Wall -Wextra -Werror -pthread \
  -D_FORTIFY_SOURCE=2 \
  -fstack-protector-strong -ffunction-sections -fdata-sections -fPIE -pie \
  -Wl,-z,relro,-z,now,--as-needed,--gc-sections \
  -Wl,-z,max-page-size=16384 -static-libstdc++ \
  -I"$script_dir/generated/include" \
  "$script_dir/consumer_ir_service.cpp" \
  "$script_dir/generated/src/android/hardware/ir/ConsumerIrFreqRange.cpp" \
  "$script_dir/generated/src/android/hardware/ir/IConsumerIr.cpp" \
  -o "$output_binary" \
  -lbinder_ndk -ldl -llog
"$strip" --strip-unneeded "$output_binary"

elf_info=$("$readelf" -h -d "$output_binary")
check_elf() {
  local pattern=$1
  local description=$2
  if ! grep -Eq "$pattern" <<<"$elf_info"; then
    printf 'Consumer IR AIDL ELF validation failed: %s\n' "$description" >&2
    exit 1
  fi
}

check_elf 'Class:[[:space:]]+ELF64' 'not ELF64'
check_elf 'Machine:[[:space:]]+AArch64' 'not AArch64'
check_elf 'Type:[[:space:]]+DYN([[:space:]]|$)' 'not a dynamic PIE'
check_elf 'FLAGS_1.*PIE|Flags:.*PIE' 'missing PIE flag'
for library in libbinder_ndk.so libdl.so liblog.so libc.so; do
  check_elf "NEEDED.*\\[$library\\]" "missing $library dependency"
done
if grep -Eq 'NEEDED.*\[(android\.hardware\.ir-V[0-9]+-ndk|libc\+\+_shared)\.so\]' \
  <<<"$elf_info"; then
  printf '%s\n' 'Consumer IR AIDL service has a forbidden VNDK33 or NDK C++ dependency' >&2
  exit 1
fi

printf 'Built %s\n' "$output_binary"
