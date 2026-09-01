#!/bin/bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
bridge_dir=$(cd -- "$script_dir/.." && pwd)
usage='Usage: install.sh PROFILE_ROOT DEVICE_ROOT SYSTEM_ROOT IMAGE_CONFIG_ROOT DEVICE_NAME'
profile_root=${1:?$usage}
device_root=${2:?$usage}
system_root=${3:?$usage}
image_config_root=${4:?$usage}
device_name=${5:?$usage}
vendor_root="$device_root/vendor"
vendor_config_root="$device_root/config"

if [[ "$device_name" != marble ]]; then
  printf 'Bluetooth Audio marble integration does not support: %s\n' \
    "$device_name" >&2
  exit 1
fi
vendor_sdk=$(sed -n 's/^ro\.vendor\.build\.version\.sdk=//p' \
  "$vendor_root/build.prop" | tail -n 1)
if [[ "$vendor_sdk" != 32 ]]; then
  printf 'Bluetooth Audio integration requires vendor SDK 32, found: %s\n' \
    "${vendor_sdk:-missing}" >&2
  exit 1
fi

vintf_target="$vendor_root/etc/vintf/manifest/android.hardware.bluetooth.audio-service.marble.xml"
policy_output=$(mktemp "$vendor_root/etc/audio/sku_ukee_qssi/.audio_policy.XXXXXX")
trap 'rm -f -- "$policy_output"' EXIT

install -D -m 0755 \
  "$profile_root/vendor/bin/hw/android.hardware.bluetooth.audio-service.marble" \
  "$vendor_root/bin/hw/android.hardware.bluetooth.audio-service.marble"
install -D -m 0644 \
  "$profile_root/vendor/lib64/libbluetooth_audio_session.so" \
  "$vendor_root/lib64/libbluetooth_audio_session.so"
install -D -m 0644 \
  "$profile_root/vendor/lib64/hw/audio.bluetooth.default.so" \
  "$vendor_root/lib64/hw/audio.bluetooth.default.so"
for library in \
  libbluetooth_jni.marble.so \
  liblhdc.so \
  liblhdcBT_enc.so \
  liblhdcv5_native_patch.so; do
  install -D -m 0644 "$profile_root/system/lib64/$library" \
    "$system_root/lib64/$library"
done

install -D -m 0644 \
  "$bridge_dir/android.hardware.bluetooth.audio-service.marble.rc" \
  "$vendor_root/etc/init/android.hardware.bluetooth.audio-service.marble.rc"
install -D -m 0644 \
  "$bridge_dir/android.hardware.bluetooth.audio-service.marble.xml" \
  "$vintf_target"
install -D -m 0644 "$script_dir/bluetooth_audio_aidl_apex_patch.rc" \
  "$system_root/etc/init/bluetooth_audio_aidl_apex_patch.rc"

python3 "$script_dir/patch_vendor_sepolicy.py" \
  "$vendor_root/etc/selinux/vendor_sepolicy.cil" \
  "$script_dir/vendor.bluetooth-audio-aidl-bridge.cil" \
  "$vendor_root/etc/selinux" \
  "$device_root/odm/etc/selinux"

python3 "$bridge_dir/patch_audio_policy.py" \
  "$vendor_root/etc/audio/sku_ukee_qssi/audio_policy_configuration.xml" \
  "$policy_output"
install -m 0644 "$policy_output" \
  "$vendor_root/etc/audio/sku_ukee_qssi/audio_policy_configuration.xml"

set_build_property() {
  local file=$1
  local key=$2
  local value=$3
  if grep -q "^${key}=" "$file"; then
    sed -i "s|^${key}=.*|${key}=${value}|" "$file"
  else
    printf '\n%s=%s\n' "$key" "$value" >>"$file"
  fi
}

append_metadata() {
  local entry=$1
  local file=$2
  grep -qxF "$entry" "$file" || printf '%s\n' "$entry" >>"$file"
}

set_build_property "$vendor_root/build.prop" \
  ro.bluetooth.a2dp_offload.supported false

append_metadata '/vendor/bin/hw/android\.hardware\.bluetooth\.audio-service\.marble u:object_r:hal_audio_default_exec:s0' \
  "$vendor_config_root/vendor_file_contexts"
append_metadata 'vendor/bin/hw/android.hardware.bluetooth.audio-service.marble 0 2000 0755' \
  "$vendor_config_root/vendor_fs_config"
append_metadata '/vendor/etc/init/android\.hardware\.bluetooth\.audio-service\.marble\.rc u:object_r:vendor_configs_file:s0' \
  "$vendor_config_root/vendor_file_contexts"
append_metadata 'vendor/etc/init/android.hardware.bluetooth.audio-service.marble.rc 0 0 0644' \
  "$vendor_config_root/vendor_fs_config"
append_metadata '/vendor/etc/vintf/manifest/android\.hardware\.bluetooth\.audio-service\.marble\.xml u:object_r:vendor_configs_file:s0' \
  "$vendor_config_root/vendor_file_contexts"
append_metadata 'vendor/etc/vintf/manifest/android.hardware.bluetooth.audio-service.marble.xml 0 0 0644' \
  "$vendor_config_root/vendor_fs_config"
append_metadata '/vendor/lib64/libbluetooth_audio_session\.so u:object_r:vendor_file:s0' \
  "$vendor_config_root/vendor_file_contexts"
append_metadata 'vendor/lib64/libbluetooth_audio_session.so 0 0 0644' \
  "$vendor_config_root/vendor_fs_config"
append_metadata '/vendor/lib64/hw/audio\.bluetooth\.default\.so u:object_r:vendor_file:s0' \
  "$vendor_config_root/vendor_file_contexts"
append_metadata 'vendor/lib64/hw/audio.bluetooth.default.so 0 0 0644' \
  "$vendor_config_root/vendor_fs_config"

for library in \
  libbluetooth_jni.marble.so \
  liblhdc.so \
  liblhdcBT_enc.so \
  liblhdcv5_native_patch.so; do
  escaped=${library//./\\.}
  append_metadata "/system/system/lib64/$escaped u:object_r:system_lib_file:s0" \
    "$image_config_root/system_file_contexts"
  append_metadata "system/lib64/$library 0 0 0644" \
    "$image_config_root/system_fs_config"
done
append_metadata '/system/system/etc/init/bluetooth_audio_aidl_apex_patch\.rc u:object_r:system_file:s0' \
  "$image_config_root/system_file_contexts"
append_metadata 'system/etc/init/bluetooth_audio_aidl_apex_patch.rc 0 0 0644' \
  "$image_config_root/system_fs_config"

printf '%s\n' 'Bluetooth Audio AIDL ROM integration installed'
