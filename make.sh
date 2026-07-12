#!/bin/bash

URL="$1"              # 移植包下载地址
VENDOR_URL="$2"       # 底包下载地址
GITHUB_ENV="$3"       # 输出环境变量
GITHUB_WORKSPACE="$4" # 工作目录

Red='\033[1;31m'    # 粗体红色
Yellow='\033[1;33m' # 粗体黄色
Blue='\033[1;34m'   # 粗体蓝色
Green='\033[1;32m'  # 粗体绿色

device=marble # 设备代号

# 移植包 OS 版本号
port_os_version=$(echo "$URL" | awk -F'/' '{print $(NF-1)}')
# 移植包 zip 名称
port_zip_name=$(echo "$URL" | awk -F'/' '{print $NF}' | awk -F'?' '{print $1}')
# 底包 OS 版本号
vendor_os_version=$(echo "$VENDOR_URL" | awk -F'/' '{print $(NF-1)}')
# 底包 zip 名称
vendor_zip_name=$(echo "$VENDOR_URL" | awk -F'/' '{print $NF}' | awk -F'?' '{print $1}')
# Android 版本号
android_version=$(echo "$URL" | grep -oE '-user-[0-9]+' | grep -oE '[0-9]+')
build_time=$(date) && build_utc=$(date -d "$build_time" +%s)   # 构建时间

magiskboot="$GITHUB_WORKSPACE"/tools/magiskboot
a7z="$GITHUB_WORKSPACE"/tools/7zzs
ksud="$GITHUB_WORKSPACE"/tools/lkm_patch/ksud
payload_extract="$GITHUB_WORKSPACE"/tools/payload_extract
mke2fs="$GITHUB_WORKSPACE"/tools/mke2fs
e2fsdroid="$GITHUB_WORKSPACE"/tools/e2fsdroid
erofs_extract="$GITHUB_WORKSPACE"/tools/extract.erofs
erofs_mkfs="$GITHUB_WORKSPACE"/tools/mkfs.erofs
lpmake="$GITHUB_WORKSPACE"/tools/lpmake

mkdir -p "$GITHUB_WORKSPACE"/tools
mkdir -p "$GITHUB_WORKSPACE"/firmware
mkdir -p "$GITHUB_WORKSPACE"/files

chmod -R 755 "$GITHUB_WORKSPACE"/tools
chmod -R 755 "$GITHUB_WORKSPACE"/firmware
chmod -R 755 "$GITHUB_WORKSPACE"/files


Start_Time() {
  Start_s=$(date +%s)
  Start_ns=$(date +%N)
}

