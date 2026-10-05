#!/bin/bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
android_ndk=${1:?Usage: build.sh ANDROID_NDK VENDOR_ROOT RUNTIME_SDK DEVICE}
vendor_root=${2:?Usage: build.sh ANDROID_NDK VENDOR_ROOT RUNTIME_SDK DEVICE}
runtime_sdk=${3:?Usage: build.sh ANDROID_NDK VENDOR_ROOT RUNTIME_SDK DEVICE}
device=${4:?Usage: build.sh ANDROID_NDK VENDOR_ROOT RUNTIME_SDK DEVICE}
toolchain="$android_ndk/toolchains/llvm/prebuilt/linux-x86_64/bin"
compiler="$toolchain/armv7a-linux-androideabi31-clang"
readelf="$toolchain/llvm-readelf"

for tool in "$compiler" "$readelf" python3; do
  if ! command -v "$tool" >/dev/null 2>&1; then
    printf 'Missing AC-4 compatibility build tool: %s\n' "$tool" >&2
    exit 1
  fi
done
if [[ ! -d "$vendor_root" || ! "$runtime_sdk" =~ ^[0-9]+$ || -z "$device" ]]; then
  printf 'Invalid AC-4 build inputs: vendor=%s runtime_sdk=%s device=%s\n' \
    "$vendor_root" "$runtime_sdk" "$device" >&2
  exit 1
fi

temp_dir=$(mktemp -d)
trap 'rm -rf -- "$temp_dir"' EXIT
wrapper="$temp_dir/libstagefright_soft_ac4dec.so"

"$compiler" \
  -std=c11 -O2 -fPIC -fvisibility=hidden \
  -Wall -Wextra -Werror \
  -shared "$script_dir/ac4_omx_wrapper.c" \
  -Wl,-soname,libstagefright_soft_ac4wrap.so \
  -Wl,-z,relro,-z,now -Wl,--no-undefined \
  -ldl -llog \
  -o "$wrapper"

elf_header=$("$readelf" -h "$wrapper")
elf_dynamic=$("$readelf" -d "$wrapper")
elf_symbols=$("$readelf" --dyn-syms -W "$wrapper")
if ! grep -Eq 'Class:[[:space:]]+ELF32' <<<"$elf_header" ||
  ! grep -Eq 'Machine:[[:space:]]+ARM' <<<"$elf_header" ||
  ! grep -Eq 'Type:[[:space:]]+DYN([[:space:]]|$)' <<<"$elf_header"; then
  printf '%s\n' 'AC-4 wrapper is not an ELF32 ARM shared object' >&2
  exit 1
fi
if [[ $(grep -Fc 'Library soname: [libstagefright_soft_ac4wrap.so]' <<<"$elf_dynamic") -ne 1 ]]; then
  printf '%s\n' 'AC-4 wrapper SONAME validation failed' >&2
  exit 1
fi
mapfile -t needed < <(sed -n 's/.*NEEDED.*\[\([^]]*\)\].*/\1/p' <<<"$elf_dynamic")
if [[ ${#needed[@]} -ne 3 ]]; then
  printf 'Unexpected AC-4 wrapper dependencies: %s\n' "${needed[*]:-none}" >&2
  exit 1
fi
for library in libdl.so liblog.so libc.so; do
  if [[ $(printf '%s\n' "${needed[@]}" | grep -Fxc "$library") -ne 1 ]]; then
    printf 'AC-4 wrapper dependency validation failed: %s\n' "$library" >&2
    exit 1
  fi
done
factory_symbol=_Z22createSoftOMXComponentPKcPK16OMX_CALLBACKTYPEPvPP17OMX_COMPONENTTYPE
if [[ $(grep -Fwc "$factory_symbol" <<<"$elf_symbols") -ne 1 ]]; then
  printf '%s\n' 'AC-4 wrapper factory export validation failed' >&2
  exit 1
fi

python3 "$script_dir/apply.py" \
  --vendor-root "$vendor_root" \
  --wrapper "$wrapper" \
  --profiles "$script_dir/profiles.json" \
  --runtime-sdk "$runtime_sdk" \
  --device "$device"
printf 'Installed verified AC-4 compatibility files into %s\n' "$vendor_root"
