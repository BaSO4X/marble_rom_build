/*
 * Android 17's A2DP_GetPacketTimestamp codec switch recognizes LHDCv5 but
 * omits the historical LHDC v2/v3 IDs. LHDCv4 is negotiated through the v3
 * codec ID. Accept exactly v2 or v3, then reuse the stock generic timestamp
 * path. Other unsupported codecs keep returning false.
 */

    .text
    .p2align 2
    .global timestamp_compat_start
    .type timestamp_compat_start, %function

    .extern timestamp_success
    .extern timestamp_failure

timestamp_compat_start:
    mov     x9, #0x3aff
    movk    x9, #0x3205, lsl #16
    movk    x9, #0x4c, lsl #32
    eor     x9, x8, x9
    /* codec_info[7] occupies bits 24..31 in the reconstructed key. */
    tst     x9, #0xfffffffffeffffff
    b.eq    timestamp_success
    b       timestamp_failure

    .size timestamp_compat_start, .-timestamp_compat_start
