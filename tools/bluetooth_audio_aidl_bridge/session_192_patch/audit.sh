#!/bin/bash
set -euo pipefail

usage='Usage: audit.sh ANDROID_NDK ORIGINAL_PROVIDER ORIGINAL_SESSION BASELINE_SESSION PCM192_SESSION'
android_ndk=${1:?$usage}
original_provider=${2:?$usage}
original_session=${3:?$usage}
baseline_session=${4:?$usage}
pcm192_session=${5:?$usage}
toolchain="$android_ndk/toolchains/llvm/prebuilt/linux-x86_64/bin"
readelf="$toolchain/llvm-readelf"
nm="$toolchain/llvm-nm"

for tool in "$readelf" "$nm"; do
  if [[ ! -x "$tool" ]]; then
    printf 'Required Android tool is missing: %s\n' "$tool" >&2
    exit 1
  fi
done
for file in "$original_provider" "$original_session" "$baseline_session" \
  "$pcm192_session"; do
  if [[ ! -f "$file" ]]; then
    printf 'Required audit input is missing: %s\n' "$file" >&2
    exit 1
  fi
done

temp_dir=$(mktemp -d)
trap 'rm -rf -- "$temp_dir"' EXIT

needed() {
  "$readelf" -d "$1" |
    sed -n 's/.*Shared library: \[\([^]]*\)\].*/\1/p'
}
defined_symbols() {
  "$nm" -D --defined-only --format=posix "$1" |
    cut -d' ' -f1 | sort -u
}
undefined_symbols() {
  "$nm" -D --undefined-only --format=posix "$1" |
    cut -d' ' -f1 | sed 's/@[^@]*$//' | sort -u
}
public_session_symbols() {
  defined_symbols "$1" | grep -E '^_ZNK?7android9bluetooth5audio'
}
assert_same() {
  local description=$1
  local left=$2
  local right=$3
  if ! cmp -s "$left" "$right"; then
    printf 'Audit mismatch: %s\n' "$description" >&2
    diff -u "$left" "$right" >&2 || true
    exit 1
  fi
}
assert_elf_properties() {
  local file=$1
  local info
  info=$("$readelf" -h -d --dyn-syms --wide "$file")
  grep -Eq 'Class:[[:space:]]+ELF64' <<<"$info"
  grep -Eq 'Machine:[[:space:]]+AArch64' <<<"$info"
  grep -Eq 'Type:[[:space:]]+DYN([[:space:]]|$)' <<<"$info"
  grep -Eq 'SONAME.*\[libbluetooth_audio_session\.so\]' <<<"$info"
  grep -Eq 'FLAGS.*BIND_NOW' <<<"$info"
  grep -Eq '\(ANDROID_RELA\)' <<<"$info"
  grep -Eq '\(RELR\)' <<<"$info"
  if grep -Eq '\(ANDROID_RELR\)|RPATH|RUNPATH' <<<"$info"; then
    printf 'Unexpected relocation tag or search path in %s\n' "$file" >&2
    exit 1
  fi
  grep -Eq 'GLOBAL.*DEFAULT.*__cfi_check$' <<<"$info"
  grep -Eq 'GLOBAL.*DEFAULT.*UND __cfi_slowpath@' <<<"$info"
}

for kind in original baseline pcm192; do
  case "$kind" in
    original) file=$original_session ;;
    baseline) file=$baseline_session ;;
    pcm192) file=$pcm192_session ;;
  esac
  needed "$file" > "$temp_dir/$kind.needed"
  defined_symbols "$file" > "$temp_dir/$kind.defined"
  undefined_symbols "$file" > "$temp_dir/$kind.undefined"
  public_session_symbols "$file" > "$temp_dir/$kind.public"
  assert_elf_properties "$file"
done

assert_same 'original/baseline NEEDED order' \
  "$temp_dir/original.needed" "$temp_dir/baseline.needed"
assert_same 'baseline/PCM192 NEEDED order' \
  "$temp_dir/baseline.needed" "$temp_dir/pcm192.needed"
assert_same 'original/baseline public session ABI' \
  "$temp_dir/original.public" "$temp_dir/baseline.public"
assert_same 'baseline/PCM192 defined dynamic symbols' \
  "$temp_dir/baseline.defined" "$temp_dir/pcm192.defined"
assert_same 'baseline/PCM192 undefined dynamic symbols' \
  "$temp_dir/baseline.undefined" "$temp_dir/pcm192.undefined"

undefined_symbols "$original_provider" |
  grep -E '^_ZNK?7android9bluetooth5audio' > "$temp_dir/provider.required"
comm -23 "$temp_dir/provider.required" "$temp_dir/pcm192.defined" \
  > "$temp_dir/provider.missing"
if [[ -s "$temp_dir/provider.missing" ]]; then
  printf '%s\n' 'PCM192 session library misses provider-required symbols:' >&2
  cat "$temp_dir/provider.missing" >&2
  exit 1
fi

baseline_source="$baseline_session.work/session"
pcm192_source="$pcm192_session.work/session"
for source in BluetoothAudioSupportedCodecsDB.cpp \
  BluetoothAudioSupportedCodecsDB_2_1.cpp; do
  if [[ $(grep -c 'RATE_192000' "$baseline_source/$source") -ne 0 ||
        $(grep -c 'RATE_192000' "$pcm192_source/$source") -ne 2 ]]; then
    printf 'Unexpected PCM 192 source delta in %s\n' "$source" >&2
    exit 1
  fi
done
while IFS= read -r -d '' source; do
  relative=${source#"$baseline_source/"}
  case "$relative" in
    BluetoothAudioSupportedCodecsDB.cpp|BluetoothAudioSupportedCodecsDB_2_1.cpp)
      continue
      ;;
  esac
  if ! cmp -s "$source" "$pcm192_source/$relative"; then
    printf 'Unexpected source delta outside PCM capability DB: %s\n' \
      "$relative" >&2
    exit 1
  fi
done < <(find "$baseline_source" -type f -print0)

sha256sum "$original_session" "$baseline_session" "$pcm192_session"
printf '%s\n' 'Bluetooth Audio session PCM 192 audit passed'
