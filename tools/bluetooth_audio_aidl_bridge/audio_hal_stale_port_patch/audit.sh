#!/bin/bash
set -euo pipefail

usage='Usage: audit.sh ANDROID_NDK ORIGINAL_HAL BASELINE_HAL PATCHED_HAL'
android_ndk=${1:?$usage}
original_hal=${2:?$usage}
baseline_hal=${3:?$usage}
patched_hal=${4:?$usage}
toolchain="$android_ndk/toolchains/llvm/prebuilt/linux-x86_64/bin"
readelf="$toolchain/llvm-readelf"
nm="$toolchain/llvm-nm"
objdump="$toolchain/llvm-objdump"

for tool in "$readelf" "$nm" "$objdump"; do
  if [[ ! -x "$tool" ]]; then
    printf 'Required Android tool is missing: %s\n' "$tool" >&2
    exit 1
  fi
done
for file in "$original_hal" "$baseline_hal" "$patched_hal"; do
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
public_hal_symbols() {
  defined_symbols "$1" |
    grep -E '^(HMI|adev_|__cfi_check|_ZNK?7android9bluetooth5audio|_ZTVN7android9bluetooth5audio|_Zls.*BluetoothStreamState)'
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
  info=$("$readelf" -h -d --notes --dyn-syms --wide "$file")
  grep -Eq 'Class:[[:space:]]+ELF64' <<<"$info"
  grep -Eq 'Machine:[[:space:]]+AArch64' <<<"$info"
  grep -Eq 'Type:[[:space:]]+DYN([[:space:]]|$)' <<<"$info"
  grep -Eq 'SONAME.*\[audio\.bluetooth\.default\.so\]' <<<"$info"
  grep -Eq 'FLAGS.*BIND_NOW' <<<"$info"
  grep -Eq 'ANDROID_RELA' <<<"$info"
  grep -Eq '\(RELR\)' <<<"$info"
  grep -Eq 'aarch64 feature: BTI, PAC' <<<"$info"
  grep -Eq 'GLOBAL.*DEFAULT.*__cfi_check$' <<<"$info"
  grep -Eq 'GLOBAL.*DEFAULT.*UND __cfi_slowpath@' <<<"$info"
  if grep -Eq 'ANDROID_RELR|RPATH|RUNPATH' <<<"$info"; then
    printf 'Unexpected relocation tag or search path in %s\n' "$file" >&2
    exit 1
  fi
}

for kind in original baseline patched; do
  case "$kind" in
    original) file=$original_hal ;;
    baseline) file=$baseline_hal ;;
    patched) file=$patched_hal ;;
  esac
  needed "$file" >"$temp_dir/$kind.needed"
  defined_symbols "$file" >"$temp_dir/$kind.defined"
  undefined_symbols "$file" >"$temp_dir/$kind.undefined"
  public_hal_symbols "$file" >"$temp_dir/$kind.public"
  assert_elf_properties "$file"
done

assert_same 'original/baseline NEEDED order' \
  "$temp_dir/original.needed" "$temp_dir/baseline.needed"
assert_same 'baseline/patched NEEDED order' \
  "$temp_dir/baseline.needed" "$temp_dir/patched.needed"
assert_same 'baseline/patched defined dynamic symbols' \
  "$temp_dir/baseline.defined" "$temp_dir/patched.defined"
assert_same 'baseline/patched undefined dynamic symbols' \
  "$temp_dir/baseline.undefined" "$temp_dir/patched.undefined"

comm -23 "$temp_dir/original.public" "$temp_dir/baseline.public" \
  >"$temp_dir/baseline.missing-original-symbols"
if [[ -s "$temp_dir/baseline.missing-original-symbols" ]]; then
  printf '%s\n' 'Baseline HAL misses original dynamic symbols:' >&2
  cat "$temp_dir/baseline.missing-original-symbols" >&2
  exit 1
fi

baseline_source="$baseline_hal.work/audio_bluetooth_hw"
patched_source="$patched_hal.work/audio_bluetooth_hw"
for source in audio_bluetooth_hw.cc stream_apis.cc utils.cc \
  device_port_proxy.h stream_apis.h utils.h; do
  assert_same "unexpected source delta in $source" \
    "$baseline_source/$source" "$patched_source/$source"
done
if ! grep -q 'stale PCM cannot enter the new' \
    "$patched_source/device_port_proxy.cc"; then
  printf '%s\n' 'Patched SessionChangedHandler marker is missing' >&2
  exit 1
fi
if [[ $(grep -c 'A2DP_SOFTWARE_ENCODING_DATAPATH' \
    "$patched_source/device_port_proxy.cc") -ne \
    $(( $(grep -c 'A2DP_SOFTWARE_ENCODING_DATAPATH' \
      "$baseline_source/device_port_proxy.cc") + 1 )) ]]; then
  printf '%s\n' 'A2DP stale-port source edit did not apply exactly once' >&2
  exit 1
fi
if cmp -s "$baseline_source/device_port_proxy.cc" \
    "$patched_source/device_port_proxy.cc"; then
  printf '%s\n' 'Patched device_port_proxy.cc is unchanged' >&2
  exit 1
fi

"$objdump" -d --demangle --no-show-raw-insn "$patched_hal" |
  sed -n '/<android::bluetooth::audio::BluetoothAudioPort::SessionChangedHandler()>:/,/^$/p' \
  >"$temp_dir/patched.session-changed.disassembly"
for instruction in \
  'ldrb[[:space:]]+w[0-9]+, \[x[0-9]+, #0xb\]' \
  'cmp[[:space:]]+w[0-9]+, #0x1' \
  'ccmp[[:space:]]+w[0-9]+, #0x0, #0x0, ne' \
  'cset[[:space:]]+w[0-9]+, eq' \
  'strb[[:space:]]+w[0-9]+, \[x[0-9]+, #0xa\]'; do
  if ! grep -Eq "$instruction" \
      "$temp_dir/patched.session-changed.disassembly"; then
    printf 'Patched session state instruction is missing: %s\n' \
      "$instruction" >&2
    exit 1
  fi
done

sha256sum "$original_hal" "$baseline_hal" "$patched_hal"
printf '%s\n' 'Bluetooth Audio stale-port HAL audit passed'
