cd $(dirname $0)
# Java环境变量
export JAVA_HOME="JAVA_HOME=/usr/lib/jvm/java-21-openjdk-amd64/"
export PATH="$JAVA_HOME/bin:$PATH"
export LD_LIBRARY_PATH="$JAVA_HOME/lib:$LD_LIBRARY_PATH"

echo "By AetherPX"
echo "Credits:"
echo "KameYuki"
echo "相见即是缘"

echo
if ! command -v java &> /dev/null; then
    echo "未检测到Java环境! 请检查路径配置"
    exit 1
fi

errors=0

DEX_NAME="classes3.dex"
baksmali=$(find ./ -name "baksmali*.jar" -type f)
smali=$(find ./ -name "smali*.jar" -type f)

MISSING_JAR=()
[[ -z "$baksmali" ]] && MISSING_JAR+=("baksmali*.jar")
[[ -z "$smali" ]] && MISSING_JAR+=("smali*.jar")

if [[ ${#MISSING_JAR[@]} -gt 0 ]]; then
    echo "错误: 未找到以下文件:"
    for file in "${MISSING_JAR[@]}"; do
        echo "- $file"
    done
    exit 1
fi

echo "反编译 $DEX_NAME"
java -jar $baksmali d ./$DEX_NAME -o temp_smali


MANAGER=$(find temp_smali -name "MiFlashlightManager.smali" -type f)
RECEIVER=$(find temp_smali -name "MiFlashlightOnSystemUiReceiver.smali" -type f)

MISSING_FILES=""
[[ -z "$MANAGER" ]] && MISSING_FILES+="MiFlashlightManager.smali "
[[ -z "$RECEIVER" ]] && MISSING_FILES+="MiFlashlightOnSystemUiReceiver.smali "

if [[ -n "$MISSING_FILES" ]]; then
    echo "错误: 未找到以下文件:"
    for file in $MISSING_FILES; do
        echo "- $file"
    done
    exit 1
fi

methods_missing=0
! grep -q "\.method.*setMaxStrength" "$MANAGER" && echo "错误：未找到setMaxStrength方法！" && methods_missing=1
! grep -q "\.method.*setCurStrength" "$MANAGER" && echo "错误：未找到setCurStrength方法！" && methods_missing=1
! grep -q "\.method.*operateCamera" "$RECEIVER" && echo "错误：未找到operateCamera方法！" && methods_missing=1
[ $methods_missing -eq 1 ] && exit 1

echo "修改setMaxStrength方法..."
awk '
/\.method.*setMaxStrength/ {
    print $0
    getline
    print $0
    print "    const/16 p1, 0x64"
    next
}
{ print }
' "$MANAGER" > tmpfile && mv tmpfile "$MANAGER"

echo "修改setCurStrength方法..."
awk '
/.method.*setCurStrength/ {
    print $0
    getline
    print $0
    print "    const/16 p1, 0x64"
    next
}
{ print }
' "$MANAGER" > tmp_setCurStrength && mv tmp_setCurStrength "$MANAGER"

echo "添加operateBright方法..."
cat >> "$MANAGER" << 'EOF'

.method public operateBright()V
.registers 7
invoke-virtual {p0}, Lmiui/systemui/flashlight/MiFlashlightManager;->getLogicStrength()F
move-result v0
const/16 v1, 0x82 # 最大值
int-to-float v1, v1
mul-float/2addr v1, v0
float-to-int v0, v1
invoke-static {v0}, Lmiui/systemui/flashlight/MiFlashlightManager;->saveStrengthToPath(I)V
return-void
.end method
EOF

echo "添加saveStrengthToPath方法..."
cat >> "$MANAGER" << 'EOF'

.method public static saveStrengthToPath(I)V
.registers 5
const/16 v0, 0x82 # 最大值
const/16 v1, 0xd # 最小值
move v2, v0
if-lez p0, :cond_a
if-gt p0, v1, :cond_a
move p0, v1
:cond_a
if-le p0, v2, :cond_d
move p0, v2
:cond_d
:try_start_d
new-instance v0, Ljava/io/FileWriter;
const-string v1, "/sys/class/leds/led:torch_0/brightness"
invoke-direct {v0, v1}, Ljava/io/FileWriter;-><init>(Ljava/lang/String;)V
invoke-static {p0}, Ljava/lang/String;->valueOf(I)Ljava/lang/String;
move-result-object v1
invoke-virtual {v0, v1}, Ljava/io/FileWriter;->write(Ljava/lang/String;)V
invoke-virtual {v0}, Ljava/io/FileWriter;->flush()V
invoke-virtual {v0}, Ljava/io/FileWriter;->close()V
new-instance v0, Ljava/lang/StringBuilder;
invoke-direct {v0}, Ljava/lang/StringBuilder;-><init>()V
const-string v1, "Wrote strength "
invoke-virtual {v0, v1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;
invoke-virtual {v0, p0}, Ljava/lang/StringBuilder;->append(I)Ljava/lang/StringBuilder;
const-string v1, " to flashlight"
invoke-virtual {v0, v1}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;
invoke-virtual {v0}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;
move-result-object v0
const-string v1, "MiFlash_MiFlashlightManager"
invoke-static {v1, v0}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I
:try_end_3c
.catch Ljava/io/IOException; {:try_start_d .. :try_end_3c} :catch_3d
goto :goto_45
:catch_3d
move-exception v0
const-string v1, "MiFlash_MiFlashlightManager"
const-string v2, "Failed to write to brightness nodes"
invoke-static {v1, v2, v0}, Landroid/util/Log;->e(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Throwable;)I
:goto_45
return-void
.end method
EOF

echo "修改operateCamera方法..."
awk '
BEGIN { in_method=0; skip=0 }
/\.method.*operateCamera/ { in_method=1 }
in_method {
    if (skip) {
        if (/^[[:space:]]*$/) {
            print
            skip=1
            next
        }
        skip=0
        next
    }
    if (/turnOnTorchWithStrengthLevel/) {
        skip=1
        next
    }
}
{ print }
/\.end method/ && in_method { in_method=0 }
' "$RECEIVER" > tmp_receiver && mv tmp_receiver "$RECEIVER"

awk '
BEGIN { 
    in_method=0
    last_set_line=0
}
/\.method.*operateCamera/ { in_method=1 }
in_method && /setTorchMode/ {
    last_set_line=NR
}
{
    lines[NR] = $0
}
/\.end method/ && in_method { in_method=0 }
END {
    for (i=1; i<=NR; i++) {
        print lines[i]
        if (i == last_set_line) {
            print "    int-to-float v2, v1"
            print "    const v4, 0x3fa66666  # 最大值"
            print "    mul-float/2addr v2, v4"
            print "    float-to-int v2, v2"
            print "    invoke-static {v2}, Lmiui/systemui/flashlight/MiFlashlightManager;->saveStrengthToPath(I)V"
        }
    }
}
' "$RECEIVER" > tmp_receiver && mv tmp_receiver "$RECEIVER"

echo "smali修改完成!"

echo "重新编译为dex"
java -jar $smali a temp_smali -o $DEX_NAME

rm -rf temp_smali

echo "完成！"