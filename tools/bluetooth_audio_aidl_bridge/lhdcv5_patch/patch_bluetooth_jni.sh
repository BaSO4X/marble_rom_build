#!/bin/sh
set -eu

usage="Usage: patch_bluetooth_jni.sh ORIGINAL_LIBBLUETOOTH_JNI OUTPUT_LIBBLUETOOTH_JNI"
original=${1:?$usage}
output=${2:?$usage}
script_dir=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
expected_original_sha=4e77ce14149073eaa52579cf24494b3ea175fab43ce5888c984b8c9642ca5d0c
companion=/system/lib64/liblhdcv5_native_patch.so
patch_va=0x8d9c58
patch_size=28
expected_patch_site=88fd8052c9bafff0299d0191e81b00b9a8beff9008b92c91e92302a9
clang_bin=${AARCH64_CLANG:-aarch64-linux-android31-clang}
ld_bin=${LD_LLD:-ld.lld}
objcopy_bin=${LLVM_OBJCOPY:-llvm-objcopy}
readelf_bin=${LLVM_READELF:-llvm-readelf}

for command_name in patchelf "$clang_bin" "$ld_bin" "$objcopy_bin" \
    "$readelf_bin" dd od tr wc; do
  if ! command -v "$command_name" >/dev/null 2>&1; then
    echo "missing patch command: $command_name" >&2
    exit 1
  fi
done
if [ "$(patchelf --version)" != "patchelf 0.18.0" ]; then
  echo "patchelf 0.18.0 is required for reproducible output" >&2
  exit 1
fi
if [ ! -f "$original" ]; then
  echo "missing original libbluetooth_jni.so: $original" >&2
  exit 1
fi
if [ "$(sha256sum "$original" | awk '{print $1}')" != "$expected_original_sha" ]; then
  echo "unsupported libbluetooth_jni.so build" >&2
  exit 1
fi

mkdir -p "$(dirname "$output")"
work_dir=$(mktemp -d)
trap 'rm -rf -- "$work_dir"' EXIT

"$clang_bin" -c "$script_dir/timestamp_compat.S" \
  -o "$work_dir/timestamp_compat.o"
"$ld_bin" --build-id=none -e timestamp_compat_start \
  --section-start=.text="$patch_va" \
  --defsym=timestamp_success=0x8d9c24 \
  --defsym=timestamp_failure=0x8d9aec \
  "$work_dir/timestamp_compat.o" -o "$work_dir/timestamp_compat.elf"
"$objcopy_bin" --dump-section \
  ".text=$work_dir/timestamp_compat.bin" \
  "$work_dir/timestamp_compat.elf" /dev/null
if [ "$(wc -c <"$work_dir/timestamp_compat.bin" | tr -d ' ')" -ne \
    "$patch_size" ]; then
  echo "unexpected LHDC timestamp patch size" >&2
  exit 1
fi

cp -f "$original" "$output"
patchelf --add-needed "$companion" "$output"

needed_count=$(patchelf --print-needed "$output" | grep -Fxc "$companion")
if [ "$needed_count" -ne 1 ]; then
  echo "failed to add the absolute LHDCv5 companion dependency" >&2
  exit 1
fi

text_location=$("$readelf_bin" -SW "$output" | \
  awk '$2 == ".text" {print $4 ":" $5}')
case "$text_location" in
  *:*) ;;
  *)
  echo "cannot locate libbluetooth_jni.so .text section" >&2
  exit 1
  ;;
esac
text_va=$((0x${text_location%:*}))
text_offset=$((0x${text_location#*:}))
patch_offset=$((patch_va - text_va + text_offset))
actual_patch_site=$(dd if="$output" iflag=skip_bytes,count_bytes \
  skip="$patch_offset" count="$patch_size" status=none | \
  od -An -v -tx1 | tr -d ' \n')
if [ "$actual_patch_site" != "$expected_patch_site" ]; then
  echo "unsupported A2DP_GetPacketTimestamp patch site" >&2
  exit 1
fi
dd if="$work_dir/timestamp_compat.bin" of="$output" oflag=seek_bytes \
  seek="$patch_offset" conv=notrunc status=none

echo "Patched libbluetooth_jni.so: $output"
sha256sum "$output"
