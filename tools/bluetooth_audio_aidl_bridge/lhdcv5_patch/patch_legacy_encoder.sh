#!/bin/sh
set -eu

usage="Usage: patch_legacy_encoder.sh ORIGINAL_LIBLHDCBT_ENC OUTPUT_LIBLHDCBT_ENC"
original=${1:?$usage}
output=${2:?$usage}
expected_original_sha=bc19fc94d1ca129376989dc105f93c273cad376e7399177424ea59e341fc0cc4
absolute_core=/system/lib64/liblhdc.so

if ! command -v patchelf >/dev/null 2>&1; then
  echo "patchelf 0.18.0 or newer is required" >&2
  exit 1
fi
if [ ! -f "$original" ]; then
  echo "missing original liblhdcBT_enc.so: $original" >&2
  exit 1
fi
if [ "$(sha256sum "$original" | awk '{print $1}')" != "$expected_original_sha" ]; then
  echo "unsupported liblhdcBT_enc.so build" >&2
  exit 1
fi

mkdir -p "$(dirname "$output")"
cp -f "$original" "$output"
patchelf --replace-needed liblhdc.so "$absolute_core" "$output"

if [ "$(patchelf --print-needed "$output" | grep -Fxc "$absolute_core")" -ne 1 ] ||
    patchelf --print-needed "$output" | grep -Fqx liblhdc.so; then
  echo "failed to replace the legacy encoder core dependency" >&2
  exit 1
fi

echo "Patched legacy LHDC encoder: $output"
sha256sum "$output"
