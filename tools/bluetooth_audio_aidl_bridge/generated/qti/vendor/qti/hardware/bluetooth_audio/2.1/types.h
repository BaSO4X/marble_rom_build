#ifndef HIDL_GENERATED_VENDOR_QTI_HARDWARE_BLUETOOTH_AUDIO_V2_1_TYPES_H
#define HIDL_GENERATED_VENDOR_QTI_HARDWARE_BLUETOOTH_AUDIO_V2_1_TYPES_H

#include <vendor/qti/hardware/bluetooth_audio/2.0/types.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace bluetooth_audio {
namespace V2_1 {

// Forward declaration for forward reference support:
enum class CodecType : uint32_t;
enum class ExtSampleRate : uint32_t;
enum class LC3ChannelMode : uint8_t;
struct lc3CodecConfig;
struct LC3Parameters;
struct CodecCapabilities;
union AudioCapabilities;
struct CodecConfiguration;
union AudioConfiguration;

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
    LC3 = 24u /* 0x18 */,
};

enum class ExtSampleRate : uint32_t {
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
    RATE_24000_2 = 256u /* 0x100 */,
    RATE_8000 = 512u /* 0x200 */,
};

enum class LC3ChannelMode : uint8_t {
    UNKNOWN = 0 /* 0x00 */,
    STEREO = 1 /* 0x01 */,
    JOINT_STEREO = 2 /* 0x02 */,
    DUAL = 4 /* 0x04 */,
    MONO = 8 /* 0x08 */,
    CH_5_1 = 16 /* 0x10 */,
};

/**
 * Used for Hardware Encoding LC3 codec parameters
 */
struct lc3CodecConfig final {
    ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate sampleRate __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode channelMode __attribute__ ((aligned(1)));
    uint16_t octetsPerFrame __attribute__ ((aligned(2)));
    uint16_t frameDuration __attribute__ ((aligned(2)));
    uint32_t bitrate __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_0::BitsPerSample bitsPerSample __attribute__ ((aligned(1)));
    uint8_t numBlocks __attribute__ ((aligned(1)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig, sampleRate) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig, channelMode) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig, octetsPerFrame) == 6, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig, frameDuration) == 8, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig, bitrate) == 12, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig, bitsPerSample) == 16, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig, numBlocks) == 17, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig) == 20, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig) == 4, "wrong alignment");

/**
 * Used for Hardware Encoding LC3 codec parameters
 */
struct LC3Parameters final {
    ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig txConfig __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig rxConfig __attribute__ ((aligned(4)));
    uint8_t rxConfigSet __attribute__ ((aligned(1)));
    uint16_t rxLatency __attribute__ ((aligned(2)));
    uint8_t decoderOuputChannels __attribute__ ((aligned(1)));
    uint8_t mode __attribute__ ((aligned(1)));
    ::android::hardware::hidl_array<uint8_t, 16> codecSpecific __attribute__ ((aligned(1)));
    uint8_t defaultQlevel __attribute__ ((aligned(1)));
    uint8_t NumStreamIDGroup __attribute__ ((aligned(1)));
    ::android::hardware::hidl_array<uint32_t, 48> streamMap __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters, txConfig) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters, rxConfig) == 20, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters, rxConfigSet) == 40, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters, rxLatency) == 42, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters, decoderOuputChannels) == 44, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters, mode) == 45, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters, codecSpecific) == 46, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters, defaultQlevel) == 62, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters, NumStreamIDGroup) == 63, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters, streamMap) == 64, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters) == 256, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters) == 4, "wrong alignment");

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
        ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters lc3Capabilities __attribute__ ((aligned(4)));
    };

    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities, sbcCapabilities) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities, aacCapabilities) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities, ldacCapabilities) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities, aptxCapabilities) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities, aptxAdaptiveCapabilities) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities, aptxTwsCapabilities) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities, baCeltCapabilities) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities, lc3Capabilities) == 0, "wrong offset");
    static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities) == 256, "wrong size");
    static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities) == 4, "wrong alignment");

    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType codecType __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities capabilities __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities, codecType) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities, capabilities) == 4, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities) == 260, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities) == 4, "wrong alignment");

/**
 * Used to specify the capabilities of the different session types.
 */
