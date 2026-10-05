#ifndef HIDL_GENERATED_VENDOR_QTI_HARDWARE_BLUETOOTH_AUDIO_V2_0_TYPES_H
#define HIDL_GENERATED_VENDOR_QTI_HARDWARE_BLUETOOTH_AUDIO_V2_0_TYPES_H

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace bluetooth_audio {
namespace V2_0 {

// Forward declaration for forward reference support:
struct TimeSpec;
enum class Status : uint8_t;
enum class SessionType : uint8_t;
enum class CodecType : uint32_t;
enum class SampleRate : uint32_t;
enum class BitsPerSample : uint8_t;
enum class ChannelMode : uint8_t;
enum class SbcChannelMode : uint8_t;
enum class SbcBlockLength : uint8_t;
enum class SbcNumSubbands : uint8_t;
enum class SbcAllocMethod : uint8_t;
enum class AacObjectType : uint8_t;
enum class AacVariableBitRate : uint8_t;
enum class AptxAdaptiveChannelMode : int32_t;
enum class AptxMode : uint32_t;
enum class InputMode : uint32_t;
struct AptxSinkBuffering;
struct AptxAdaptive_TTP;
enum class LdacChannelMode : uint8_t;
enum class LdacQualityIndex : uint8_t;
enum class SessionParamType : uint8_t;
struct PcmParameters;
struct SbcParameters;
struct AacParameters;
struct LdacParameters;
struct AptxParameters;
struct AptxAdaptiveParameters;
struct AptxTwsParameters;
struct BaCeltParameters;
struct CodecCapabilities;
struct SinkLatency;
struct SessionParams;
union AudioCapabilities;
struct CodecConfiguration;
union AudioConfiguration;

/**
 * The different audio parameter structs are used to provide a method to list
 * all the Capabilities of a codec as well as to configure the codecs. All
 * fields are bitfields unless specified. If used as a configuration, only one
 * bit may be enabled. If used for Capabilities, enable all bits corresponding to
 * supported features.
 *
 *
 * POSIX timespec.
 */
struct TimeSpec final {
    uint64_t tvSec __attribute__ ((aligned(8)));
    uint64_t tvNSec __attribute__ ((aligned(8)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec, tvSec) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec, tvNSec) == 8, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec) == 16, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec) == 8, "wrong alignment");

enum class Status : uint8_t {
    SUCCESS = 0 /* 0x00 */,
    /**
     * Codec configuration not supported by the audio platform
     */
    UNSUPPORTED_CODEC_CONFIGURATION = 1 /* ::vendor::qti::hardware::bluetooth_audio::V2_0::Status.SUCCESS implicitly + 1 */,
    /**
     * Any other failure
     */
    FAILURE = 2 /* ::vendor::qti::hardware::bluetooth_audio::V2_0::Status.UNSUPPORTED_CODEC_CONFIGURATION implicitly + 1 */,
    /*
     * Pending response from Sink
     */
    SINK_NOT_READY = 3 /* ::vendor::qti::hardware::bluetooth_audio::V2_0::Status.FAILURE implicitly + 1 */,
    /*
     * Telephony Call in progress
     */
    CALL_IN_PROGRESS = 4 /* ::vendor::qti::hardware::bluetooth_audio::V2_0::Status.SINK_NOT_READY implicitly + 1 */,
    /*
     * Short wait error
     */
    SW_ERROR = 5 /* ::vendor::qti::hardware::bluetooth_audio::V2_0::Status.CALL_IN_PROGRESS implicitly + 1 */,
    /*
     * Long wait error
     */
    LW_ERROR = 6 /* ::vendor::qti::hardware::bluetooth_audio::V2_0::Status.SW_ERROR implicitly + 1 */,
    /*
     * disconnection in progress
     */
    FAILURE_DISC_IN_PROGRESS = 7 /* ::vendor::qti::hardware::bluetooth_audio::V2_0::Status.LW_ERROR implicitly + 1 */,
    /*
     * No wait failure
     */
    FAILURE_NO_WAIT = 8 /* ::vendor::qti::hardware::bluetooth_audio::V2_0::Status.FAILURE_DISC_IN_PROGRESS implicitly + 1 */,
};

enum class SessionType : uint8_t {
    UNKNOWN = 0,
    /**
     * A2DP legacy that AVDTP media is encoded by Bluetooth Stack
     */
    A2DP_SOFTWARE_ENCODING_DATAPATH = 1 /* ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType.UNKNOWN implicitly + 1 */,
    /**
     * The encoding of AVDTP media is done by HW and there is control only
     */
    A2DP_HARDWARE_OFFLOAD_DATAPATH = 2 /* ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType.A2DP_SOFTWARE_ENCODING_DATAPATH implicitly + 1 */,
    /**
     * Used when encoded by Bluetooth Stack and streaming to Hearing Aid
     */
    HEARING_AID_SOFTWARE_ENCODING_DATAPATH = 3 /* ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType.A2DP_HARDWARE_OFFLOAD_DATAPATH implicitly + 1 */,
};

enum class CodecType : uint32_t {
    UNKNOWN = 0u /* 0x00 */,
    SBC = 1u /* 0x01 */,
    AAC = 2u /* 0x02 */,
    APTX = 4u /* 0x04 */,
    APTX_HD = 8u /* 0x08 */,
    LDAC = 16u /* 0x10 */,
    BA_CELT = 17u /* 0x11 */,
    APTX_ADAPTIVE = 18u /* 0x12 */,
    APTX_TWS = 20u /* 0x14 */,
};

enum class SampleRate : uint32_t {
    RATE_UNKNOWN = 0u /* 0x00 */,
    RATE_44100 = 1u /* 0x01 */,
    RATE_48000 = 2u /* 0x02 */,
    RATE_88200 = 4u /* 0x04 */,
    RATE_96000 = 8u /* 0x08 */,
    RATE_176400 = 16u /* 0x10 */,
    RATE_192000 = 32u /* 0x20 */,
    RATE_16000 = 64u /* 0x40 */,
    RATE_24000 = 96u /* 0x60 */,
    RATE_32000 = 128u /* 0x80 */,
};

enum class BitsPerSample : uint8_t {
    BITS_UNKNOWN = 0 /* 0x00 */,
    BITS_16 = 1 /* 0x01 */,
    BITS_24 = 2 /* 0x02 */,
    BITS_32 = 4 /* 0x04 */,
};

enum class ChannelMode : uint8_t {
    UNKNOWN = 0 /* 0x00 */,
    MONO = 1 /* 0x01 */,
    STEREO = 2 /* 0x02 */,
};

enum class SbcChannelMode : uint8_t {
    /**
     * Channel Mode: 4 bits
     */
    UNKNOWN = 0 /* 0x00 */,
    JOINT_STEREO = 1 /* 0x01 */,
    STEREO = 2 /* 0x02 */,
    DUAL = 4 /* 0x04 */,
    MONO = 8 /* 0x08 */,
};

