#!/bin/sh
set -eu

usage="Usage: audit.sh ORIGINAL_LIBBLUETOOTH_JNI PATCHED_LIBBLUETOOTH_JNI LHDCV5_COMPANION"
original=${1:?$usage}
patched=${2:?$usage}
companion=${3:?$usage}
readelf_bin=${LLVM_READELF:-llvm-readelf}
objcopy_bin=${LLVM_OBJCOPY:-llvm-objcopy}
objdump_bin=${LLVM_OBJDUMP:-llvm-objdump}
expected_original_sha=4e77ce14149073eaa52579cf24494b3ea175fab43ce5888c984b8c9642ca5d0c
absolute_companion=/system/lib64/liblhdcv5_native_patch.so
patch_va=0x8d9c58
patch_text_offset=$((patch_va - 0x34c000))
patch_size=28
expected_original_site=88fd8052c9bafff0299d0191e81b00b9a8beff9008b92c91e92302a9

for command_name in "$readelf_bin" "$objcopy_bin" "$objdump_bin" awk cmp dd \
    grep head mktemp od sed sha256sum sort tail tr wc; do
  if ! command -v "$command_name" >/dev/null 2>&1; then
    echo "missing audit command: $command_name" >&2
    exit 1
  fi
done
for input in "$original" "$patched" "$companion"; do
  if [ ! -f "$input" ]; then
    echo "missing audit input: $input" >&2
    exit 1
  fi
done
if [ "$(sha256sum "$original" | awk '{print $1}')" != "$expected_original_sha" ]; then
  echo "original library hash mismatch" >&2
  exit 1
fi

work_dir=$(mktemp -d)
trap 'rm -rf -- "$work_dir"' EXIT

dynamic_values() {
  "$readelf_bin" -d "$1"
}
needed_values() {
  dynamic_values "$1" | sed -n 's/.*Shared library: \[\(.*\)\]/\1/p'
}

needed_values "$original" | sort >"$work_dir/original-needed"
needed_values "$patched" | grep -Fvx "$absolute_companion" | sort \
  >"$work_dir/patched-original-needed"
cmp "$work_dir/original-needed" "$work_dir/patched-original-needed"
if [ "$(needed_values "$patched" | grep -Fxc "$absolute_companion")" -ne 1 ]; then
  echo "patched library does not contain exactly one absolute companion dependency" >&2
  exit 1
fi

for section in .data.rel.ro .gnu_debugdata .note.gnu.build-id; do
  "$objcopy_bin" --dump-section "$section=$work_dir/original-section" "$original" /dev/null
  "$objcopy_bin" --dump-section "$section=$work_dir/patched-section" "$patched" /dev/null
  cmp "$work_dir/original-section" "$work_dir/patched-section"
done

"$objcopy_bin" --dump-section ".text=$work_dir/original-text" "$original" /dev/null
"$objcopy_bin" --dump-section ".text=$work_dir/patched-text" "$patched" /dev/null
if [ "$(wc -c <"$work_dir/original-text" | tr -d ' ')" != \
    "$(wc -c <"$work_dir/patched-text" | tr -d ' ')" ]; then
  echo "patched .text size changed" >&2
  exit 1
fi
original_site=$(dd if="$work_dir/original-text" iflag=skip_bytes,count_bytes \
  skip="$patch_text_offset" count="$patch_size" status=none | \
  od -An -v -tx1 | tr -d ' \n')
if [ "$original_site" != "$expected_original_site" ]; then
  echo "original timestamp patch site mismatch" >&2
  exit 1
fi
head -c "$patch_text_offset" "$work_dir/original-text" \
  >"$work_dir/original-prefix"
head -c "$patch_text_offset" "$work_dir/patched-text" \
  >"$work_dir/patched-prefix"
cmp "$work_dir/original-prefix" "$work_dir/patched-prefix"
suffix_offset=$((patch_text_offset + patch_size))
tail -c +$((suffix_offset + 1)) "$work_dir/original-text" \
  >"$work_dir/original-suffix"
tail -c +$((suffix_offset + 1)) "$work_dir/patched-text" \
  >"$work_dir/patched-suffix"
cmp "$work_dir/original-suffix" "$work_dir/patched-suffix"

"$objdump_bin" -d --start-address="$patch_va" \
  --stop-address=$((patch_va + patch_size)) "$patched" \
  >"$work_dir/timestamp-disassembly"
for pattern in \
    'mov[[:space:]]+x9, #0x3aff' \
    'movk[[:space:]]+x9, #0x3205, lsl #16' \
    'movk[[:space:]]+x9, #0x4c, lsl #32' \
    'eor[[:space:]]+x9, x8, x9' \
    'tst[[:space:]]+x9, #0xfffffffffeffffff' \
    'b.eq[[:space:]]+0x8d9c24' \
    'b[[:space:]]+0x8d9aec'; do
  if ! grep -Eq "$pattern" "$work_dir/timestamp-disassembly"; then
    echo "timestamp compatibility patch audit failed: $pattern" >&2
    exit 1
  fi
done

original_soname=$(dynamic_values "$original" | sed -n 's/.*Library soname: \[\(.*\)\]/\1/p')
patched_soname=$(dynamic_values "$patched" | sed -n 's/.*Library soname: \[\(.*\)\]/\1/p')
if [ "$original_soname" != "$patched_soname" ]; then
  echo "SONAME changed while patching" >&2
  exit 1
fi
for tag in ANDROID_RELA ANDROID_RELR BIND_NOW; do
  dynamic_values "$patched" | grep -q "$tag"
done

needed_values "$companion" | sort >"$work_dir/companion-needed"
printf '%s\n' libc.so libdl.so liblog.so libm.so | sort >"$work_dir/allowed-needed"
cmp "$work_dir/allowed-needed" "$work_dir/companion-needed"
dynamic_values "$companion" | grep -q 'Library soname: \[liblhdcv5_native_patch.so\]'
dynamic_values "$companion" | grep -q 'BIND_NOW'
dynamic_values "$companion" | grep -q 'INIT_ARRAY'
if dynamic_values "$companion" | grep -Eq 'RPATH|RUNPATH'; then
  echo "companion unexpectedly contains RPATH/RUNPATH" >&2
  exit 1
fi

echo "LHDCv5 ELF audit passed"
sha256sum "$original" "$patched" "$companion"
