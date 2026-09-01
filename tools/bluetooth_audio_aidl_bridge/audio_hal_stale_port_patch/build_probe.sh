#!/bin/bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
usage='Usage: build_probe.sh ANDROID_NDK OUTPUT_BINARY'
android_ndk=${1:?$usage}
output_binary=${2:?$usage}
toolchain="$android_ndk/toolchains/llvm/prebuilt/linux-x86_64/bin"
compiler="$toolchain/aarch64-linux-android32-clang"
strip="$toolchain/llvm-strip"
readelf="$toolchain/llvm-readelf"

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

"$compiler" -std=c17 -O2 -Wall -Wextra -Werror \
  -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fPIE \
  -mbranch-protection=standard -pie \
  -Wl,-z,relro,-z,now,--gc-sections -Wl,-z,max-page-size=4096 \
  "$script_dir/dlopen_probe.c" -ldl -o "$output_binary"
"$strip" --strip-unneeded "$output_binary"

elf_info=$("$readelf" -h -d --notes --wide "$output_binary")
grep -Eq 'Class:[[:space:]]+ELF64' <<<"$elf_info"
grep -Eq 'Machine:[[:space:]]+AArch64' <<<"$elf_info"
grep -Eq 'FLAGS_1.*PIE|Flags:.*PIE' <<<"$elf_info"
grep -Eq 'NEEDED.*\[libdl\.so\]' <<<"$elf_info"
grep -Eq 'aarch64 feature: BTI, PAC' <<<"$elf_info"
if grep -Eq 'RPATH|RUNPATH' <<<"$elf_info"; then
  printf '%s\n' 'HAL dlopen probe unexpectedly contains RPATH/RUNPATH' >&2
  exit 1
fi
printf 'Built Bluetooth Audio HAL dlopen probe: %s\n' "$output_binary"
