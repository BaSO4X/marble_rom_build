#!/bin/sh
set -eu

script_dir=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
usage='Usage: build.sh ANDROID_NDK LHDCV5_CORE_ARCHIVE OUTPUT_DIR STOCK_LIBBLUETOOTH_JNI'
ndk_root=${1:-${ANDROID_NDK_ROOT:-}}
core_archive=${2:-${LHDCV5_CORE_ARCHIVE:-}}
output_dir=${3:-${OUT_DIR:-}}
stock_library=${4:-${STOCK_LIBBLUETOOTH_JNI:-}}
if [ -z "$ndk_root" ] || [ -z "$core_archive" ] || [ -z "$output_dir" ] || \
    [ -z "$stock_library" ]; then
  echo "$usage" >&2
  exit 1
fi
compiler="$ndk_root/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android31-clang++"
strip="$ndk_root/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip"

if [ ! -x "$compiler" ]; then
  echo "missing Android compiler: $compiler" >&2
  exit 1
fi
if [ ! -f "$core_archive" ]; then
  echo "missing LHDCv5 core archive: $core_archive" >&2
  exit 1
fi
if [ ! -f "$stock_library" ]; then
  echo "missing stock libbluetooth_jni.so: $stock_library" >&2
  exit 1
fi
if ! command -v python3 >/dev/null 2>&1; then
  echo "missing patch command: python3" >&2
  exit 1
fi

mkdir -p "$output_dir"
# Both patches refuse to install themselves unless the running Bluetooth
# library is the one these addresses came from, so regenerate the profile from
# whichever build this ROM ships.
generated_dir="$output_dir/generated"
mkdir -p "$generated_dir"
python3 "$script_dir/derive_profile.py" --format=header \
  --output="$generated_dir/bluetooth_jni_profile.h" "$stock_library"
output="$output_dir/liblhdcv5_native_patch.so"
temporary=$(mktemp "$output_dir/.liblhdcv5_native_patch.so.XXXXXX")
trap 'rm -f -- "$temporary"' EXIT
"$compiler" \
  -std=c++17 \
  -fPIC \
  -fvisibility=hidden \
  -ffunction-sections \
  -fdata-sections \
  -fno-exceptions \
  -fno-rtti \
  -mbranch-protection=standard \
  -O2 \
  -Wall \
  -Wextra \
  -Werror \
  -shared \
  -nostdlib++ \
  -I"$generated_dir" \
  -Wl,-soname,liblhdcv5_native_patch.so \
  -Wl,-z,relro,-z,now \
  -Wl,--gc-sections \
  -Wl,--exclude-libs,ALL \
  "$script_dir/lhdcv5_native_patch.cpp" \
  "$script_dir/lhdc_legacy_patch.cpp" \
  -Wl,--whole-archive "$core_archive" -Wl,--no-whole-archive \
  -llog -ldl -lm \
  -o "$temporary"
"$strip" --strip-unneeded "$temporary"
chmod 0644 "$temporary"
mv -f -- "$temporary" "$output"
trap - EXIT

echo "$output"
