# Marble proprietary LHDC inputs

`liblhdc.so` and `liblhdcBT_enc.so` are the proprietary LHDC v3/v4 encoder
implementation from Xiaomi's marble Bluetooth APEX. No corresponding source is
published in AOSP. The ROM build keeps these two files as explicit device
inputs and applies the narrowly scoped linker fixes at build time; every other
Bluetooth Audio compatibility output is built from source or patched from the
current port ROM's Bluetooth APEX.

Source revision: marble Android 15 Bluetooth APEX from repository history
`2efb54c:marble_files/bluetooth.zip`.

- `liblhdc.so`: `3cdc73f296a56864e245dfca8385d2d4f9a21793d05336c754779802b984573a`
- `liblhdcBT_enc.so`: `bc19fc94d1ca129376989dc105f93c273cad376e7399177424ea59e341fc0cc4`