End_Time() {
  local End_s End_ns time_s time_ns
  End_s=$(date +%s)
  End_ns=$(date +%N)
  time_s=$((10#$End_s - 10#$Start_s))
  time_ns=$((10#$End_ns - 10#$Start_ns))
  if ((time_ns < 0)); then
    ((time_s--))
    ((time_ns += 1000000000))
  fi
 
  local ns ms sec min hour
  ns=$((time_ns % 1000000))
  ms=$((time_ns / 1000000))
  sec=$((time_s % 60))
  min=$((time_s / 60 % 60))
  hour=$((time_s / 3600))

  if ((hour > 0)); then
    echo -e "${Green}- 本次$1用时: ${Blue}$hour小时$min分$sec秒$ms毫秒"
  elif ((min > 0)); then
    echo -e "${Green}- 本次$1用时: ${Blue}$min分$sec秒$ms毫秒"
  elif ((sec > 0)); then
    echo -e "${Green}- 本次$1用时: ${Blue}$sec秒$ms毫秒"
  elif ((ms > 0)); then
    echo -e "${Green}- 本次$1用时: ${Blue}$ms毫秒"
  else
    echo -e "${Green}- 本次$1用时: ${Blue}$ns纳秒"
  fi
}

### 系统包下载
echo -e "${Red}- 开始下载系统包"
Start_Time
aria2c -x16 -j$(nproc) -U "Mozilla/5.0" -d "$GITHUB_WORKSPACE" ${VENDOR_URL} &
aria2c -x16 -j$(nproc) -U "Mozilla/5.0" -d "$GITHUB_WORKSPACE" ${URL} &
wait
End_Time 下载系统包
### 系统包下载结束

### 解包
echo -e "${Red}- 开始解压系统包"
mkdir -p "$GITHUB_WORKSPACE"/vendor_zip
mkdir -p "$GITHUB_WORKSPACE"/port_zip
mkdir -p "$GITHUB_WORKSPACE"/images/config
mkdir -p "$GITHUB_WORKSPACE"/super
mkdir -p "$GITHUB_WORKSPACE"/Extra_dir
mkdir -p "$GITHUB_WORKSPACE"/zip

echo -e "${Yellow}- 开始解压底包"
Start_Time
$a7z x "$GITHUB_WORKSPACE"/${vendor_zip_name} -o"$GITHUB_WORKSPACE"/vendor_zip payload.bin >/dev/null
rm -rf "$GITHUB_WORKSPACE"/${vendor_zip_name}
End_Time 解压底包

echo -e "${Red}- 开始解底包 Payload"
$payload_extract -s -o "$GITHUB_WORKSPACE"/firmware/images -i "$GITHUB_WORKSPACE"/vendor_zip/payload.bin -X abl,aop,aop_config,bluetooth,boot,cpucp,devcfg,dsp,dtbo,featenabler,hyp,keymaster,modem,qupfw,shrm,tz,uefi,uefisecapp,vendor_boot,xbl,xbl_config,xbl_ramdump,vbmeta,vbmeta_system -T0
$payload_extract -s -o "$GITHUB_WORKSPACE"/Extra_dir -i "$GITHUB_WORKSPACE"/vendor_zip/payload.bin -X vendor,odm,vendor_dlkm -T0
sudo rm -rf "$GITHUB_WORKSPACE"/vendor_zip/payload.bin

echo -e "${Yellow}- 开始解压移植包"
Start_Time
$a7z x "$GITHUB_WORKSPACE"/${port_zip_name} -o"$GITHUB_WORKSPACE"/port_zip payload.bin >/dev/null
rm -rf "$GITHUB_WORKSPACE"/${port_zip_name}
End_Time 解压移植包

echo -e "${Red}- 开始解移植包 Payload"
$payload_extract -s -o "$GITHUB_WORKSPACE"/Extra_dir -i "$GITHUB_WORKSPACE"/port_zip/payload.bin -X mi_ext,product,system,system_ext -T0
sudo rm -rf "$GITHUB_WORKSPACE"/port_zip/payload.bin

echo -e "${Red}- 开始分解Images"
for i in system_ext vendor mi_ext system product odm vendor_dlkm; do
  echo -e "${Yellow}- 正在分解底包: $i.img"
  cd "$GITHUB_WORKSPACE"/images
  sudo $erofs_extract -i "$GITHUB_WORKSPACE"/Extra_dir/$i.img -x -s
  rm -rf "$GITHUB_WORKSPACE"/Extra_dir/$i.img
done
echo -e "${Red}- 下载recovery.img"
curl -s https://api.github.com/repos/AviderMin/ofrp_device_xiaomi_marble/releases/latest | grep -o 'https://[^"]*\.img' | xargs -I {} aria2c -x16 -s16 -o recovery.img {} -d "${GITHUB_WORKSPACE}/firmware/images"
# 去除 AVB2.0 校验
echo -e "${Red}- 去除 AVB2.0 校验"
"$GITHUB_WORKSPACE"/tools/vbmeta-disable-verification "$GITHUB_WORKSPACE"/firmware/images/vbmeta.img
"$GITHUB_WORKSPACE"/tools/vbmeta-disable-verification "$GITHUB_WORKSPACE"/firmware/images/vbmeta_system.img
### 解包结束

### 写入变量
echo -e "${Red}- 开始写入变量"
# 构建日期
echo "build_time=$build_time" >>$GITHUB_ENV
echo -e "${Blue}- 构建日期: $build_time"
# 移植包机型信息
model=$(echo "$port_zip_name" | cut -d'-' -f1)
echo "model=$model" >> $GITHUB_ENV
echo -e "${Blue}- 移植包机型: $model"
# 移植包版本
echo "port_os_version=$port_os_version" >>$GITHUB_ENV
echo -e "${Blue}- 移植包版本: $port_os_version"
# 底包版本
echo "vendor_os_version=$vendor_os_version" >>$GITHUB_ENV
# 移植包安全补丁
system_build_prop=$(find "$GITHUB_WORKSPACE"/images/system/system/ -maxdepth 1 -type f -name "build.prop" | head -n 1)
port_security_patch=$(grep "ro.build.version.security_patch=" "$system_build_prop" | awk -F "=" '{print $2}')
echo -e "${Blue}- 移植包安全补丁版本: $port_security_patch"
echo "port_security_patch=$port_security_patch" >>$GITHUB_ENV
# 底包安全补丁
vendor_build_prop=$GITHUB_WORKSPACE/images/vendor/build.prop
vendor_security_patch=$(grep "ro.vendor.build.security_patch=" "$vendor_build_prop" | awk -F "=" '{print $2}')
echo -e "${Blue}- 底包安全补丁版本: $vendor_security_patch"
echo "vendor_security_patch=$vendor_security_patch" >>$GITHUB_ENV
# 移植包基线版本
port_base_line=$(grep "ro.system.build.id=" "$system_build_prop" | awk -F "=" '{print $2}')
echo -e "${Blue}- 移植包基线版本: $port_base_line"
echo "port_base_line=$port_base_line" >>$GITHUB_ENV
# 底包vendor基线版本
vendor_base_line=$(grep "ro.vendor.build.id=" "$vendor_build_prop" | awk -F "=" '{print $2}')
echo -e "${Blue}- 底包vendor基线版本: $vendor_base_line"
echo "vendor_base_line=$vendor_base_line" >>$GITHUB_ENV
# 增量版本号
mi_ext_build_prop=$GITHUB_WORKSPACE/images/mi_ext/etc/build.prop
incremental_version=$(grep "ro.mi.xms.version.incremental=" "$mi_ext_build_prop" | awk -F "=" '{print $2}')
echo -e "${Blue}- 增量版本号: $incremental_version"
echo "incremental_version=$incremental_version" >>$GITHUB_ENV
### 写入变量结束

### 功能修复
echo -e "${Red}- 开始功能修复"
Start_Time
echo "精简apk"
rm -rf "$GITHUB_WORKSPACE"/images/product/app/AnalyticsCore
rm -rf "$GITHUB_WORKSPACE"/images/product/app/BSGameCenter
rm -rf "$GITHUB_WORKSPACE"/images/product/app/HybridPlatform
rm -rf "$GITHUB_WORKSPACE"/images/product/app/MiTrustService
rm -rf "$GITHUB_WORKSPACE"/images/product/app/subscreencenter
rm -rf "$GITHUB_WORKSPACE"/images/product/app/Updater
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/OS2VipAccount
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/SmartHome
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/BaiduIME
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/Health
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/iFlytekIME
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/MIGalleryLockscreen
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/MIpay
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/MiShop
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/MIUIDuokanReader
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/MIUIEmail
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/MIUIGameCenter
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/MIUIHuanji
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/MIUIMusicT
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/MIUINewHome_Removable
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/MIUIVideo
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/MIUIVirtualSim
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/MIUIYoupin
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/MiRadio
rm -rf "$GITHUB_WORKSPACE"/images/product/data-app/TinyGame
rm -rf "$GITHUB_WORKSPACE"/images/product/priv-app/MiniGameService
rm -rf "$GITHUB_WORKSPACE"/images/product/priv-app/MIUIBrowser
rm -rf "$GITHUB_WORKSPACE"/images/product/priv-app/AIService
rm -rf "$GITHUB_WORKSPACE"/images/product/priv-app/MiuiCamera
rm -rf "$GITHUB_WORKSPACE"/images/product/pangu/system/app/Nfc_st
rm -rf "$GITHUB_WORKSPACE"/images/mi_ext/product/ai/taiyi
echo "精简apk完成"
echo "当前机型为 $model"
echo "正在复制文件..."
mkdir -p "$GITHUB_WORKSPACE"/images
\cp -rf "$GITHUB_WORKSPACE"/files/common/* "$GITHUB_WORKSPACE"/images/
echo "处理build.prop"
cat "$GITHUB_WORKSPACE"/files/build.prop >> "$GITHUB_WORKSPACE"/images/mi_ext/etc/build.prop
curl -s https://api.github.com/repos/BaSO4X/Backup/releases/tags/backup | grep -o 'https://[^"]*com\.android\.vndk\.v30\.apex' | xargs -I {} aria2c -x16 -s16 -o com.android.vndk.v30.apex {} -d "${GITHUB_WORKSPACE}/images/system_ext/apex"
curl -s https://api.github.com/repos/BaSO4X/Backup/releases/tags/backup | grep -o 'https://[^"]*MiuiCamera\.apk' | xargs -I {} aria2c -x16 -s16 -o MiuiCamera.apk {} -d "${GITHUB_WORKSPACE}/images/product/priv-app/MiuiCamera"
echo "开始更换GPU驱动"
mkdir -p "$GITHUB_WORKSPACE"/images
\cp -rf "$GITHUB_WORKSPACE"/files/gpu_drivers/* "$GITHUB_WORKSPACE"/images/
echo "/vendor/lib/libllvm-qgl\.so u:object_r:same_process_hal_file:s0" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_file_contexts
echo "vendor/lib/libllvm-qgl.so 0 0 0644" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_fs_config
echo "/vendor/lib64/libllvm-qgl\.so u:object_r:same_process_hal_file:s0" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_file_contexts
echo "vendor/lib64/libllvm-qgl.so 0 0 0644" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_fs_config
echo "/vendor/lib/libdmabufheap\.so u:object_r:same_process_hal_file:s0" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_file_contexts
echo "vendor/lib/libdmabufheap.so 0 0 0644" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_fs_config
echo "/vendor/lib64/libdmabufheap\.so u:object_r:same_process_hal_file:s0" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_file_contexts
echo "vendor/lib64/libdmabufheap.so 0 0 0644" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_fs_config
echo "vendor/lib/egl/libVkLayer_ADRENO_qprofiler.so 0 0 0644" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_fs_config
echo "/vendor/lib/egl/libVkLayer_ADRENO_qprofiler\.so u:object_r:system_lib_file:s0" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_file_contexts
echo "vendor/lib64/egl/libVkLayer_ADRENO_qprofiler.so 0 0 0644" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_fs_config
echo "/vendor/lib64/egl/libVkLayer_ADRENO_qprofiler\.so u:object_r:system_lib_file:s0" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_file_contexts
echo "vendor/firmware/a650_sqe.fw 0 0 0644" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_fs_config
echo "/vendor/firmware/a650_sqe\.fw u:object_r:system_file:s0" | sudo tee -a "$GITHUB_WORKSPACE"/images/config/vendor_file_contexts
End_Time 功能修复
### 功能修复结束

### 生成 super.img
echo -e "${Red}- 开始打包super.img"
Start_Time
partitions=("mi_ext" "product" "system" "system_ext" "vendor" "odm" "vendor_dlkm")
  for partition in "${partitions[@]}"; do
    echo -e "${Red}- 正在生成: $partition"
    sudo python3 "$GITHUB_WORKSPACE"/tools/fspatch.py "$GITHUB_WORKSPACE"/images/$partition "$GITHUB_WORKSPACE"/images/config/"$partition"_fs_config
    sudo python3 "$GITHUB_WORKSPACE"/tools/contextpatch.py "$GITHUB_WORKSPACE"/images/$partition "$GITHUB_WORKSPACE"/images/config/"$partition"_file_contexts None
    Start_Time
    sudo $erofs_mkfs --quiet -zlz4hc,9 -T 1230768000 --mount-point /$partition --fs-config-file "$GITHUB_WORKSPACE"/images/config/"$partition"_fs_config --file-contexts "$GITHUB_WORKSPACE"/images/config/"$partition"_file_contexts "$GITHUB_WORKSPACE"/super/$partition.img "$GITHUB_WORKSPACE"/images/$partition
    End_Time 打包erofs
    eval "$partition"_size=$(du -sb "$GITHUB_WORKSPACE"/super/$partition.img | awk {'print $1'})
    sudo rm -rf "$GITHUB_WORKSPACE"/images/$partition
  done
  sudo rm -rf "$GITHUB_WORKSPACE"/images/config
  $lpmake --metadata-size 65536 --super-name super --block-size 4096 \
  --partition mi_ext_a:readonly:"$mi_ext_size":qti_dynamic_partitions_a \
  --image mi_ext_a="$GITHUB_WORKSPACE"/super/mi_ext.img \
  --partition mi_ext_b:readonly:0:qti_dynamic_partitions_b \
  --partition odm_a:readonly:"$odm_size":qti_dynamic_partitions_a \
  --image odm_a="$GITHUB_WORKSPACE"/super/odm.img \
  --partition odm_b:readonly:0:qti_dynamic_partitions_b \
  --partition product_a:readonly:"$product_size":qti_dynamic_partitions_a \
  --image product_a="$GITHUB_WORKSPACE"/super/product.img \
  --partition product_b:readonly:0:qti_dynamic_partitions_b \
  --partition system_a:readonly:"$system_size":qti_dynamic_partitions_a \
  --image system_a="$GITHUB_WORKSPACE"/super/system.img \
  --partition system_b:readonly:0:qti_dynamic_partitions_b \
  --partition system_ext_a:readonly:"$system_ext_size":qti_dynamic_partitions_a \
  --image system_ext_a="$GITHUB_WORKSPACE"/super/system_ext.img \
  --partition system_ext_b:readonly:0:qti_dynamic_partitions_b \
  --partition vendor_a:readonly:"$vendor_size":qti_dynamic_partitions_a \
  --image vendor_a="$GITHUB_WORKSPACE"/super/vendor.img \
  --partition vendor_b:readonly:0:qti_dynamic_partitions_b \
  --partition vendor_dlkm_a:readonly:"$vendor_dlkm_size":qti_dynamic_partitions_a \
  --image vendor_dlkm_a="$GITHUB_WORKSPACE"/super/vendor_dlkm.img \
  --partition vendor_dlkm_b:readonly:0:qti_dynamic_partitions_b \
  --device super:9126805504 \
  --metadata-slots 3 \
  --group qti_dynamic_partitions_a:9126805504 \
  --group qti_dynamic_partitions_b:9126805504 \
  --virtual-ab -F \
  --output "$GITHUB_WORKSPACE"/super/super.img
  End_Time 打包super
  for partition in "${partitions[@]}"; do
    rm -rf "$GITHUB_WORKSPACE"/super/$partition.img
  done
### 生成 super.img 结束

### 输出刷机包
echo -e "${Red}- 开始生成刷机包"
echo -e "${Red}- 开始压缩super"
Start_Time
sudo find "$GITHUB_WORKSPACE"/super/ -exec touch -t 200901010000.00 {} \;
zstd -3 -f "$GITHUB_WORKSPACE"/super/super.img -o "$GITHUB_WORKSPACE"/firmware/super.img.zst
rm -f "$GITHUB_WORKSPACE"/super/super.img
End_Time 压缩super
# 生成刷机包
echo -e "${Red}- 生成刷机包"
Start_Time
cd "$GITHUB_WORKSPACE"/firmware
zip -r -1 "$GITHUB_WORKSPACE"/zip/marble_HyperT-${port_os_version}-BaSO4.zip $(ls | grep -v '^super.img.zst$')
zip -0 -u "$GITHUB_WORKSPACE"/zip/marble_HyperT-${port_os_version}-BaSO4.zip super.img.zst
sudo rm -rf "$GITHUB_WORKSPACE"/images
End_Time 压缩卡刷包
# 定制 ROM 包名
echo -e "${Red}- 定制 ROM 包名"
md5=$(md5sum "$GITHUB_WORKSPACE"/zip/marble_HyperT-${port_os_version}-BaSO4.zip)
echo "MD5=${md5:0:32}" >>$GITHUB_ENV
zip_md5=${md5:0:10}
rom_name="marble_HyperT-${port_os_version}-BaSO4-${zip_md5}.zip"
sudo mv "$GITHUB_WORKSPACE"/zip/marble_HyperT-${port_os_version}-BaSO4.zip "$GITHUB_WORKSPACE"/zip/"${rom_name}"
echo "rom_name=$rom_name" >>$GITHUB_ENV
### 输出刷机包结束