enum class SbcBlockLength : uint8_t {
    BLOCKS_4 = 128 /* 0x80 */,
    BLOCKS_8 = 64 /* 0x40 */,
    BLOCKS_12 = 32 /* 0x20 */,
    BLOCKS_16 = 16 /* 0x10 */,
};

enum class SbcNumSubbands : uint8_t {
    SUBBAND_4 = 8 /* 0x08 */,
    SUBBAND_8 = 4 /* 0x04 */,
};

enum class SbcAllocMethod : uint8_t {
    /**
     * SNR
     */
    ALLOC_MD_S = 2 /* 0x02 */,
    /**
     * Loudness
     */
    ALLOC_MD_L = 1 /* 0x01 */,
};

enum class AacObjectType : uint8_t {
    /**
     * MPEG-2 Low Complexity. Support is Mandatory.
     */
    MPEG2_LC = 128 /* 0x80 */,
    /**
     * MPEG-4 Low Complexity. Support is Optional.
     */
    MPEG4_LC = 64 /* 0x40 */,
    /**
     * MPEG-4 Long Term Prediction. Support is Optional.
     */
    MPEG4_LTP = 32 /* 0x20 */,
    /**
     * MPEG-4 Scalable. Support is Optional.
     */
    MPEG4_SCALABLE = 16 /* 0x10 */,
};

enum class AacVariableBitRate : uint8_t {
    ENABLED = 128 /* 0x80 */,
    DISABLED = 0 /* 0x00 */,
};

enum class AptxAdaptiveChannelMode : int32_t {
    /**
     * Channel Mode: 5 bits
     */
    UNCHANGED = -1 /* -1 */,
    JOINT_STEREO = 0,
    MONO = 1,
    DUAL_MONO = 2,
    TWS_STEREO = 4,
    EARBUD = 8,
    TWS_MONO = 10,
    UNKNOWN = 255 /* 0xFF */,
};

enum class AptxMode : uint32_t {
    /**
     * Aptx Adaptive Mode: 32 bits
     */
    UNKNOWN = 0u /* 0x00 */,
    HQ = 4096u /* 0x1000 */,
    LL = 8192u /* 0x2000 */,
    ULL = 16384u /* 0x4000 */,
    SCAN_CONTROL = 32768u /* 0x8000 */,
};

enum class InputMode : uint32_t {
    /**
     * Aptx Adaptive Input Mode: 32 bits
     */
    STEREO = 0u /* 0x00 */,
    DUAL_MONO = 1u /* 0x01 */,
};

struct AptxSinkBuffering final {
    uint8_t minSinkBuff_LL __attribute__ ((aligned(1)));
    uint8_t maxSinkBuff_LL __attribute__ ((aligned(1)));
    uint8_t minSinkBuff_HQ __attribute__ ((aligned(1)));
    uint8_t maxSinkBuff_HQ __attribute__ ((aligned(1)));
    uint8_t minSinkBuff_TWS __attribute__ ((aligned(1)));
    uint8_t maxSinkBuff_TWS __attribute__ ((aligned(1)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering, minSinkBuff_LL) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering, maxSinkBuff_LL) == 1, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering, minSinkBuff_HQ) == 2, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering, maxSinkBuff_HQ) == 3, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering, minSinkBuff_TWS) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering, maxSinkBuff_TWS) == 5, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering) == 6, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering) == 1, "wrong alignment");

struct AptxAdaptive_TTP final {
    uint8_t TTP_LL_low __attribute__ ((aligned(1)));
    uint8_t TTP_LL_high __attribute__ ((aligned(1)));
    uint8_t TTP_HQ_low __attribute__ ((aligned(1)));
    uint8_t TTP_HQ_high __attribute__ ((aligned(1)));
    uint8_t TTP_TWS_low __attribute__ ((aligned(1)));
    uint8_t TTP_TWS_high __attribute__ ((aligned(1)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP, TTP_LL_low) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP, TTP_LL_high) == 1, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP, TTP_HQ_low) == 2, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP, TTP_HQ_high) == 3, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP, TTP_TWS_low) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP, TTP_TWS_high) == 5, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP) == 6, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP) == 1, "wrong alignment");

enum class LdacChannelMode : uint8_t {
    /**
     * Channel Mode: 3 bits
     */
    UNKNOWN = 0 /* 0x00 */,
    STEREO = 1 /* 0x01 */,
    DUAL = 2 /* 0x02 */,
    MONO = 4 /* 0x04 */,
};

enum class LdacQualityIndex : uint8_t {
    QUALITY_HIGH = 0 /* 0x00 */,
    QUALITY_MID = 1 /* 0x01 */,
    QUALITY_LOW = 2 /* 0x02 */,
    QUALITY_ABR = 127 /* 0x7F */,
};

enum class SessionParamType : uint8_t {
    UNKNOWN = 0 /* 0x00 */,
    MTU = 1 /* 0x01 */,
    BITRATE = 2 /* 0x02 */,
    SINK_LATENCY = 3 /* 0x03 */,
};

/**
 * Used for Software Encoding audio feed parameters
 */
struct PcmParameters final {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate sampleRate __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode channelMode __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample bitsPerSample __attribute__ ((aligned(1)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters, sampleRate) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters, channelMode) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters, bitsPerSample) == 5, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters) == 8, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters) == 4, "wrong alignment");

/**
 * Used for Hardware Encoding SBC codec parameters.
 * minBitpool and maxBitpool are not bitfields.
 */
struct SbcParameters final {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate sampleRate __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode channelMode __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength blockLength __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands numSubbands __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod allocMethod __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample bitsPerSample __attribute__ ((aligned(1)));
    uint8_t minBitpool __attribute__ ((aligned(1)));
    uint8_t maxBitpool __attribute__ ((aligned(1)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters, sampleRate) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters, channelMode) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters, blockLength) == 5, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters, numSubbands) == 6, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters, allocMethod) == 7, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters, bitsPerSample) == 8, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters, minBitpool) == 9, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters, maxBitpool) == 10, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters) == 12, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters) == 4, "wrong alignment");

/**
 * Used for Hardware Encoding AAC codec parameters
 */
struct AacParameters final {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType objectType __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate sampleRate __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode channelMode __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate variableBitRateEnabled __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample bitsPerSample __attribute__ ((aligned(1)));
    bool frameControlEnabled __attribute__ ((aligned(1)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters, objectType) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters, sampleRate) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters, channelMode) == 8, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters, variableBitRateEnabled) == 9, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters, bitsPerSample) == 10, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters, frameControlEnabled) == 11, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters) == 12, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters) == 4, "wrong alignment");

/**
 * Used for Hardware Encoding LDAC codec parameters
 * Only used when configuring the codec. When Capabilities are requested, this
 * field is left empty since all qualities must be supported. Not a bitfield.
 */