union AudioCapabilities final {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters pcmCapabilities __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities codecCapabilities __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::AudioCapabilities, pcmCapabilities) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::AudioCapabilities, codecCapabilities) == 0, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_1::AudioCapabilities) == 260, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_1::AudioCapabilities) == 4, "wrong alignment");

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
        ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters lc3Config __attribute__ ((aligned(4)));
    };

    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific, sbcConfig) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific, aacConfig) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific, ldacConfig) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific, aptxConfig) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific, aptxAdaptiveConfig) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific, aptxTwsConfig) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific, baCeltConfig) == 0, "wrong offset");
    static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific, lc3Config) == 0, "wrong offset");
    static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific) == 256, "wrong size");
    static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific) == 4, "wrong alignment");

    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType codecType __attribute__ ((aligned(4)));
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
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific config __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration, codecType) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration, encodedAudioBitrate) == 4, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration, peerMtu) == 8, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration, isScmstEnabled) == 10, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration, isScramblingEnabled) == 11, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration, config) == 12, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration) == 268, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration) == 4, "wrong alignment");

/**
 * Used to configure either a Hardware or Software Encoding session based on session type
 */
union AudioConfiguration final {
    ::vendor::qti::hardware::bluetooth_audio::V2_0::PcmParameters pcmConfig __attribute__ ((aligned(4)));
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration codecConfig __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::AudioConfiguration, pcmConfig) == 0, "wrong offset");
static_assert(offsetof(::vendor::qti::hardware::bluetooth_audio::V2_1::AudioConfiguration, codecConfig) == 0, "wrong offset");
static_assert(sizeof(::vendor::qti::hardware::bluetooth_audio::V2_1::AudioConfiguration) == 268, "wrong size");
static_assert(__alignof(::vendor::qti::hardware::bluetooth_audio::V2_1::AudioConfiguration) == 4, "wrong alignment");

//
// type declarations for package
//

template<typename>
static inline std::string toString(uint32_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType o, ::std::ostream* os);
constexpr uint32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const uint32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const uint32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}
constexpr uint32_t &operator|=(uint32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType e) {
    v |= static_cast<uint32_t>(e);
    return v;
}
constexpr uint32_t &operator&=(uint32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint32_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate o, ::std::ostream* os);
constexpr uint32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const uint32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const uint32_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}
constexpr uint32_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}
constexpr uint32_t &operator|=(uint32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate e) {
    v |= static_cast<uint32_t>(e);
    return v;
}
constexpr uint32_t &operator&=(uint32_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
static inline std::string toString(uint8_t o);
static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode o);
static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode o, ::std::ostream* os);
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator|(const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const uint8_t lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}
constexpr uint8_t operator&(const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}
constexpr uint8_t &operator|=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode e) {
    v |= static_cast<uint8_t>(e);
    return v;
}
constexpr uint8_t &operator&=(uint8_t& v, const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig& rhs);

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters& o, ::std::ostream*);
static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters& rhs);
static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters& rhs);

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities& o, ::std::ostream*);
// operator== and operator!= are not generated for Capabilities

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities& o, ::std::ostream*);
// operator== and operator!= are not generated for CodecCapabilities

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::AudioCapabilities& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::AudioCapabilities& o, ::std::ostream*);
// operator== and operator!= are not generated for AudioCapabilities

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific& o, ::std::ostream*);
// operator== and operator!= are not generated for CodecSpecific

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration& o, ::std::ostream*);
// operator== and operator!= are not generated for CodecConfiguration

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::AudioConfiguration& o);
static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::AudioConfiguration& o, ::std::ostream*);
// operator== and operator!= are not generated for AudioConfiguration

