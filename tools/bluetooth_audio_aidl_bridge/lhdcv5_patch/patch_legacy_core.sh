#!/bin/sh
set -eu

usage="Usage: patch_legacy_core.sh ORIGINAL_LIBLHDC OUTPUT_LIBLHDC"
original=${1:?$usage}
output=${2:?$usage}
expected_original_sha=3cdc73f296a56864e245dfca8385d2d4f9a21793d05336c754779802b984573a
unused_dependency=libstdc++.so

if ! command -v patchelf >/dev/null 2>&1; then
  echo "patchelf 0.18.0 or newer is required" >&2
  exit 1
fi
if [ ! -f "$original" ]; then
  echo "missing original liblhdc.so: $original" >&2
  exit 1
fi
if [ "$(sha256sum "$original" | awk '{print $1}')" != "$expected_original_sha" ]; then
  echo "unsupported liblhdc.so build" >&2
  exit 1
fi
if [ "$(patchelf --print-needed "$original" | grep -Fxc "$unused_dependency")" -ne 1 ]; then
  echo "expected legacy libstdc++ dependency is missing or duplicated" >&2
  exit 1
fi

mkdir -p "$(dirname "$output")"
cp -f "$original" "$output"
patchelf --remove-needed "$unused_dependency" "$output"

if patchelf --print-needed "$output" | grep -Fqx "$unused_dependency"; then
  echo "failed to remove the unused legacy libstdc++ dependency" >&2
  exit 1
fi

echo "Patched legacy LHDC core: $output"
sha256sum "$output"