struct LdacParameters final {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate sampleRate __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode channelMode __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex qualityIndex __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample bitsPerSample __attribute__ ((aligned(1)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters, sampleRate) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters, channelMode) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters, qualityIndex) == 5, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters, bitsPerSample) == 6, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters) == 8, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters) == 4, "wrong alignment");

/**
 * Used for Hardware Encoding AptX and AptX-HD codec parameters
 */
struct AptxParameters final {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate sampleRate __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode channelMode __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample bitsPerSample __attribute__ ((aligned(1)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters, sampleRate) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters, channelMode) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters, bitsPerSample) == 5, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters) == 8, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters) == 4, "wrong alignment");

/**
 * Used for Hardware Encoding AptX-Adaptive codec parameters
 */
struct AptxAdaptiveParameters final {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate sampleRate __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode channelMode __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample bitsPerSample __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode aptxMode __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering sinkBuffering __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP ttp __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode inputMode __attribute__ ((aligned(4)));
    uint32_t inputFadeDuration __attribute__ ((aligned(4)));
    ::android::hardware::hidl_array<uint8_t, 25> aptxAdaptiveConfigStream __attribute__ ((aligned(1)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters, sampleRate) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters, channelMode) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters, bitsPerSample) == 8, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters, aptxMode) == 12, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters, sinkBuffering) == 16, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters, ttp) == 22, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters, inputMode) == 28, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters, inputFadeDuration) == 32, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters, aptxAdaptiveConfigStream) == 36, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters) == 64, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters) == 4, "wrong alignment");

/**
 * Used for Hardware Encoding AptX TWS codec parameters
 */
struct AptxTwsParameters final {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate sampleRate __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode channelMode __attribute__ ((aligned(1)));
    uint8_t syncMode __attribute__ ((aligned(1)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters, sampleRate) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters, channelMode) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters, syncMode) == 5, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters) == 8, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters) == 4, "wrong alignment");

/**
 * Used for Hardware Encoding BA CELT codec parameters
 */
struct BaCeltParameters final {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate sampleRate __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode channelMode __attribute__ ((aligned(1)));
    uint16_t frameSize __attribute__ ((aligned(2)));
    uint16_t complexity __attribute__ ((aligned(2)));
    uint16_t predictionMode __attribute__ ((aligned(2)));
    uint16_t vbrFlag __attribute__ ((aligned(2)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters, sampleRate) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters, channelMode) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters, frameSize) == 6, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters, complexity) == 8, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters, predictionMode) == 10, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters, vbrFlag) == 12, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters) == 16, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters) == 4, "wrong alignment");

/**
 * Used to specify the capabilities of the codecs supported by Hardware Encoding.
 * AptX and AptX-HD both use the AptxParameters field.
 */
struct CodecCapabilities final {
    // Forward declaration for forward reference support:
    union Capabilities;

    union Capabilities final {
        ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters sbcCapabilities __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters aacCapabilities __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters ldacCapabilities __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters aptxCapabilities __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters aptxAdaptiveCapabilities __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters aptxTwsCapabilities __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters baCeltCapabilities __attribute__ ((aligned(4)));
    };

    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities, sbcCapabilities) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities, aacCapabilities) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities, ldacCapabilities) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities, aptxCapabilities) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities, aptxAdaptiveCapabilities) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities, aptxTwsCapabilities) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities, baCeltCapabilities) == 0, "wrong offset");
    static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities) == 64, "wrong size");
    static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities) == 4, "wrong alignment");

    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType codecType __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities capabilities __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities, codecType) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities, capabilities) == 4, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities) == 68, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities) == 4, "wrong alignment");

struct SinkLatency final {
    uint64_t remoteDeviceAudioDelay __attribute__ ((aligned(8)));
    uint64_t transmittedOctets __attribute__ ((aligned(8)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec transmittedOctetsTimeStamp __attribute__ ((aligned(8)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency, remoteDeviceAudioDelay) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency, transmittedOctets) == 8, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency, transmittedOctetsTimeStamp) == 16, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency) == 32, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency) == 8, "wrong alignment");

/**
 * Used for proactive update of Session Parameters to server
 */
struct SessionParams final {
    // Forward declaration for forward reference support:
    union Param;

    union Param final {
        uint16_t mtu __attribute__ ((aligned(2)));
        uint32_t encodedAudioBitrate __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency sinkLatency __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams::Param, mtu) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams::Param, encodedAudioBitrate) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams::Param, sinkLatency) == 0, "wrong offset");
    static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams::Param) == 32, "wrong size");
    static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams::Param) == 8, "wrong alignment");

    ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType paramType __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams::Param param __attribute__ ((aligned(8)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams, paramType) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams, param) == 8, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams) == 40, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams) == 8, "wrong alignment");

/**
 * Used to specify the capabilities of the different session types.
 */
union AudioCapabilities final {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters pcmCapabilities __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities codecCapabilities __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AudioCapabilities, pcmCapabilities) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AudioCapabilities, codecCapabilities) == 0, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::AudioCapabilities) == 68, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::AudioCapabilities) == 4, "wrong alignment");

/**
 * Used to configure a Hardware Encoding session.
 * AptX and AptX-HD both use the AptxParameters field.
 */
struct CodecConfiguration final {
    // Forward declaration for forward reference support:
    union CodecSpecific;

    union CodecSpecific final {
        ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters sbcConfig __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters aacConfig __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters ldacConfig __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters aptxConfig __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters aptxAdaptiveConfig __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters aptxTwsConfig __attribute__ ((aligned(4)));
        ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters baCeltConfig __attribute__ ((aligned(4)));
    };

    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific, sbcConfig) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific, aacConfig) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific, ldacConfig) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific, aptxConfig) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific, aptxAdaptiveConfig) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific, aptxTwsConfig) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific, baCeltConfig) == 0, "wrong offset");
    static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific) == 64, "wrong size");
    static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific) == 4, "wrong alignment");

    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType codecType __attribute__ ((aligned(4)));
    /**
     * The encoded audio bitrate in bits / second.
     * 0x00000000 - The audio bitrate is not specified / unused
     * 0x00000001 - 0x00FFFFFF - Encoded audio bitrate in bits/second
     * 0x01000000 - 0xFFFFFFFF - Reserved
     *
     * The HAL needs to support all legal bitrates for the selected codec.
     */
    uint32_t encodedAudioBitrate __attribute__ ((aligned(4)));
    /**
     * Peer MTU (in octets)
     */
    uint16_t peerMtu __attribute__ ((aligned(2)));
    /**
     * Content protection by SCMS-T
     */
    bool isScmstEnabled __attribute__ ((aligned(1)));
    /**
     * Scrambling Requirement
     */
    bool isScramblingEnabled __attribute__ ((aligned(1)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific config __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration, codecType) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration, encodedAudioBitrate) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration, peerMtu) == 8, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration, isScmstEnabled) == 10, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration, isScramblingEnabled) == 11, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration, config) == 12, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration) == 76, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration) == 4, "wrong alignment");

