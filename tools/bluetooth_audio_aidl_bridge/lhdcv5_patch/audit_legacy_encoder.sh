#!/bin/sh
set -eu

usage="Usage: audit_legacy_encoder.sh ORIGINAL_LIBLHDC PATCHED_LIBLHDC ORIGINAL_LIBLHDCBT_ENC PATCHED_LIBLHDCBT_ENC"
original_core=${1:?$usage}
patched_core=${2:?$usage}
original=${3:?$usage}
patched=${4:?$usage}
readelf_bin=${LLVM_READELF:-llvm-readelf}
objcopy_bin=${LLVM_OBJCOPY:-llvm-objcopy}
nm_bin=${LLVM_NM:-llvm-nm}
expected_original_core_sha=3cdc73f296a56864e245dfca8385d2d4f9a21793d05336c754779802b984573a
expected_patched_core_sha=0291f29f4b7a5a86bfa2a7a2a4b6332fd2c7f338fff113d9e614461c2ffc9a9e
expected_original_sha=bc19fc94d1ca129376989dc105f93c273cad376e7399177424ea59e341fc0cc4
absolute_core=/system/lib64/liblhdc.so

for command_name in "$readelf_bin" "$objcopy_bin" "$nm_bin" awk cmp grep mktemp sed sha256sum sort; do
  if ! command -v "$command_name" >/dev/null 2>&1; then
    echo "missing audit command: $command_name" >&2
    exit 1
  fi
done
for input in "$original_core" "$patched_core" "$original" "$patched"; do
  if [ ! -f "$input" ]; then
    echo "missing legacy encoder audit input: $input" >&2
    exit 1
  fi
done
if [ "$(sha256sum "$original_core" | awk '{print $1}')" != "$expected_original_core_sha" ] ||
    [ "$(sha256sum "$patched_core" | awk '{print $1}')" != "$expected_patched_core_sha" ] ||
    [ "$(sha256sum "$original" | awk '{print $1}')" != "$expected_original_sha" ]; then
  echo "legacy encoder input hash mismatch" >&2
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

needed_values "$original_core" | grep -Fvx libstdc++.so | sort >"$work_dir/original-core-needed"
needed_values "$patched_core" | sort >"$work_dir/patched-core-needed"
cmp "$work_dir/original-core-needed" "$work_dir/patched-core-needed"
if [ "$(needed_values "$original_core" | grep -Fxc libstdc++.so)" -ne 1 ] ||
    needed_values "$patched_core" | grep -Fqx libstdc++.so; then
  echo "patched core dependency list is invalid" >&2
  exit 1
fi

"$nm_bin" -D --undefined-only "$original_core" | sort >"$work_dir/original-core-undefined"
"$nm_bin" -D --undefined-only "$patched_core" | sort >"$work_dir/patched-core-undefined"
cmp "$work_dir/original-core-undefined" "$work_dir/patched-core-undefined"
if grep -Eq '[[:space:]]U (_Z|__gxx_|_Unwind_)' "$work_dir/original-core-undefined"; then
  echo "legacy core still imports a C++ runtime symbol" >&2
  exit 1
fi

for section in .text .note.gnu.build-id; do
  "$objcopy_bin" --dump-section "$section=$work_dir/original-core-section" \
    "$original_core" /dev/null
  "$objcopy_bin" --dump-section "$section=$work_dir/patched-core-section" \
    "$patched_core" /dev/null
  cmp "$work_dir/original-core-section" "$work_dir/patched-core-section"
done

original_core_soname=$(dynamic_values "$original_core" | sed -n 's/.*Library soname: \[\(.*\)\]/\1/p')
patched_core_soname=$(dynamic_values "$patched_core" | sed -n 's/.*Library soname: \[\(.*\)\]/\1/p')
if [ "$original_core_soname" != liblhdc.so ] ||
    [ "$patched_core_soname" != "$original_core_soname" ]; then
  echo "legacy core SONAME mismatch" >&2
  exit 1
fi
if dynamic_values "$patched_core" | grep -Eq 'RPATH|RUNPATH'; then
  echo "patched legacy core unexpectedly contains RPATH/RUNPATH" >&2
  exit 1
fi

needed_values "$original" | grep -Fvx liblhdc.so | sort >"$work_dir/original-needed"
needed_values "$patched" | grep -Fvx "$absolute_core" | sort >"$work_dir/patched-needed"
cmp "$work_dir/original-needed" "$work_dir/patched-needed"
if [ "$(needed_values "$patched" | grep -Fxc "$absolute_core")" -ne 1 ] ||
    needed_values "$patched" | grep -Fqx liblhdc.so; then
  echo "patched encoder dependency list is invalid" >&2
  exit 1
fi

for section in .text .note.gnu.build-id; do
  "$objcopy_bin" --dump-section "$section=$work_dir/original-section" "$original" /dev/null
  "$objcopy_bin" --dump-section "$section=$work_dir/patched-section" "$patched" /dev/null
  cmp "$work_dir/original-section" "$work_dir/patched-section"
done

original_soname=$(dynamic_values "$original" | sed -n 's/.*Library soname: \[\(.*\)\]/\1/p')
patched_soname=$(dynamic_values "$patched" | sed -n 's/.*Library soname: \[\(.*\)\]/\1/p')
if [ "$original_soname" != liblhdcBT_enc.so ] || [ "$patched_soname" != "$original_soname" ]; then
  echo "legacy encoder SONAME mismatch" >&2
  exit 1
fi
dynamic_values "$patched_core" | grep -q 'Library soname: \[liblhdc.so\]'
if dynamic_values "$patched" | grep -Eq 'RPATH|RUNPATH'; then
  echo "patched legacy encoder unexpectedly contains RPATH/RUNPATH" >&2
  exit 1
fi

echo "Legacy LHDC encoder ELF audit passed"
sha256sum "$original_core" "$patched_core" "$original" "$patched"
