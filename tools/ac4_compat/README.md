# Marble AC-4 OMX compatibility

Android 15 and newer no longer provide the Xiaomi/Dolby private initialization
used by the vendor SDK 32 `OMX.dolby.ac4.decoder`. This tool preserves the
verified vendor decoder as `libstagefright_soft_ac4src.so`, builds a 32-bit OMX
wrapper at the original library path and enables the existing AC-4 codec block
without replacing unrelated vendor XML.

The build is fail-closed. `profiles.json` binds the source and renamed decoder
hashes, SONAME bytes, vendor/runtime SDK range, private ABI, table ID and exact
wrapper hash. Unknown blobs, duplicate profiles, unexpected XML structure or a
different wrapper are rejected. The current profile supports runtime API 35+
without an artificial upper bound, but remains specific to marble's verified
32-bit decoder ABI.

`make.sh` calls:

```bash
tools/ac4_compat/build.sh ANDROID_NDK UNPACKED_VENDOR_ROOT RUNTIME_SDK DEVICE
```

The build requires Android NDK r27d and Python 3. It produces no repository
artifact: the temporary wrapper is compiled outside the tree and installed
directly into the unpacked vendor image. Run the host-side protection tests
with:

```bash
python3 -m unittest tools/ac4_compat/test_apply.py
```

Adding a decoder hash is safe only after confirming the same factory ABI,
private OMX index, table layout and contents, followed by decode/EOS/lifecycle,
crash and listening regression tests. A different private ABI requires a new
wrapper implementation rather than another profile row.
