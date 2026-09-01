#!/bin/bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
usage='Usage: build.sh ANDROID_NDK VNDK32_ROOT PLATFORM_LIBCXX_ROOT OUTPUT_BINARY'
android_ndk=${1:?$usage}
vndk_root=${2:?$usage}
platform_libcxx_root=${3:?$usage}
output_binary=${4:?$usage}
toolchain="$android_ndk/toolchains/llvm/prebuilt/linux-x86_64/bin"
compiler="$toolchain/aarch64-linux-android31-clang++"
readelf="$toolchain/llvm-readelf"
strip="$toolchain/llvm-strip"
vndk_arm64="$vndk_root/arm64"
vndk_sp="$vndk_arm64/arch-arm64-armv8-a/shared/vndk-sp"
vndk_core="$vndk_arm64/arch-arm64-armv8-a/shared/vndk-core"
llndk_stub="$vndk_arm64/arch-arm64-armv8-a/shared/llndk-stub"

for tool in "$compiler" "$readelf" "$strip"; do
  if [[ ! -x "$tool" ]]; then
    printf 'Required Android tool is missing: %s\n' "$tool" >&2
    exit 1
  fi
done
if [[ ! -f "$platform_libcxx_root/include/__config" ]]; then
  printf 'Android platform libc++ headers are missing: %s\n' \
    "$platform_libcxx_root/include/__config" >&2
  exit 1
fi
for library in libc++.so libhidlbase.so libutils.so libcutils.so libbase.so; do
  if [[ ! -f "$vndk_sp/$library" ]]; then
    printf 'Required VNDK 32 library is missing: %s\n' "$vndk_sp/$library" >&2
    exit 1
  fi
done
for library in libbinder.so libfmq.so; do
  if [[ ! -f "$vndk_core/$library" ]]; then
    printf 'Required VNDK 32 library is missing: %s\n' "$vndk_core/$library" >&2
    exit 1
  fi
done
if [[ ! -f "$llndk_stub/liblog.so" ]]; then
  printf 'Required VNDK 32 LL-NDK stub is missing: %s\n' \
    "$llndk_stub/liblog.so" >&2
  exit 1
fi

sources=("$script_dir/bluetooth_audio_bridge.cpp")
while IFS= read -r -d '' source; do
  sources+=("$source")
done < <(find "$script_dir/generated/aidl/source" -type f -name '*.cpp' -print0 | sort -z)
while IFS= read -r -d '' source; do
  sources+=("$source")
done < <(find "$script_dir/generated/qti" -type f \
  \( -name '*All.cpp' -o -name 'types.cpp' \) -print0 | sort -z)
while IFS= read -r -d '' source; do
  sources+=("$source")
done < <(find "$script_dir/generated/aosp" -type f \
  \( -name '*All.cpp' -o -name 'types.cpp' \) -print0 | sort -z)

includes=(
  -I"$script_dir/generated/aidl/include"
  -I"$script_dir/generated/qti"
  -I"$script_dir/generated/aosp"
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
  -D_FORTIFY_SOURCE=2
  # NDK r27 Bionic already declares the locale-aware ctype functions that the
  # Android 12L libc++ headers otherwise provide as compatibility fallbacks.
  -D_LIBCPP_SUPPORT_XLOCALE_POSIX_L_FALLBACK_H
  -fno-rtti -fstack-protector-strong -ffunction-sections -fdata-sections -fPIE
  -nostdinc++ -isystem "$platform_libcxx_root/include"
  -nostdlib++
)
output_dir=$(dirname -- "$output_binary")
object_dir="$output_binary.objects"
mkdir -p -- "$output_dir" "$object_dir"
objects=()
job_limit=${BT_BRIDGE_JOBS:-$(nproc)}
if ((job_limit < 1)); then
  job_limit=1
elif ((job_limit > 6)); then
  job_limit=6
fi
active_jobs=0
for index in "${!sources[@]}"; do
  object="$object_dir/$index.o"
  objects+=("$object")
  "$compiler" "${compile_flags[@]}" "${includes[@]}" \
    -c "${sources[$index]}" -o "$object" &
  ((active_jobs += 1))
  if ((active_jobs >= job_limit)); then
    wait -n
    ((active_jobs -= 1))
  fi
done
wait

"$compiler" -fPIE -pie -nostdlib++ \
  -Wl,-z,relro,-z,now,--as-needed,--gc-sections \
  -Wl,-z,max-page-size=16384 \
  "${objects[@]}" \
  -L"$llndk_stub" -L"$vndk_sp" -L"$vndk_core" \
  -Wl,-rpath-link,"$vndk_sp" -Wl,-rpath-link,"$vndk_core" \
  -o "$output_binary" \
  -lbinder_ndk -lhidlbase -lutils -lcutils -lbase -lfmq -lbinder \
  -lc++ -ldl -llog
"$strip" --strip-unneeded "$output_binary"

elf_info=$("$readelf" -h -d "$output_binary")
check_elf() {
  local pattern=$1
  local description=$2
  if ! grep -Eq "$pattern" <<<"$elf_info"; then
    printf 'Bluetooth Audio AIDL bridge validation failed: %s\n' \
      "$description" >&2
    exit 1
  fi
}

check_elf 'Class:[[:space:]]+ELF64' 'not ELF64'
check_elf 'Machine:[[:space:]]+AArch64' 'not AArch64'
check_elf 'Type:[[:space:]]+DYN([[:space:]]|$)' 'not a dynamic PIE'
check_elf 'FLAGS_1.*PIE|Flags:.*PIE' 'missing PIE flag'
for library in libbinder_ndk.so libhidlbase.so libutils.so libc++.so liblog.so libc.so; do
  check_elf "NEEDED.*\\[$library\\]" "missing $library dependency"
done
if grep -Eq 'NEEDED.*\[(android\.hardware\.bluetooth\.audio|vendor\.qti\.hardware\.bluetooth_audio)@?-?[^]]*\.so\]' <<<"$elf_info"; then
  printf '%s\n' 'Bridge unexpectedly depends on generated interface shared libraries' >&2
  exit 1
fi

printf 'Built %s from %d source files\n' "$output_binary" "${#sources[@]}"