//
// type header definitions for package
//

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType>(uint32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::UNKNOWN) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::SBC) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::SBC)) {
        os += (first ? "" : " | ");
        os += "SBC";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::SBC;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::AAC) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::AAC)) {
        os += (first ? "" : " | ");
        os += "AAC";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::AAC;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX)) {
        os += (first ? "" : " | ");
        os += "APTX";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_HD) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_HD)) {
        os += (first ? "" : " | ");
        os += "APTX_HD";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_HD;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::LDAC) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::LDAC)) {
        os += (first ? "" : " | ");
        os += "LDAC";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::LDAC;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::BA_CELT) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::BA_CELT)) {
        os += (first ? "" : " | ");
        os += "BA_CELT";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::BA_CELT;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_ADAPTIVE) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_ADAPTIVE)) {
        os += (first ? "" : " | ");
        os += "APTX_ADAPTIVE";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_ADAPTIVE;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_TWS) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_TWS)) {
        os += (first ? "" : " | ");
        os += "APTX_TWS";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_TWS;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::LC3) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::LC3)) {
        os += (first ? "" : " | ");
        os += "LC3";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::LC3;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::UNKNOWN) {
        return "UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::SBC) {
        return "SBC";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::AAC) {
        return "AAC";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX) {
        return "APTX";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_HD) {
        return "APTX_HD";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::LDAC) {
        return "LDAC";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::BA_CELT) {
        return "BA_CELT";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_ADAPTIVE) {
        return "APTX_ADAPTIVE";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_TWS) {
        return "APTX_TWS";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::LC3) {
        return "LC3";
    }
    std::string os;
    os += toHexString(static_cast<uint32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate>(uint32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_UNKNOWN) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "RATE_UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_44100) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_44100)) {
        os += (first ? "" : " | ");
        os += "RATE_44100";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_44100;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_48000) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_48000)) {
        os += (first ? "" : " | ");
        os += "RATE_48000";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_48000;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_88200) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_88200)) {
        os += (first ? "" : " | ");
        os += "RATE_88200";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_88200;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_96000) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_96000)) {
        os += (first ? "" : " | ");
        os += "RATE_96000";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_96000;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_176400) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_176400)) {
        os += (first ? "" : " | ");
        os += "RATE_176400";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_176400;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_192000) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_192000)) {
        os += (first ? "" : " | ");
        os += "RATE_192000";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_192000;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_16000) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_16000)) {
        os += (first ? "" : " | ");
        os += "RATE_16000";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_16000;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_24000) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_24000)) {
        os += (first ? "" : " | ");
        os += "RATE_24000";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_24000;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_32000) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_32000)) {
        os += (first ? "" : " | ");
        os += "RATE_32000";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_32000;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_24000_2) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_24000_2)) {
        os += (first ? "" : " | ");
        os += "RATE_24000_2";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_24000_2;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_8000) == static_cast<uint32_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_8000)) {
        os += (first ? "" : " | ");
        os += "RATE_8000";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_8000;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_UNKNOWN) {
        return "RATE_UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_44100) {
        return "RATE_44100";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_48000) {
        return "RATE_48000";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_88200) {
        return "RATE_88200";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_96000) {
        return "RATE_96000";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_176400) {
        return "RATE_176400";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_192000) {
        return "RATE_192000";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_16000) {
        return "RATE_16000";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_24000) {
        return "RATE_24000";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_32000) {
        return "RATE_32000";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_24000_2) {
        return "RATE_24000_2";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_8000) {
        return "RATE_8000";
    }
    std::string os;
    os += toHexString(static_cast<uint32_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate o, ::std::ostream* os) {
    *os << toString(o);
}

template<>
inline std::string toString<::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode>(uint8_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode> flipped = 0;
    bool first = true;
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::UNKNOWN) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::UNKNOWN)) {
        os += (first ? "" : " | ");
        os += "UNKNOWN";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::UNKNOWN;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::STEREO) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::STEREO)) {
        os += (first ? "" : " | ");
        os += "STEREO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::STEREO;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::JOINT_STEREO) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::JOINT_STEREO)) {
        os += (first ? "" : " | ");
        os += "JOINT_STEREO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::JOINT_STEREO;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::DUAL) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::DUAL)) {
        os += (first ? "" : " | ");
        os += "DUAL";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::DUAL;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::MONO) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::MONO)) {
        os += (first ? "" : " | ");
        os += "MONO";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::MONO;
    }
    if ((o & ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::CH_5_1) == static_cast<uint8_t>(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::CH_5_1)) {
        os += (first ? "" : " | ");
        os += "CH_5_1";
        first = false;
        flipped |= ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::CH_5_1;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::UNKNOWN) {
        return "UNKNOWN";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::STEREO) {
        return "STEREO";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::JOINT_STEREO) {
        return "JOINT_STEREO";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::DUAL) {
        return "DUAL";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::MONO) {
        return "MONO";
    }
    if (o == ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::CH_5_1) {
        return "CH_5_1";
    }
    std::string os;
    os += toHexString(static_cast<uint8_t>(o));
    return os;
}

static inline void PrintTo(::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode o, ::std::ostream* os) {
    *os << toString(o);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".sampleRate = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_1::toString(o.sampleRate);
    os += ", .channelMode = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_1::toString(o.channelMode);
    os += ", .octetsPerFrame = ";
    os += ::android::hardware::toString(o.octetsPerFrame);
    os += ", .frameDuration = ";
    os += ::android::hardware::toString(o.frameDuration);
    os += ", .bitrate = ";
    os += ::android::hardware::toString(o.bitrate);
    os += ", .bitsPerSample = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.bitsPerSample);
    os += ", .numBlocks = ";
    os += ::android::hardware::toString(o.numBlocks);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig& rhs) {
    if (lhs.sampleRate != rhs.sampleRate) {
        return false;
    }
    if (lhs.channelMode != rhs.channelMode) {
        return false;
    }
    if (lhs.octetsPerFrame != rhs.octetsPerFrame) {
        return false;
    }
    if (lhs.frameDuration != rhs.frameDuration) {
        return false;
    }
    if (lhs.bitrate != rhs.bitrate) {
        return false;
    }
    if (lhs.bitsPerSample != rhs.bitsPerSample) {
        return false;
    }
    if (lhs.numBlocks != rhs.numBlocks) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::lc3CodecConfig& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".txConfig = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_1::toString(o.txConfig);
    os += ", .rxConfig = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_1::toString(o.rxConfig);
    os += ", .rxConfigSet = ";
    os += ::android::hardware::toString(o.rxConfigSet);
    os += ", .rxLatency = ";
    os += ::android::hardware::toString(o.rxLatency);
    os += ", .decoderOuputChannels = ";
    os += ::android::hardware::toString(o.decoderOuputChannels);
    os += ", .mode = ";
    os += ::android::hardware::toString(o.mode);
    os += ", .codecSpecific = ";
    os += ::android::hardware::toString(o.codecSpecific);
    os += ", .defaultQlevel = ";
    os += ::android::hardware::toString(o.defaultQlevel);
    os += ", .NumStreamIDGroup = ";
    os += ::android::hardware::toString(o.NumStreamIDGroup);
    os += ", .streamMap = ";
    os += ::android::hardware::toString(o.streamMap);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters& o, ::std::ostream* os) {
    *os << toString(o);
}