/**
 * Used to configure either a Hardware or Software Encoding session based on session type
 */
union AudioConfiguration final {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters pcmConfig __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration codecConfig __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AudioConfiguration, pcmConfig) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_0::AudioConfiguration, codecConfig) == 0, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_0::AudioConfiguration) == 76, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_0::AudioConfiguration) == 4, "wrong alignment");

//
// type declarations for package
//

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec& rhs);

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::Status o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::Status o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::Status lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::Status rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::Status rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::Status lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::Status lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::Status rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::Status rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::Status lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::Status e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::Status e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint32_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType o, ::std::ostream* os);
constexpr uint32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const uint32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const uint32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}
constexpr uint32_t &operator|=(uint32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType e) {
    v |= static_cast<uint32_t>(e);
    return v;
}
constexpr uint32_t &operator&=(uint32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint32_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate o, ::std::ostream* os);
constexpr uint32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const uint32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const uint32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}
constexpr uint32_t &operator|=(uint32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate e) {
    v |= static_cast<uint32_t>(e);
    return v;
}
constexpr uint32_t &operator&=(uint32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(int32_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode o, ::std::ostream* os);
constexpr int32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const int32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}
constexpr int32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const int32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}
constexpr int32_t &operator|=(int32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode e) {
    v |= static_cast<int32_t>(e);
    return v;
}
constexpr int32_t &operator&=(int32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint32_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode o, ::std::ostream* os);
constexpr uint32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const uint32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const uint32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}
constexpr uint32_t &operator|=(uint32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode e) {
    v |= static_cast<uint32_t>(e);
    return v;
}
constexpr uint32_t &operator&=(uint32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint32_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode o, ::std::ostream* os);
constexpr uint32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const uint32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const uint32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}
constexpr uint32_t &operator|=(uint32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode e) {
    v |= static_cast<uint32_t>(e);
    return v;
}
constexpr uint32_t &operator&=(uint32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering& rhs);

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP& rhs);

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters& rhs);

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters& rhs);

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters& rhs);

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters& rhs);

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters& rhs);

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters& rhs);

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters& rhs);

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters& rhs);

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities& o, ::std::ostream*);
// operator== and operator!= are not generated for Capabilities

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities& o, ::std::ostream*);
// operator== and operator!= are not generated for CodecCapabilities

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency& rhs);

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams::Param& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams::Param& o, ::std::ostream*);
// operator== and operator!= are not generated for Param

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams& o, ::std::ostream*);
// operator== and operator!= are not generated for SessionParams

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioCapabilities& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioCapabilities& o, ::std::ostream*);
// operator== and operator!= are not generated for AudioCapabilities

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific& o, ::std::ostream*);
// operator== and operator!= are not generated for CodecSpecific

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration& o, ::std::ostream*);
// operator== and operator!= are not generated for CodecConfiguration

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioConfiguration& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioConfiguration& o, ::std::ostream*);
// operator== and operator!= are not generated for AudioConfiguration

