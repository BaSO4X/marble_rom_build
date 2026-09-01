#!/bin/bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
usage='Usage: build_and_install.sh ANDROID_NDK CARGO SEVEN_ZIP DEVICE_ROOT SYSTEM_ROOT IMAGE_CONFIG_ROOT DEVICE_NAME'
android_ndk=${1:?$usage}
cargo=${2:?$usage}
seven_zip=${3:?$usage}
device_root=${4:?$usage}
system_root=${5:?$usage}
image_config_root=${6:?$usage}
device_name=${7:?$usage}

build_root=$(mktemp -d)
trap 'rm -rf -- "$build_root"' EXIT

bash "$script_dir/build_profile.sh" \
  "$android_ndk" "$cargo" "$seven_zip" \
  "$device_root" "$system_root" "$build_root"
bash "$script_dir/install.sh" \
  "$build_root/profile" "$device_root" "$system_root" \
  "$image_config_root" "$device_name"