static inline bool operator==(const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters& rhs) {
    if (lhs.txConfig != rhs.txConfig) {
        return false;
    }
    if (lhs.rxConfig != rhs.rxConfig) {
        return false;
    }
    if (lhs.rxConfigSet != rhs.rxConfigSet) {
        return false;
    }
    if (lhs.rxLatency != rhs.rxLatency) {
        return false;
    }
    if (lhs.decoderOuputChannels != rhs.decoderOuputChannels) {
        return false;
    }
    if (lhs.mode != rhs.mode) {
        return false;
    }
    if (lhs.codecSpecific != rhs.codecSpecific) {
        return false;
    }
    if (lhs.defaultQlevel != rhs.defaultQlevel) {
        return false;
    }
    if (lhs.NumStreamIDGroup != rhs.NumStreamIDGroup) {
        return false;
    }
    if (lhs.streamMap != rhs.streamMap) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters& lhs, const ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3Parameters& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities& o) {
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
    os += ", .lc3Capabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_1::toString(o.lc3Capabilities);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities::Capabilities& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for Capabilities

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".codecType = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_1::toString(o.codecType);
    os += ", .capabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_1::toString(o.capabilities);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecCapabilities& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for CodecCapabilities

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::AudioCapabilities& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".pcmCapabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.pcmCapabilities);
    os += ", .codecCapabilities = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_1::toString(o.codecCapabilities);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::AudioCapabilities& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for AudioCapabilities

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific& o) {
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
    os += ", .lc3Config = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_1::toString(o.lc3Config);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration::CodecSpecific& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for CodecSpecific

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".codecType = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_1::toString(o.codecType);
    os += ", .encodedAudioBitrate = ";
    os += ::android::hardware::toString(o.encodedAudioBitrate);
    os += ", .peerMtu = ";
    os += ::android::hardware::toString(o.peerMtu);
    os += ", .isScmstEnabled = ";
    os += ::android::hardware::toString(o.isScmstEnabled);
    os += ", .isScramblingEnabled = ";
    os += ::android::hardware::toString(o.isScramblingEnabled);
    os += ", .config = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_1::toString(o.config);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecConfiguration& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for CodecConfiguration

static inline std::string toString(const ::vendor::qti::hardware::bluetooth_audio::V2_1::AudioConfiguration& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".pcmConfig = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_0::toString(o.pcmConfig);
    os += ", .codecConfig = ";
    os += ::vendor::qti::hardware::bluetooth_audio::V2_1::toString(o.codecConfig);
    os += "}"; return os;
}

static inline void PrintTo(const ::vendor::qti::hardware::bluetooth_audio::V2_1::AudioConfiguration& o, ::std::ostream* os) {
    *os << toString(o);
}

// operator== and operator!= are not generated for AudioConfiguration


}  // namespace V2_1
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
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType, 10> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::UNKNOWN,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::SBC,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::AAC,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_HD,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::LDAC,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::BA_CELT,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_ADAPTIVE,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::APTX_TWS,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::CodecType::LC3,
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
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate, 12> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_UNKNOWN,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_44100,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_48000,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_88200,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_96000,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_176400,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_192000,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_16000,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_24000,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_32000,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_24000_2,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::ExtSampleRate::RATE_8000,
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
template<> inline constexpr std::array<::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode, 6> hidl_enum_values<::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode> = {
    ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::UNKNOWN,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::STEREO,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::JOINT_STEREO,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::DUAL,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::MONO,
    ::vendor::qti::hardware::bluetooth_audio::V2_1::LC3ChannelMode::CH_5_1,
};
#pragma clang diagnostic pop
}  // namespace details
}  // namespace hardware
}  // namespace android


#endif  // HIDL_GENERATED_VENDOR_QTI_HARDWARE_BLUETOOTH_AUDIO_V2_1_TYPES_H