//
// type header definitions for package
//

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".tvSec = ";
    os += ::android::hardware::toString(o.tvSec);
    os += ", .tvNSec = ";
    os += ::android::hardware::toString(o.tvNSec);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec& rhs) {
    if (lhs.tvSec != rhs.tvSec) {
        return false;
    }
    if (lhs.tvNSec != rhs.tvNSec) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::TimeSpec& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::Status>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::Status> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SUCCESS) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SUCCESS)) {
        os += (first ? "" : " | ");
        os += "SUCCESS";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SUCCESS;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::UNSUPPORTED_CODEC_CONFIGURATION) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::Status::UNSUPPORTED_CODEC_CONFIGURATION)) {
        os += (first ? "" : " | ");
        os += "UNSUPPORTED_CODEC_CONFIGURATION";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::UNSUPPORTED_CODEC_CONFIGURATION;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE)) {
        os += (first ? "" : " | ");
        os += "FAILURE";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SINK_NOT_READY) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SINK_NOT_READY)) {
        os += (first ? "" : " | ");
        os += "SINK_NOT_READY";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SINK_NOT_READY;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::CALL_IN_PROGRESS) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::Status::CALL_IN_PROGRESS)) {
        os += (first ? "" : " | ");
        os += "CALL_IN_PROGRESS";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::CALL_IN_PROGRESS;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SW_ERROR) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SW_ERROR)) {
        os += (first ? "" : " | ");
        os += "SW_ERROR";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SW_ERROR;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::LW_ERROR) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::Status::LW_ERROR)) {
        os += (first ? "" : " | ");
        os += "LW_ERROR";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::LW_ERROR;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE_DISC_IN_PROGRESS) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE_DISC_IN_PROGRESS)) {
        os += (first ? "" : " | ");
        os += "FAILURE_DISC_IN_PROGRESS";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE_DISC_IN_PROGRESS;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE_NO_WAIT) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE_NO_WAIT)) {
        os += (first ? "" : " | ");
        os += "FAILURE_NO_WAIT";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE_NO_WAIT;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::Status o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SUCCESS) {
        return "SUCCESS";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::UNSUPPORTED_CODEC_CONFIGURATION) {
        return "UNSUPPORTED_CODEC_CONFIGURATION";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE) {
        return "FAILURE";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SINK_NOT_READY) {
        return "SINK_NOT_READY";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::CALL_IN_PROGRESS) {
        return "CALL_IN_PROGRESS";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SW_ERROR) {
        return "SW_ERROR";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::LW_ERROR) {
        return "LW_ERROR";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE_DISC_IN_PROGRESS) {
        return "FAILURE_DISC_IN_PROGRESS";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE_NO_WAIT) {
        return "FAILURE_NO_WAIT";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::Status o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::UNKNOWN) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::A2DP_SOFTWARE_ENCODING_DATAPATH) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::A2DP_SOFTWARE_ENCODING_DATAPATH)) {
        os += (first ? "" : " | ");
        os += "A2DP_SOFTWARE_ENCODING_DATAPATH";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::A2DP_SOFTWARE_ENCODING_DATAPATH;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::A2DP_HARDWARE_OFFLOAD_DATAPATH) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::A2DP_HARDWARE_OFFLOAD_DATAPATH)) {
        os += (first ? "" : " | ");
        os += "A2DP_HARDWARE_OFFLOAD_DATAPATH";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::A2DP_HARDWARE_OFFLOAD_DATAPATH;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::HEARING_AID_SOFTWARE_ENCODING_DATAPATH) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::HEARING_AID_SOFTWARE_ENCODING_DATAPATH)) {
        os += (first ? "" : " | ");
        os += "HEARING_AID_SOFTWARE_ENCODING_DATAPATH";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::HEARING_AID_SOFTWARE_ENCODING_DATAPATH;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::UNKNOWN) {
        return "UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::A2DP_SOFTWARE_ENCODING_DATAPATH) {
        return "A2DP_SOFTWARE_ENCODING_DATAPATH";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::A2DP_HARDWARE_OFFLOAD_DATAPATH) {
        return "A2DP_HARDWARE_OFFLOAD_DATAPATH";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::HEARING_AID_SOFTWARE_ENCODING_DATAPATH) {
        return "HEARING_AID_SOFTWARE_ENCODING_DATAPATH";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType>(uint32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::UNKNOWN) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::SBC) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::SBC)) {
        os += (first ? "" : " | ");
        os += "SBC";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::SBC;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::AAC) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::AAC)) {
        os += (first ? "" : " | ");
        os += "AAC";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::AAC;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX)) {
        os += (first ? "" : " | ");
        os += "APTX";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_HD) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_HD)) {
        os += (first ? "" : " | ");
        os += "APTX_HD";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_HD;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::LDAC) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::LDAC)) {
        os += (first ? "" : " | ");
        os += "LDAC";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::LDAC;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::BA_CELT) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::BA_CELT)) {
        os += (first ? "" : " | ");
        os += "BA_CELT";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::BA_CELT;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_ADAPTIVE) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_ADAPTIVE)) {
        os += (first ? "" : " | ");
        os += "APTX_ADAPTIVE";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_ADAPTIVE;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_TWS) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_TWS)) {
        os += (first ? "" : " | ");
        os += "APTX_TWS";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_TWS;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::UNKNOWN) {
        return "UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::SBC) {
        return "SBC";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::AAC) {
        return "AAC";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX) {
        return "APTX";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_HD) {
        return "APTX_HD";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::LDAC) {
        return "LDAC";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::BA_CELT) {
        return "BA_CELT";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_ADAPTIVE) {
        return "APTX_ADAPTIVE";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_TWS) {
        return "APTX_TWS";
    }
    std::string os;
    os += toHexString(static_cast<uint32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate>(uint32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_UNKNOWN) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "RATE_UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_44100) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_44100)) {
        os += (first ? "" : " | ");
        os += "RATE_44100";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_44100;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_48000) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_48000)) {
        os += (first ? "" : " | ");
        os += "RATE_48000";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_48000;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_88200) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_88200)) {
        os += (first ? "" : " | ");
        os += "RATE_88200";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_88200;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_96000) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_96000)) {
        os += (first ? "" : " | ");
        os += "RATE_96000";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_96000;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_176400) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_176400)) {
        os += (first ? "" : " | ");
        os += "RATE_176400";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_176400;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_192000) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_192000)) {
        os += (first ? "" : " | ");
        os += "RATE_192000";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_192000;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_16000) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_16000)) {
        os += (first ? "" : " | ");
        os += "RATE_16000";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_16000;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_24000) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_24000)) {
        os += (first ? "" : " | ");
        os += "RATE_24000";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_24000;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_32000) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_32000)) {
        os += (first ? "" : " | ");
        os += "RATE_32000";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_32000;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_UNKNOWN) {
        return "RATE_UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_44100) {
        return "RATE_44100";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_48000) {
        return "RATE_48000";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_88200) {
        return "RATE_88200";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_96000) {
        return "RATE_96000";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_176400) {
        return "RATE_176400";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_192000) {
        return "RATE_192000";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_16000) {
        return "RATE_16000";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_24000) {
        return "RATE_24000";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_32000) {
        return "RATE_32000";
    }
    std::string os;
    os += toHexString(static_cast<uint32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_UNKNOWN) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "BITS_UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_16) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_16)) {
        os += (first ? "" : " | ");
        os += "BITS_16";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_16;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_24) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_24)) {
        os += (first ? "" : " | ");
        os += "BITS_24";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_24;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_32) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_32)) {
        os += (first ? "" : " | ");
        os += "BITS_32";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_32;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_UNKNOWN) {
        return "BITS_UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_16) {
        return "BITS_16";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_24) {
        return "BITS_24";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_32) {
        return "BITS_32";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::UNKNOWN) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::MONO) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::MONO)) {
        os += (first ? "" : " | ");
        os += "MONO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::MONO;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::STEREO) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::STEREO)) {
        os += (first ? "" : " | ");
        os += "STEREO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::STEREO;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::UNKNOWN) {
        return "UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::MONO) {
        return "MONO";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::STEREO) {
        return "STEREO";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::UNKNOWN) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::JOINT_STEREO) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::JOINT_STEREO)) {
        os += (first ? "" : " | ");
        os += "JOINT_STEREO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::JOINT_STEREO;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::STEREO) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::STEREO)) {
        os += (first ? "" : " | ");
        os += "STEREO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::STEREO;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::DUAL) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::DUAL)) {
        os += (first ? "" : " | ");
        os += "DUAL";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::DUAL;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::MONO) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::MONO)) {
        os += (first ? "" : " | ");
        os += "MONO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::MONO;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::UNKNOWN) {
        return "UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::JOINT_STEREO) {
        return "JOINT_STEREO";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::STEREO) {
        return "STEREO";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::DUAL) {
        return "DUAL";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::MONO) {
        return "MONO";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_4) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_4)) {
        os += (first ? "" : " | ");
        os += "BLOCKS_4";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_4;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_8) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_8)) {
        os += (first ? "" : " | ");
        os += "BLOCKS_8";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_8;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_12) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_12)) {
        os += (first ? "" : " | ");
        os += "BLOCKS_12";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_12;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_16) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_16)) {
        os += (first ? "" : " | ");
        os += "BLOCKS_16";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_16;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_4) {
        return "BLOCKS_4";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_8) {
        return "BLOCKS_8";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_12) {
        return "BLOCKS_12";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_16) {
        return "BLOCKS_16";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands::SUBBAND_4) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands::SUBBAND_4)) {
        os += (first ? "" : " | ");
        os += "SUBBAND_4";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands::SUBBAND_4;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands::SUBBAND_8) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands::SUBBAND_8)) {
        os += (first ? "" : " | ");
        os += "SUBBAND_8";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands::SUBBAND_8;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands::SUBBAND_4) {
        return "SUBBAND_4";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands::SUBBAND_8) {
        return "SUBBAND_8";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod::ALLOC_MD_S) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod::ALLOC_MD_S)) {
        os += (first ? "" : " | ");
        os += "ALLOC_MD_S";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod::ALLOC_MD_S;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod::ALLOC_MD_L) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod::ALLOC_MD_L)) {
        os += (first ? "" : " | ");
        os += "ALLOC_MD_L";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod::ALLOC_MD_L;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod::ALLOC_MD_S) {
        return "ALLOC_MD_S";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod::ALLOC_MD_L) {
        return "ALLOC_MD_L";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG2_LC) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG2_LC)) {
        os += (first ? "" : " | ");
        os += "MPEG2_LC";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG2_LC;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_LC) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_LC)) {
        os += (first ? "" : " | ");
        os += "MPEG4_LC";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_LC;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_LTP) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_LTP)) {
        os += (first ? "" : " | ");
        os += "MPEG4_LTP";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_LTP;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_SCALABLE) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_SCALABLE)) {
        os += (first ? "" : " | ");
        os += "MPEG4_SCALABLE";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_SCALABLE;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG2_LC) {
        return "MPEG2_LC";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_LC) {
        return "MPEG4_LC";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_LTP) {
        return "MPEG4_LTP";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_SCALABLE) {
        return "MPEG4_SCALABLE";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate::ENABLED) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate::ENABLED)) {
        os += (first ? "" : " | ");
        os += "ENABLED";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate::ENABLED;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate::DISABLED) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate::DISABLED)) {
        os += (first ? "" : " | ");
        os += "DISABLED";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate::DISABLED;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate::ENABLED) {
        return "ENABLED";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate::DISABLED) {
        return "DISABLED";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode>(int32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::UNCHANGED) == static_cast<int32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::UNCHANGED)) {
        os += (first ? "" : " | ");
        os += "UNCHANGED";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::UNCHANGED;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::JOINT_STEREO) == static_cast<int32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::JOINT_STEREO)) {
        os += (first ? "" : " | ");
        os += "JOINT_STEREO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::JOINT_STEREO;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::MONO) == static_cast<int32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::MONO)) {
        os += (first ? "" : " | ");
        os += "MONO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::MONO;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::DUAL_MONO) == static_cast<int32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::DUAL_MONO)) {
        os += (first ? "" : " | ");
        os += "DUAL_MONO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::DUAL_MONO;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::TWS_STEREO) == static_cast<int32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::TWS_STEREO)) {
        os += (first ? "" : " | ");
        os += "TWS_STEREO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::TWS_STEREO;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::EARBUD) == static_cast<int32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::EARBUD)) {
        os += (first ? "" : " | ");
        os += "EARBUD";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::EARBUD;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::TWS_MONO) == static_cast<int32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::TWS_MONO)) {
        os += (first ? "" : " | ");
        os += "TWS_MONO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::TWS_MONO;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::UNKNOWN) == static_cast<int32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::UNKNOWN;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::UNCHANGED) {
        return "UNCHANGED";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::JOINT_STEREO) {
        return "JOINT_STEREO";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::MONO) {
        return "MONO";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::DUAL_MONO) {
        return "DUAL_MONO";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::TWS_STEREO) {
        return "TWS_STEREO";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::EARBUD) {
        return "EARBUD";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::TWS_MONO) {
        return "TWS_MONO";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::UNKNOWN) {
        return "UNKNOWN";
    }
    std::string os;
    os += toHexString(static_cast<int32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode>(uint32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::UNKNOWN) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::HQ) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::HQ)) {
        os += (first ? "" : " | ");
        os += "HQ";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::HQ;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::LL) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::LL)) {
        os += (first ? "" : " | ");
        os += "LL";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::LL;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::ULL) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::ULL)) {
        os += (first ? "" : " | ");
        os += "ULL";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::ULL;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::SCAN_CONTROL) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::SCAN_CONTROL)) {
        os += (first ? "" : " | ");
        os += "SCAN_CONTROL";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::SCAN_CONTROL;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::UNKNOWN) {
        return "UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::HQ) {
        return "HQ";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::LL) {
        return "LL";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::ULL) {
        return "ULL";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::SCAN_CONTROL) {
        return "SCAN_CONTROL";
    }
    std::string os;
    os += toHexString(static_cast<uint32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode>(uint32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode::STEREO) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode::STEREO)) {
        os += (first ? "" : " | ");
        os += "STEREO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode::STEREO;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode::DUAL_MONO) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode::DUAL_MONO)) {
        os += (first ? "" : " | ");
        os += "DUAL_MONO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode::DUAL_MONO;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode::STEREO) {
        return "STEREO";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode::DUAL_MONO) {
        return "DUAL_MONO";
    }
    std::string os;
    os += toHexString(static_cast<uint32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".minSinkBuff_LL = ";
    os += ::android::hardware::toString(o.minSinkBuff_LL);
    os += ", .maxSinkBuff_LL = ";
    os += ::android::hardware::toString(o.maxSinkBuff_LL);
    os += ", .minSinkBuff_HQ = ";
    os += ::android::hardware::toString(o.minSinkBuff_HQ);
    os += ", .maxSinkBuff_HQ = ";
    os += ::android::hardware::toString(o.maxSinkBuff_HQ);
    os += ", .minSinkBuff_TWS = ";
    os += ::android::hardware::toString(o.minSinkBuff_TWS);
    os += ", .maxSinkBuff_TWS = ";
    os += ::android::hardware::toString(o.maxSinkBuff_TWS);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering& rhs) {
    if (lhs.minSinkBuff_LL != rhs.minSinkBuff_LL) {
        return false;
    }
    if (lhs.maxSinkBuff_LL != rhs.maxSinkBuff_LL) {
        return false;
    }
    if (lhs.minSinkBuff_HQ != rhs.minSinkBuff_HQ) {
        return false;
    }
    if (lhs.maxSinkBuff_HQ != rhs.maxSinkBuff_HQ) {
        return false;
    }
    if (lhs.minSinkBuff_TWS != rhs.minSinkBuff_TWS) {
        return false;
    }
    if (lhs.maxSinkBuff_TWS != rhs.maxSinkBuff_TWS) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxSinkBuffering& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".TTP_LL_low = ";
    os += ::android::hardware::toString(o.TTP_LL_low);
    os += ", .TTP_LL_high = ";
    os += ::android::hardware::toString(o.TTP_LL_high);
    os += ", .TTP_HQ_low = ";
    os += ::android::hardware::toString(o.TTP_HQ_low);
    os += ", .TTP_HQ_high = ";
    os += ::android::hardware::toString(o.TTP_HQ_high);
    os += ", .TTP_TWS_low = ";
    os += ::android::hardware::toString(o.TTP_TWS_low);
    os += ", .TTP_TWS_high = ";
    os += ::android::hardware::toString(o.TTP_TWS_high);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP& rhs) {
    if (lhs.TTP_LL_low != rhs.TTP_LL_low) {
        return false;
    }
    if (lhs.TTP_LL_high != rhs.TTP_LL_high) {
        return false;
    }
    if (lhs.TTP_HQ_low != rhs.TTP_HQ_low) {
        return false;
    }
    if (lhs.TTP_HQ_high != rhs.TTP_HQ_high) {
        return false;
    }
    if (lhs.TTP_TWS_low != rhs.TTP_TWS_low) {
        return false;
    }
    if (lhs.TTP_TWS_high != rhs.TTP_TWS_high) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptive_TTP& rhs){
    return !(lhs == rhs);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::UNKNOWN) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::STEREO) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::STEREO)) {
        os += (first ? "" : " | ");
        os += "STEREO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::STEREO;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::DUAL) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::DUAL)) {
        os += (first ? "" : " | ");
        os += "DUAL";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::DUAL;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::MONO) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::MONO)) {
        os += (first ? "" : " | ");
        os += "MONO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::MONO;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::UNKNOWN) {
        return "UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::STEREO) {
        return "STEREO";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::DUAL) {
        return "DUAL";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::MONO) {
        return "MONO";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_HIGH) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_HIGH)) {
        os += (first ? "" : " | ");
        os += "QUALITY_HIGH";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_HIGH;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_MID) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_MID)) {
        os += (first ? "" : " | ");
        os += "QUALITY_MID";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_MID;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_LOW) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_LOW)) {
        os += (first ? "" : " | ");
        os += "QUALITY_LOW";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_LOW;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_ABR) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_ABR)) {
        os += (first ? "" : " | ");
        os += "QUALITY_ABR";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_ABR;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_HIGH) {
        return "QUALITY_HIGH";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_MID) {
        return "QUALITY_MID";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_LOW) {
        return "QUALITY_LOW";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_ABR) {
        return "QUALITY_ABR";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::UNKNOWN) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::MTU) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::MTU)) {
        os += (first ? "" : " | ");
        os += "MTU";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::MTU;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::BITRATE) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::BITRATE)) {
        os += (first ? "" : " | ");
        os += "BITRATE";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::BITRATE;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::SINK_LATENCY) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::SINK_LATENCY)) {
        os += (first ? "" : " | ");
        os += "SINK_LATENCY";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::SINK_LATENCY;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::UNKNOWN) {
        return "UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::MTU) {
        return "MTU";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::BITRATE) {
        return "BITRATE";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::SINK_LATENCY) {
        return "SINK_LATENCY";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".sampleRate = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.sampleRate);
    os += ", .channelMode = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.channelMode);
    os += ", .bitsPerSample = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.bitsPerSample);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters& rhs) {
    if (lhs.sampleRate != rhs.sampleRate) {
        return false;
    }
    if (lhs.channelMode != rhs.channelMode) {
        return false;
    }
    if (lhs.bitsPerSample != rhs.bitsPerSample) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".sampleRate = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.sampleRate);
    os += ", .channelMode = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.channelMode);
    os += ", .blockLength = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.blockLength);
    os += ", .numSubbands = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.numSubbands);
    os += ", .allocMethod = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.allocMethod);
    os += ", .bitsPerSample = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.bitsPerSample);
    os += ", .minBitpool = ";
    os += ::android::hardware::toString(o.minBitpool);
    os += ", .maxBitpool = ";
    os += ::android::hardware::toString(o.maxBitpool);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters& rhs) {
    if (lhs.sampleRate != rhs.sampleRate) {
        return false;
    }
    if (lhs.channelMode != rhs.channelMode) {
        return false;
    }
    if (lhs.blockLength != rhs.blockLength) {
        return false;
    }
    if (lhs.numSubbands != rhs.numSubbands) {
        return false;
    }
    if (lhs.allocMethod != rhs.allocMethod) {
        return false;
    }
    if (lhs.bitsPerSample != rhs.bitsPerSample) {
        return false;
    }
    if (lhs.minBitpool != rhs.minBitpool) {
        return false;
    }
    if (lhs.maxBitpool != rhs.maxBitpool) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcParameters& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".objectType = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.objectType);
    os += ", .sampleRate = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.sampleRate);
    os += ", .channelMode = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.channelMode);
    os += ", .variableBitRateEnabled = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.variableBitRateEnabled);
    os += ", .bitsPerSample = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.bitsPerSample);
    os += ", .frameControlEnabled = ";
    os += ::android::hardware::toString(o.frameControlEnabled);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters& rhs) {
    if (lhs.objectType != rhs.objectType) {
        return false;
    }
    if (lhs.sampleRate != rhs.sampleRate) {
        return false;
    }
    if (lhs.channelMode != rhs.channelMode) {
        return false;
    }
    if (lhs.variableBitRateEnabled != rhs.variableBitRateEnabled) {
        return false;
    }
    if (lhs.bitsPerSample != rhs.bitsPerSample) {
        return false;
    }
    if (lhs.frameControlEnabled != rhs.frameControlEnabled) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AacParameters& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".sampleRate = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.sampleRate);
    os += ", .channelMode = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.channelMode);
    os += ", .qualityIndex = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.qualityIndex);
    os += ", .bitsPerSample = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.bitsPerSample);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters& rhs) {
    if (lhs.sampleRate != rhs.sampleRate) {
        return false;
    }
    if (lhs.channelMode != rhs.channelMode) {
        return false;
    }
    if (lhs.qualityIndex != rhs.qualityIndex) {
        return false;
    }
    if (lhs.bitsPerSample != rhs.bitsPerSample) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacParameters& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".sampleRate = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.sampleRate);
    os += ", .channelMode = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.channelMode);
    os += ", .bitsPerSample = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.bitsPerSample);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters& rhs) {
    if (lhs.sampleRate != rhs.sampleRate) {
        return false;
    }
    if (lhs.channelMode != rhs.channelMode) {
        return false;
    }
    if (lhs.bitsPerSample != rhs.bitsPerSample) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxParameters& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".sampleRate = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.sampleRate);
    os += ", .channelMode = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.channelMode);
    os += ", .bitsPerSample = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.bitsPerSample);
    os += ", .aptxMode = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.aptxMode);
    os += ", .sinkBuffering = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.sinkBuffering);
    os += ", .ttp = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.ttp);
    os += ", .inputMode = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.inputMode);
    os += ", .inputFadeDuration = ";
    os += ::android::hardware::toString(o.inputFadeDuration);
    os += ", .aptxAdaptiveConfigStream = ";
    os += ::android::hardware::toString(o.aptxAdaptiveConfigStream);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters& rhs) {
    if (lhs.sampleRate != rhs.sampleRate) {
        return false;
    }
    if (lhs.channelMode != rhs.channelMode) {
        return false;
    }
    if (lhs.bitsPerSample != rhs.bitsPerSample) {
        return false;
    }
    if (lhs.aptxMode != rhs.aptxMode) {
        return false;
    }
    if (lhs.sinkBuffering != rhs.sinkBuffering) {
        return false;
    }
    if (lhs.ttp != rhs.ttp) {
        return false;
    }
    if (lhs.inputMode != rhs.inputMode) {
        return false;
    }
    if (lhs.inputFadeDuration != rhs.inputFadeDuration) {
        return false;
    }
    if (lhs.aptxAdaptiveConfigStream != rhs.aptxAdaptiveConfigStream) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveParameters& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".sampleRate = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.sampleRate);
    os += ", .channelMode = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.channelMode);
    os += ", .syncMode = ";
    os += ::android::hardware::toString(o.syncMode);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters& rhs) {
    if (lhs.sampleRate != rhs.sampleRate) {
        return false;
    }
    if (lhs.channelMode != rhs.channelMode) {
        return false;
    }
    if (lhs.syncMode != rhs.syncMode) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxTwsParameters& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".sampleRate = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.sampleRate);
    os += ", .channelMode = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.channelMode);
    os += ", .frameSize = ";
    os += ::android::hardware::toString(o.frameSize);
    os += ", .complexity = ";
    os += ::android::hardware::toString(o.complexity);
    os += ", .predictionMode = ";
    os += ::android::hardware::toString(o.predictionMode);
    os += ", .vbrFlag = ";
    os += ::android::hardware::toString(o.vbrFlag);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters& rhs) {
    if (lhs.sampleRate != rhs.sampleRate) {
        return false;
    }
    if (lhs.channelMode != rhs.channelMode) {
        return false;
    }
    if (lhs.frameSize != rhs.frameSize) {
        return false;
    }
    if (lhs.complexity != rhs.complexity) {
        return false;
    }
    if (lhs.predictionMode != rhs.predictionMode) {
        return false;
    }
    if (lhs.vbrFlag != rhs.vbrFlag) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::BaCeltParameters& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".sbcCapabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.sbcCapabilities);
    os += ", .aacCapabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.aacCapabilities);
    os += ", .ldacCapabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.ldacCapabilities);
    os += ", .aptxCapabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.aptxCapabilities);
    os += ", .aptxAdaptiveCapabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.aptxAdaptiveCapabilities);
    os += ", .aptxTwsCapabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.aptxTwsCapabilities);
    os += ", .baCeltCapabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.baCeltCapabilities);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities::Capabilities& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for Capabilities

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".codecType = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.codecType);
    os += ", .capabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.capabilities);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecCapabilities& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for CodecCapabilities

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".remoteDeviceAudioDelay = ";
    os += ::android::hardware::toString(o.remoteDeviceAudioDelay);
    os += ", .transmittedOctets = ";
    os += ::android::hardware::toString(o.transmittedOctets);
    os += ", .transmittedOctetsTimeStamp = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.transmittedOctetsTimeStamp);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency& rhs) {
    if (lhs.remoteDeviceAudioDelay != rhs.remoteDeviceAudioDelay) {
        return false;
    }
    if (lhs.transmittedOctets != rhs.transmittedOctets) {
        return false;
    }
    if (lhs.transmittedOctetsTimeStamp != rhs.transmittedOctetsTimeStamp) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_0::SinkLatency& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams::Param& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".mtu = ";
    os += ::android::hardware::toString(o.mtu);
    os += ", .encodedAudioBitrate = ";
    os += ::android::hardware::toString(o.encodedAudioBitrate);
    os += ", .sinkLatency = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.sinkLatency);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams::Param& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for Param

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".paramType = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.paramType);
    os += ", .param = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.param);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParams& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for SessionParams

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioCapabilities& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".pcmCapabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.pcmCapabilities);
    os += ", .codecCapabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.codecCapabilities);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioCapabilities& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for AudioCapabilities

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".sbcConfig = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.sbcConfig);
    os += ", .aacConfig = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.aacConfig);
    os += ", .ldacConfig = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.ldacConfig);
    os += ", .aptxConfig = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.aptxConfig);
    os += ", .aptxAdaptiveConfig = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.aptxAdaptiveConfig);
    os += ", .aptxTwsConfig = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.aptxTwsConfig);
    os += ", .baCeltConfig = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.baCeltConfig);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration::CodecSpecific& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for CodecSpecific

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".codecType = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.codecType);
    os += ", .encodedAudioBitrate = ";
    os += ::android::hardware::toString(o.encodedAudioBitrate);
    os += ", .peerMtu = ";
    os += ::android::hardware::toString(o.peerMtu);
    os += ", .isScmstEnabled = ";
    os += ::android::hardware::toString(o.isScmstEnabled);
    os += ", .isScramblingEnabled = ";
    os += ::android::hardware::toString(o.isScramblingEnabled);
    os += ", .config = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.config);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecConfiguration& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for CodecConfiguration

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioConfiguration& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".pcmConfig = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.pcmConfig);
    os += ", .codecConfig = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.codecConfig);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_0::AudioConfiguration& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for AudioConfiguration


}  // namespace V2_0
}  // namespace bluetooth_audio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

