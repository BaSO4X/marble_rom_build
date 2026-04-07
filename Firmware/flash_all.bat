fastboot getvar product 2>&1 | findstr /r /c:"^product: *marble" || echo Missmatching image and device
fastboot getvar product 2>&1 | findstr /r /c:"^product: *marble" || exit /B 1

zstd -k -d %~dp0/super.img.zst -o %~dp0images/super.img
fastboot flash super %~dp0images/super.img || @echo "Flash super error" && exit 1
fastboot flash cust %~dp0images/cust.img || @echo "Flash cust error" && exit 1

fastboot erase boot_ab || @echo "Erase boot error" && exit /B 1
fastboot flash abl_ab %~dp0images/abl.img || @echo "Flash abl error" && exit 1
fastboot flash xbl_ab %~dp0images/xbl.img || @echo "Flash xbl error" && exit 1
fastboot flash xbl_config_ab %~dp0images/xbl_config.img || @echo "Flash xbl_config error" && exit 1
fastboot flash shrm_ab %~dp0images/shrm.img || @echo "Flash shrm error" && exit 1
fastboot flash aop_ab %~dp0images/aop.img || @echo "Flash aop error" && exit 1
fastboot flash aop_config_ab %~dp0images/aop_config.img || @echo "Flash aop_config error" && exit 1
fastboot flash tz_ab %~dp0images/tz.img || @echo "Flash tz error" && exit 1
fastboot flash devcfg_ab %~dp0images/devcfg.img || @echo "Flash devcfg error" && exit 1
fastboot flash featenabler_ab %~dp0images/featenabler.img || @echo "Flash featenabler error" && exit 1
fastboot flash hyp_ab %~dp0images/hyp.img || @echo "Flash hyp error" && exit 1
fastboot flash uefi_ab %~dp0images/uefi.img || @echo "Flash uefi error" && exit 1
fastboot flash uefisecapp_ab %~dp0images/uefisecapp.img || @echo "Flash uefisecapp error" && exit 1
fastboot flash modem_ab %~dp0images/modem.img || @echo "Flash modem error" && exit 1
fastboot flash bluetooth_ab %~dp0images/bluetooth.img || @echo "Flash bluetooth error" && exit 1
fastboot flash dsp_ab %~dp0images/dsp.img || @echo "Flash dsp error" && exit 1
fastboot flash keymaster_ab %~dp0images/keymaster.img || @echo "Flash keymaster error" && exit 1
fastboot flash qupfw_ab %~dp0images/qupfw.img || @echo "Flash qupfw error" && exit 1
fastboot flash cpucp_ab %~dp0images/cpucp.img || @echo "Flash cpucp error" && exit 1
fastboot flash xbl_ramdump_ab %~dp0images/xbl_ramdump.img || @echo "Flash xbl_ramdump error" && exit 1
fastboot erase imagefv_ab || @echo "Erase imagefv error" && exit /B 1
fastboot flash imagefv_ab %~dp0images/imagefv.img || @echo "Flash imagefv error" && exit 1
fastboot flash vendor_boot_ab %~dp0images/vendor_boot.img || @echo "Flash vendor_boot error" && exit 1
fastboot flash dtbo_ab %~dp0images/dtbo.img || @echo "Flash dtbo error" && exit 1
fastboot flash vbmeta_ab %~dp0images/vbmeta.img || @echo "Flash vbmeta error" && exit 1
fastboot flash vbmeta_system_ab %~dp0images/vbmeta_system.img || @echo "Flash vbmeta_system error" && exit 1
fastboot flash recovery_ab %~dp0images/recovery.img || @echo "Flash recovery error" && exit 1
fastboot flash boot_ab %~dp0images/boot.img || @echo "Flash boot error" && exit 1

fastboot erase metadata || @echo "Erase metadata error" && exit 1
fastboot flash metadata %~dp0images/metadata.img || @echo "Flash metadata error" && exit 1
fastboot flash userdata %~dp0images/userdata.img || @echo "Flash userdata error" && exit 1
fastboot set_active a || @echo "Set active a error" && exit 1
fastboot reboot || @echo "Reboot error" && exit 1

cd %~dp0images/
del super.img