//
// global type declarations for package
//

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::Status, 9> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::Status> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SUCCESS,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::UNSUPPORTED_CODEC_CONFIGURATION,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SINK_NOT_READY,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::CALL_IN_PROGRESS,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::SW_ERROR,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::LW_ERROR,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE_DISC_IN_PROGRESS,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::Status::FAILURE_NO_WAIT,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType, 4> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::UNKNOWN,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::A2DP_SOFTWARE_ENCODING_DATAPATH,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::A2DP_HARDWARE_OFFLOAD_DATAPATH,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionType::HEARING_AID_SOFTWARE_ENCODING_DATAPATH,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType, 9> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::UNKNOWN,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::SBC,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::AAC,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_HD,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::LDAC,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::BA_CELT,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_ADAPTIVE,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::CodecType::APTX_TWS,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate, 10> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_UNKNOWN,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_44100,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_48000,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_88200,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_96000,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_176400,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_192000,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_16000,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_24000,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SampleRate::RATE_32000,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample, 4> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_UNKNOWN,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_16,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_24,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample::BITS_32,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode, 3> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::UNKNOWN,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::MONO,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::ChannelMode::STEREO,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode, 5> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::UNKNOWN,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::JOINT_STEREO,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::STEREO,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::DUAL,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcChannelMode::MONO,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength, 4> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_4,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_8,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_12,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcBlockLength::BLOCKS_16,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands, 2> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands::SUBBAND_4,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcNumSubbands::SUBBAND_8,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod, 2> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod::ALLOC_MD_S,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SbcAllocMethod::ALLOC_MD_L,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType, 4> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG2_LC,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_LC,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_LTP,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AacObjectType::MPEG4_SCALABLE,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate, 2> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate::ENABLED,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AacVariableBitRate::DISABLED,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode, 8> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::UNCHANGED,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::JOINT_STEREO,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::MONO,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::DUAL_MONO,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::TWS_STEREO,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::EARBUD,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::TWS_MONO,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxAdaptiveChannelMode::UNKNOWN,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode, 5> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::UNKNOWN,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::HQ,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::LL,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::ULL,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::AptxMode::SCAN_CONTROL,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode, 2> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode::STEREO,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::InputMode::DUAL_MONO,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode, 4> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::UNKNOWN,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::STEREO,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::DUAL,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacChannelMode::MONO,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex, 4> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_HIGH,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_MID,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_LOW,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::LdacQualityIndex::QUALITY_ABR,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android

namespace android {
namespace hardware {
namespace details {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType, 4> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::UNKNOWN,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::MTU,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::BITRATE,
    ::vendor::qti::hardware::bluetooth_audio::V2_0::SessionParamType::SINK_LATENCY,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_VENDOR_QTI_HARDWARE_BLUETOOTH_AUDIO_V2_0_TYPES_H
