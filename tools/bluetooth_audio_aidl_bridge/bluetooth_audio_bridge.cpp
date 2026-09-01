/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Android Bluetooth Audio AIDL V3 compatibility bridge for legacy Bluetooth
 * Audio HIDL vendors. The standard HIDL backend provides software sessions;
 * the Qualcomm HIDL backend is optional and provides A2DP offload when present.
 */

#define LOG_TAG "bt-audio-aidl-bridge"

#include <aidl/android/hardware/bluetooth/audio/AacCapabilities.h>
#include <aidl/android/hardware/bluetooth/audio/AacConfiguration.h>
#include <aidl/android/hardware/bluetooth/audio/AudioCapabilities.h>
#include <aidl/android/hardware/bluetooth/audio/AudioConfiguration.h>
#include <aidl/android/hardware/bluetooth/audio/BnBluetoothAudioPort.h>
#include <aidl/android/hardware/bluetooth/audio/BnBluetoothAudioProvider.h>
#include <aidl/android/hardware/bluetooth/audio/BnBluetoothAudioProviderFactory.h>
#include <aidl/android/hardware/bluetooth/audio/CodecCapabilities.h>
#include <aidl/android/hardware/bluetooth/audio/CodecConfiguration.h>
#include <aidl/android/hardware/bluetooth/audio/PresentationPosition.h>
#include <aidl/android/hardware/bluetooth/audio/SbcCapabilities.h>
#include <aidl/android/hardware/bluetooth/audio/SbcConfiguration.h>
#include <android/binder_ibinder.h>
#include <android/log.h>
#include <android/hardware/bluetooth/audio/2.0/IBluetoothAudioPort.h>
#include <android/hardware/bluetooth/audio/2.0/IBluetoothAudioProvider.h>
#include <android/hardware/bluetooth/audio/2.0/IBluetoothAudioProvidersFactory.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <fmq/AidlMessageQueue.h>
#include <hidl/HidlTransportSupport.h>
#include <vendor/qti/hardware/bluetooth_audio/2.1/IBluetoothAudioPort.h>
#include <vendor/qti/hardware/bluetooth_audio/2.1/IBluetoothAudioProvider.h>
#include <vendor/qti/hardware/bluetooth_audio/2.1/IBluetoothAudioProvidersFactory.h>

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <unistd.h>

namespace {

namespace Aidl = ::aidl::android::hardware::bluetooth::audio;
namespace Aosp20 = ::android::hardware::bluetooth::audio::V2_0;
namespace AudioCommon50 = ::android::hardware::audio::common::V5_0;
namespace Qti20 = ::vendor::qti::hardware::bluetooth_audio::V2_0;
namespace Qti21 = ::vendor::qti::hardware::bluetooth_audio::V2_1;

using DataMqDescriptor =
        ::aidl::android::hardware::common::fmq::MQDescriptor<
                int8_t,
                ::aidl::android::hardware::common::fmq::SynchronizedReadWrite>;

constexpr char kServiceName[] =
        "android.hardware.bluetooth.audio.IBluetoothAudioProviderFactory/default";
constexpr auto kA2dpOffloadSession =
        Aidl::SessionType::A2DP_HARDWARE_OFFLOAD_ENCODING_DATAPATH;
constexpr auto kA2dpSoftwareSession =
        Aidl::SessionType::A2DP_SOFTWARE_ENCODING_DATAPATH;
constexpr auto kHearingAidSoftwareSession =
        Aidl::SessionType::HEARING_AID_SOFTWARE_ENCODING_DATAPATH;
constexpr auto kQtiA2dpOffloadSession =
        Qti20::SessionType::A2DP_HARDWARE_OFFLOAD_DATAPATH;
constexpr auto kQtiA2dpSoftwareSession =
        Qti20::SessionType::A2DP_SOFTWARE_ENCODING_DATAPATH;
constexpr auto kQtiHearingAidSoftwareSession =
        Qti20::SessionType::HEARING_AID_SOFTWARE_ENCODING_DATAPATH;
constexpr uint32_t kMaxEncodedAudioBitrate = 0x00ffffff;

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

template <typename Enum>
constexpr auto enumValue(Enum value) {
    return static_cast<std::underlying_type_t<Enum>>(value);
}

template <typename Enum>
bool hasFlag(Enum mask, Enum flag) {
    const auto flag_value = enumValue(flag);
    return flag_value != 0 && (enumValue(mask) & flag_value) == flag_value;
}

template <typename Enum, typename Value>
void appendFlag(Enum mask, Enum flag, Value value, std::vector<Value>* output) {
    if (hasFlag(mask, flag)) {
        output->push_back(value);
    }
}

void appendSampleRates(Qti20::SampleRate mask, std::vector<int32_t>* output) {
    // RATE_24000 is 0x60 in the frozen QTI interface, which overlaps the
    // RATE_16000 (0x40) and RATE_192000 (0x20) bits.  Only advertise the two
    // unambiguous rates needed by the first-stage SBC/AAC bridge.
    appendFlag(mask, Qti20::SampleRate::RATE_44100, 44100, output);
    appendFlag(mask, Qti20::SampleRate::RATE_48000, 48000, output);
}

void appendBitsPerSample(Qti20::BitsPerSample mask,
                         std::vector<uint8_t>* output) {
    appendFlag(mask, Qti20::BitsPerSample::BITS_16, uint8_t{16}, output);
    appendFlag(mask, Qti20::BitsPerSample::BITS_24, uint8_t{24}, output);
    appendFlag(mask, Qti20::BitsPerSample::BITS_32, uint8_t{32}, output);
}

bool convertPcmCapabilities(const Qti20::PcmParameters& input,
                            Aidl::PcmCapabilities* output) {
    appendSampleRates(input.sampleRate, &output->sampleRateHz);
    appendFlag(input.channelMode, Qti20::ChannelMode::MONO,
               Aidl::ChannelMode::MONO, &output->channelMode);
    appendFlag(input.channelMode, Qti20::ChannelMode::STEREO,
               Aidl::ChannelMode::STEREO, &output->channelMode);
    appendBitsPerSample(input.bitsPerSample, &output->bitsPerSample);
    // QTI HIDL 2.1 predates the AIDL interval field.  These cover the normal
    // 10 ms low-latency and 20 ms A2DP software encoder periods; startSession
    // accepts any positive interval because the QTI backend does not consume it.
    output->dataIntervalUs = {10000, 20000};
    return !output->sampleRateHz.empty() && !output->channelMode.empty() &&
            !output->bitsPerSample.empty();
}

bool convertStandardPcmCapabilities(const Aosp20::PcmParameters& input,
                                    Aidl::PcmCapabilities* output) {
    appendFlag(input.sampleRate, Aosp20::SampleRate::RATE_16000, 16000,
               &output->sampleRateHz);
    appendFlag(input.sampleRate, Aosp20::SampleRate::RATE_24000, 24000,
               &output->sampleRateHz);
    appendFlag(input.sampleRate, Aosp20::SampleRate::RATE_44100, 44100,
               &output->sampleRateHz);
    appendFlag(input.sampleRate, Aosp20::SampleRate::RATE_48000, 48000,
               &output->sampleRateHz);
    appendFlag(input.sampleRate, Aosp20::SampleRate::RATE_88200, 88200,
               &output->sampleRateHz);
    appendFlag(input.sampleRate, Aosp20::SampleRate::RATE_96000, 96000,
               &output->sampleRateHz);
    appendFlag(input.sampleRate, Aosp20::SampleRate::RATE_176400, 176400,
               &output->sampleRateHz);
    appendFlag(input.sampleRate, Aosp20::SampleRate::RATE_192000, 192000,
               &output->sampleRateHz);
    appendFlag(input.channelMode, Aosp20::ChannelMode::MONO,
               Aidl::ChannelMode::MONO, &output->channelMode);
    appendFlag(input.channelMode, Aosp20::ChannelMode::STEREO,
               Aidl::ChannelMode::STEREO, &output->channelMode);
    appendFlag(input.bitsPerSample, Aosp20::BitsPerSample::BITS_16,
               uint8_t{16}, &output->bitsPerSample);
    appendFlag(input.bitsPerSample, Aosp20::BitsPerSample::BITS_24,
               uint8_t{24}, &output->bitsPerSample);
    appendFlag(input.bitsPerSample, Aosp20::BitsPerSample::BITS_32,
               uint8_t{32}, &output->bitsPerSample);
    // HIDL 2.0 does not expose a PCM interval.  The bridge accepts every
    // positive interval at startSession; advertise the periods used by A2DP
    // and Hearing Aid clients so the AIDL stack can select a configuration.
    output->dataIntervalUs = {10000, 20000};
    return !output->sampleRateHz.empty() && !output->channelMode.empty() &&
            !output->bitsPerSample.empty();
}

bool convertSbcCapabilities(const Qti20::SbcParameters& input,
                            Aidl::SbcCapabilities* output) {
    appendSampleRates(input.sampleRate, &output->sampleRateHz);
    appendFlag(input.channelMode, Qti20::SbcChannelMode::JOINT_STEREO,
               Aidl::SbcChannelMode::JOINT_STEREO, &output->channelMode);
    appendFlag(input.channelMode, Qti20::SbcChannelMode::STEREO,
               Aidl::SbcChannelMode::STEREO, &output->channelMode);
    appendFlag(input.channelMode, Qti20::SbcChannelMode::DUAL,
               Aidl::SbcChannelMode::DUAL, &output->channelMode);
    appendFlag(input.channelMode, Qti20::SbcChannelMode::MONO,
               Aidl::SbcChannelMode::MONO, &output->channelMode);
    appendFlag(input.blockLength, Qti20::SbcBlockLength::BLOCKS_4,
               uint8_t{4}, &output->blockLength);
    appendFlag(input.blockLength, Qti20::SbcBlockLength::BLOCKS_8,
               uint8_t{8}, &output->blockLength);
    appendFlag(input.blockLength, Qti20::SbcBlockLength::BLOCKS_12,
               uint8_t{12}, &output->blockLength);
    appendFlag(input.blockLength, Qti20::SbcBlockLength::BLOCKS_16,
               uint8_t{16}, &output->blockLength);
    appendFlag(input.numSubbands, Qti20::SbcNumSubbands::SUBBAND_4,
               uint8_t{4}, &output->numSubbands);
    appendFlag(input.numSubbands, Qti20::SbcNumSubbands::SUBBAND_8,
               uint8_t{8}, &output->numSubbands);
    appendFlag(input.allocMethod, Qti20::SbcAllocMethod::ALLOC_MD_S,
               Aidl::SbcAllocMethod::ALLOC_MD_S, &output->allocMethod);
    appendFlag(input.allocMethod, Qti20::SbcAllocMethod::ALLOC_MD_L,
               Aidl::SbcAllocMethod::ALLOC_MD_L, &output->allocMethod);
    appendBitsPerSample(input.bitsPerSample, &output->bitsPerSample);
    output->minBitpool = input.minBitpool;
    output->maxBitpool = input.maxBitpool;
    return !output->sampleRateHz.empty() && !output->channelMode.empty() &&
           !output->blockLength.empty() && !output->numSubbands.empty() &&
           !output->allocMethod.empty() && !output->bitsPerSample.empty();
}

bool convertAacCapabilities(const Qti20::AacParameters& input,
                            Aidl::AacCapabilities* output) {
    appendFlag(input.objectType, Qti20::AacObjectType::MPEG2_LC,
               Aidl::AacObjectType::MPEG2_LC, &output->objectType);
    appendFlag(input.objectType, Qti20::AacObjectType::MPEG4_LC,
               Aidl::AacObjectType::MPEG4_LC, &output->objectType);
    appendFlag(input.objectType, Qti20::AacObjectType::MPEG4_LTP,
               Aidl::AacObjectType::MPEG4_LTP, &output->objectType);
    appendFlag(input.objectType, Qti20::AacObjectType::MPEG4_SCALABLE,
               Aidl::AacObjectType::MPEG4_SCALABLE, &output->objectType);
    appendSampleRates(input.sampleRate, &output->sampleRateHz);
    appendFlag(input.channelMode, Qti20::ChannelMode::MONO,
               Aidl::ChannelMode::MONO, &output->channelMode);
    appendFlag(input.channelMode, Qti20::ChannelMode::STEREO,
               Aidl::ChannelMode::STEREO, &output->channelMode);
    output->variableBitRateSupported =
            hasFlag(input.variableBitRateEnabled,
                    Qti20::AacVariableBitRate::ENABLED);
    appendBitsPerSample(input.bitsPerSample, &output->bitsPerSample);
    output->adaptiveBitRateSupported = input.frameControlEnabled;
    return !output->objectType.empty() && !output->sampleRateHz.empty() &&
           !output->channelMode.empty() && !output->bitsPerSample.empty();
}

std::optional<Aidl::AudioCapabilities> convertCapabilities(
        const Qti21::AudioCapabilities& input) {
    const auto& codec = input.codecCapabilities;
    Aidl::CodecCapabilities output;

    switch (codec.codecType) {
        case Qti21::CodecType::SBC: {
            Aidl::SbcCapabilities capabilities;
            if (!convertSbcCapabilities(codec.capabilities.sbcCapabilities,
                                        &capabilities)) {
                return std::nullopt;
            }
            output.codecType = Aidl::CodecType::SBC;
            output.capabilities =
                    Aidl::CodecCapabilities::Capabilities::make<
                            Aidl::CodecCapabilities::Capabilities::
                                    sbcCapabilities>(std::move(capabilities));
            break;
        }
        case Qti21::CodecType::AAC: {
            Aidl::AacCapabilities capabilities;
            if (!convertAacCapabilities(codec.capabilities.aacCapabilities,
                                        &capabilities)) {
                return std::nullopt;
            }
            output.codecType = Aidl::CodecType::AAC;
            output.capabilities =
                    Aidl::CodecCapabilities::Capabilities::make<
                            Aidl::CodecCapabilities::Capabilities::
                                    aacCapabilities>(std::move(capabilities));
            break;
        }
        default:
            return std::nullopt;
    }

    return Aidl::AudioCapabilities::make<
            Aidl::AudioCapabilities::a2dpCapabilities>(std::move(output));
}

bool convertSampleRate(int32_t input, Qti20::SampleRate* output) {
    switch (input) {
        case 16000: *output = Qti20::SampleRate::RATE_16000; return true;
        case 24000: *output = Qti20::SampleRate::RATE_24000; return true;
        case 32000: *output = Qti20::SampleRate::RATE_32000; return true;
        case 44100: *output = Qti20::SampleRate::RATE_44100; return true;
        case 48000: *output = Qti20::SampleRate::RATE_48000; return true;
        case 88200: *output = Qti20::SampleRate::RATE_88200; return true;
        case 96000: *output = Qti20::SampleRate::RATE_96000; return true;
        case 176400: *output = Qti20::SampleRate::RATE_176400; return true;
        case 192000: *output = Qti20::SampleRate::RATE_192000; return true;
        default: return false;
    }
}

bool convertBitsPerSample(int8_t input, Qti20::BitsPerSample* output) {
    switch (input) {
        case 16: *output = Qti20::BitsPerSample::BITS_16; return true;
        case 24: *output = Qti20::BitsPerSample::BITS_24; return true;
        case 32: *output = Qti20::BitsPerSample::BITS_32; return true;
        default: return false;
    }
}

bool convertPcmConfiguration(const Aidl::AudioConfiguration& input,
                             Qti21::AudioConfiguration* output,
                             Aidl::PcmConfiguration* aidl_pcm) {
    if (input.getTag() != Aidl::AudioConfiguration::pcmConfig) {
        return false;
    }
    const auto& pcm = input.get<Aidl::AudioConfiguration::pcmConfig>();
    if (pcm.dataIntervalUs <= 0 ||
        !convertSampleRate(pcm.sampleRateHz, &output->pcmConfig.sampleRate) ||
        !convertBitsPerSample(pcm.bitsPerSample,
                              &output->pcmConfig.bitsPerSample)) {
        return false;
    }
    switch (pcm.channelMode) {
        case Aidl::ChannelMode::MONO:
            output->pcmConfig.channelMode = Qti20::ChannelMode::MONO;
            break;
        case Aidl::ChannelMode::STEREO:
            output->pcmConfig.channelMode = Qti20::ChannelMode::STEREO;
            break;
        default:
            return false;
    }
    *aidl_pcm = pcm;
    return true;
}

bool convertStandardPcmConfiguration(
        const Aidl::AudioConfiguration& input,
        Aosp20::AudioConfiguration* output,
        Aidl::PcmConfiguration* aidl_pcm) {
    if (input.getTag() != Aidl::AudioConfiguration::pcmConfig) {
        return false;
    }
    const auto& pcm = input.get<Aidl::AudioConfiguration::pcmConfig>();
    if (pcm.dataIntervalUs <= 0) {
        return false;
    }

    Aosp20::PcmParameters converted{};
    switch (pcm.sampleRateHz) {
        case 16000:
            converted.sampleRate = Aosp20::SampleRate::RATE_16000;
            break;
        case 24000:
            converted.sampleRate = Aosp20::SampleRate::RATE_24000;
            break;
        case 44100:
            converted.sampleRate = Aosp20::SampleRate::RATE_44100;
            break;
        case 48000:
            converted.sampleRate = Aosp20::SampleRate::RATE_48000;
            break;
        case 88200:
            converted.sampleRate = Aosp20::SampleRate::RATE_88200;
            break;
        case 96000:
            converted.sampleRate = Aosp20::SampleRate::RATE_96000;
            break;
        case 176400:
            converted.sampleRate = Aosp20::SampleRate::RATE_176400;
            break;
        case 192000:
            converted.sampleRate = Aosp20::SampleRate::RATE_192000;
            break;
        default:
            return false;
    }
    switch (pcm.channelMode) {
        case Aidl::ChannelMode::MONO:
            converted.channelMode = Aosp20::ChannelMode::MONO;
            break;
        case Aidl::ChannelMode::STEREO:
            converted.channelMode = Aosp20::ChannelMode::STEREO;
            break;
        default:
            return false;
    }
    switch (pcm.bitsPerSample) {
        case 16:
            converted.bitsPerSample = Aosp20::BitsPerSample::BITS_16;
            break;
        case 24:
            converted.bitsPerSample = Aosp20::BitsPerSample::BITS_24;
            break;
        case 32:
            converted.bitsPerSample = Aosp20::BitsPerSample::BITS_32;
            break;
        default:
            return false;
    }
    output->pcmConfig(std::move(converted));
    *aidl_pcm = pcm;
    return true;
}

bool convertSbcConfiguration(const Aidl::SbcConfiguration& input,
                             Qti20::SbcParameters* output) {
    if (!convertSampleRate(input.sampleRateHz, &output->sampleRate) ||
        !convertBitsPerSample(input.bitsPerSample, &output->bitsPerSample) ||
        input.minBitpool < 0 || input.minBitpool > UINT8_MAX ||
        input.maxBitpool < input.minBitpool || input.maxBitpool > UINT8_MAX) {
        return false;
    }
    switch (input.channelMode) {
        case Aidl::SbcChannelMode::JOINT_STEREO:
            output->channelMode = Qti20::SbcChannelMode::JOINT_STEREO;
            break;
        case Aidl::SbcChannelMode::STEREO:
            output->channelMode = Qti20::SbcChannelMode::STEREO;
            break;
        case Aidl::SbcChannelMode::DUAL:
            output->channelMode = Qti20::SbcChannelMode::DUAL;
            break;
        case Aidl::SbcChannelMode::MONO:
            output->channelMode = Qti20::SbcChannelMode::MONO;
            break;
        default:
            return false;
    }
    switch (input.blockLength) {
        case 4: output->blockLength = Qti20::SbcBlockLength::BLOCKS_4; break;
        case 8: output->blockLength = Qti20::SbcBlockLength::BLOCKS_8; break;
        case 12: output->blockLength = Qti20::SbcBlockLength::BLOCKS_12; break;
        case 16: output->blockLength = Qti20::SbcBlockLength::BLOCKS_16; break;
        default: return false;
    }
    switch (input.numSubbands) {
        case 4: output->numSubbands = Qti20::SbcNumSubbands::SUBBAND_4; break;
        case 8: output->numSubbands = Qti20::SbcNumSubbands::SUBBAND_8; break;
        default: return false;
    }
    switch (input.allocMethod) {
        case Aidl::SbcAllocMethod::ALLOC_MD_S:
            output->allocMethod = Qti20::SbcAllocMethod::ALLOC_MD_S;
            break;
        case Aidl::SbcAllocMethod::ALLOC_MD_L:
            output->allocMethod = Qti20::SbcAllocMethod::ALLOC_MD_L;
            break;
        default:
            return false;
    }
    output->minBitpool = static_cast<uint8_t>(input.minBitpool);
    output->maxBitpool = static_cast<uint8_t>(input.maxBitpool);
    return true;
}

bool convertAacConfiguration(const Aidl::AacConfiguration& input,
                             Qti20::AacParameters* output) {
    if (!convertSampleRate(input.sampleRateHz, &output->sampleRate) ||
        !convertBitsPerSample(input.bitsPerSample, &output->bitsPerSample)) {
        return false;
    }
    switch (input.objectType) {
        case Aidl::AacObjectType::MPEG2_LC:
            output->objectType = Qti20::AacObjectType::MPEG2_LC;
            break;
        case Aidl::AacObjectType::MPEG4_LC:
            output->objectType = Qti20::AacObjectType::MPEG4_LC;
            break;
        case Aidl::AacObjectType::MPEG4_LTP:
            output->objectType = Qti20::AacObjectType::MPEG4_LTP;
            break;
        case Aidl::AacObjectType::MPEG4_SCALABLE:
            output->objectType = Qti20::AacObjectType::MPEG4_SCALABLE;
            break;
        default:
            return false;
    }
    switch (input.channelMode) {
        case Aidl::ChannelMode::MONO:
            output->channelMode = Qti20::ChannelMode::MONO;
            break;
        case Aidl::ChannelMode::STEREO:
            output->channelMode = Qti20::ChannelMode::STEREO;
            break;
        default:
            return false;
    }
    output->variableBitRateEnabled = input.variableBitRateEnabled
            ? Qti20::AacVariableBitRate::ENABLED
            : Qti20::AacVariableBitRate::DISABLED;
    output->frameControlEnabled = input.adaptiveBitRateSupported;
    return true;
}

bool convertConfiguration(const Aidl::AudioConfiguration& input,
                          Qti21::AudioConfiguration* output,
                          Aidl::CodecConfiguration* aidl_codec) {
    if (input.getTag() != Aidl::AudioConfiguration::a2dpConfig) {
        return false;
    }
    const auto& codec = input.get<Aidl::AudioConfiguration::a2dpConfig>();
    if (codec.encodedAudioBitrate < 0 ||
        codec.encodedAudioBitrate >
                static_cast<int32_t>(kMaxEncodedAudioBitrate) ||
        codec.peerMtu < 0 ||
        codec.peerMtu > UINT16_MAX) {
        return false;
    }

    auto& qti_codec = output->codecConfig;
    qti_codec.encodedAudioBitrate =
            static_cast<uint32_t>(codec.encodedAudioBitrate);
    qti_codec.peerMtu = static_cast<uint16_t>(codec.peerMtu);
    qti_codec.isScmstEnabled = codec.isScmstEnabled;
    qti_codec.isScramblingEnabled = false;

    switch (codec.codecType) {
        case Aidl::CodecType::SBC:
            if (codec.config.getTag() !=
                        Aidl::CodecConfiguration::CodecSpecific::sbcConfig ||
                !convertSbcConfiguration(
                        codec.config.get<
                                Aidl::CodecConfiguration::CodecSpecific::
                                        sbcConfig>(),
                        &qti_codec.config.sbcConfig)) {
                return false;
            }
            qti_codec.codecType = Qti21::CodecType::SBC;
            break;
        case Aidl::CodecType::AAC:
            if (codec.config.getTag() !=
                        Aidl::CodecConfiguration::CodecSpecific::aacConfig ||
                !convertAacConfiguration(
                        codec.config.get<
                                Aidl::CodecConfiguration::CodecSpecific::
                                        aacConfig>(),
                        &qti_codec.config.aacConfig)) {
                return false;
            }
            qti_codec.codecType = Qti21::CodecType::AAC;
            break;
        default:
            return false;
    }
    *aidl_codec = codec;
    return true;
}

Qti20::Status toQtiStatus(Aidl::BluetoothAudioStatus status) {
    switch (status) {
        case Aidl::BluetoothAudioStatus::SUCCESS:
            return Qti20::Status::SUCCESS;
        case Aidl::BluetoothAudioStatus::UNSUPPORTED_CODEC_CONFIGURATION:
            return Qti20::Status::UNSUPPORTED_CODEC_CONFIGURATION;
        default:
            return Qti20::Status::FAILURE;
    }
}

Aosp20::Status toAospStatus(Aidl::BluetoothAudioStatus status) {
    switch (status) {
        case Aidl::BluetoothAudioStatus::SUCCESS:
            return Aosp20::Status::SUCCESS;
        case Aidl::BluetoothAudioStatus::UNSUPPORTED_CODEC_CONFIGURATION:
            return Aosp20::Status::UNSUPPORTED_CODEC_CONFIGURATION;
        default:
            return Aosp20::Status::FAILURE;
    }
}

std::optional<Aosp20::SessionType> toAospSessionType(
        Aidl::SessionType session_type) {
    switch (session_type) {
        case kA2dpSoftwareSession:
            return Aosp20::SessionType::A2DP_SOFTWARE_ENCODING_DATAPATH;
        case kHearingAidSoftwareSession:
            return Aosp20::SessionType::HEARING_AID_SOFTWARE_ENCODING_DATAPATH;
        default:
            return std::nullopt;
    }
}

std::optional<Qti20::SessionType> toQtiSessionType(
        Aidl::SessionType session_type) {
    switch (session_type) {
        case kA2dpSoftwareSession:
            return kQtiA2dpSoftwareSession;
        case kA2dpOffloadSession:
            return kQtiA2dpOffloadSession;
        case kHearingAidSoftwareSession:
            return kQtiHearingAidSoftwareSession;
        default:
            return std::nullopt;
    }
}

bool isPcmSession(Aidl::SessionType session_type) {
    return session_type == kA2dpSoftwareSession ||
            session_type == kHearingAidSoftwareSession;
}

bool convertDataMqDescriptor(
        const ::android::hardware::MQDescriptorSync<uint8_t>& input,
        DataMqDescriptor* output) {
    DataMqDescriptor converted;
    const native_handle_t* handle = input.handle();
    if (!input.isHandleValid() || handle == nullptr || handle->numFds <= 0 ||
        handle->numInts < 0 || input.countGrantors() < 3 ||
        input.getQuantum() != sizeof(uint8_t) ||
        input.getFlags() != ::android::hardware::kSynchronizedReadWrite ||
        input.getQuantum() > static_cast<size_t>(INT32_MAX)) {
        return false;
    }

    converted.grantors.reserve(input.countGrantors());
    for (const auto& grantor : input.grantors()) {
        if (grantor.flags != 0 ||
            grantor.fdIndex >= static_cast<uint32_t>(handle->numFds) ||
            grantor.fdIndex > static_cast<uint32_t>(INT32_MAX) ||
            grantor.offset > static_cast<uint32_t>(INT32_MAX) ||
            grantor.extent > static_cast<uint64_t>(INT64_MAX)) {
            return false;
        }
        ::aidl::android::hardware::common::fmq::GrantorDescriptor
                converted_grantor;
        converted_grantor.fdIndex = static_cast<int32_t>(grantor.fdIndex);
        converted_grantor.offset = static_cast<int32_t>(grantor.offset);
        converted_grantor.extent = static_cast<int64_t>(grantor.extent);
        converted.grantors.push_back(converted_grantor);
    }

    converted.handle.fds.reserve(static_cast<size_t>(handle->numFds));
    for (int index = 0; index < handle->numFds; ++index) {
        const int duplicated = fcntl(handle->data[index], F_DUPFD_CLOEXEC, 0);
        if (duplicated < 0) {
            return false;
        }
        converted.handle.fds.emplace_back(duplicated);
    }
    converted.handle.ints.reserve(static_cast<size_t>(handle->numInts));
    for (int index = 0; index < handle->numInts; ++index) {
        converted.handle.ints.push_back(handle->data[handle->numFds + index]);
    }
    converted.quantum = static_cast<int32_t>(input.getQuantum());
    converted.flags = input.getFlags();
    *output = std::move(converted);
    return true;
}

uint64_t nonNegative(int64_t value) {
    return value > 0 ? static_cast<uint64_t>(value) : 0;
}

class QtiBluetoothAudioPort final : public Qti21::IBluetoothAudioPort {
  public:
    QtiBluetoothAudioPort(std::shared_ptr<Aidl::IBluetoothAudioPort> host,
                          std::shared_ptr<std::atomic_bool> low_latency_allowed)
        : host_(std::move(host)),
          low_latency_allowed_(std::move(low_latency_allowed)) {}

    ::android::hardware::Return<void> startStream() override {
        const auto status = host_->startStream(low_latency_allowed_->load());
        logAidlFailure("startStream", status);
        return ::android::hardware::Void();
    }

    ::android::hardware::Return<void> suspendStream() override {
        const auto status = host_->suspendStream();
        logAidlFailure("suspendStream", status);
        return ::android::hardware::Void();
    }

    ::android::hardware::Return<void> stopStream() override {
        const auto status = host_->stopStream();
        logAidlFailure("stopStream", status);
        return ::android::hardware::Void();
    }

    ::android::hardware::Return<void> getPresentationPosition(
            getPresentationPosition_cb callback) override {
        Aidl::PresentationPosition position;
        const auto status = host_->getPresentationPosition(&position);
        if (!status.isOk()) {
            logAidlFailure("getPresentationPosition", status);
            callback(Qti20::Status::FAILURE, 0, 0, Qti20::TimeSpec{});
            return ::android::hardware::Void();
        }
        Qti20::TimeSpec timestamp{};
        timestamp.tvSec = nonNegative(position.transmittedOctetsTimestamp.tvSec);
        timestamp.tvNSec = nonNegative(position.transmittedOctetsTimestamp.tvNSec);
        callback(Qti20::Status::SUCCESS,
                 nonNegative(position.remoteDeviceAudioDelayNanos),
                 nonNegative(position.transmittedOctets), timestamp);
        return ::android::hardware::Void();
    }

    ::android::hardware::Return<void> updateAptxMode(uint16_t aptx_mode) override {
        LOGW("Ignoring unexpected aptX mode update for SBC/AAC bridge: %u",
             aptx_mode);
        return ::android::hardware::Void();
    }

  private:
    static void logAidlFailure(const char* operation,
                               const ::ndk::ScopedAStatus& status) {
        if (!status.isOk()) {
            const std::string description = status.getDescription();
            LOGE("AIDL host %s failed: %s", operation, description.c_str());
        }
    }

    const std::shared_ptr<Aidl::IBluetoothAudioPort> host_;
    const std::shared_ptr<std::atomic_bool> low_latency_allowed_;
};

class StandardBluetoothAudioPort final : public Aosp20::IBluetoothAudioPort {
  public:
    StandardBluetoothAudioPort(
            std::shared_ptr<Aidl::IBluetoothAudioPort> host,
            std::shared_ptr<std::atomic_bool> low_latency_allowed)
        : host_(std::move(host)),
          low_latency_allowed_(std::move(low_latency_allowed)) {}

    ::android::hardware::Return<void> startStream() override {
        logAidlFailure("startStream",
                       host_->startStream(low_latency_allowed_->load()));
        return ::android::hardware::Void();
    }

    ::android::hardware::Return<void> suspendStream() override {
        logAidlFailure("suspendStream", host_->suspendStream());
        return ::android::hardware::Void();
    }

    ::android::hardware::Return<void> stopStream() override {
        logAidlFailure("stopStream", host_->stopStream());
        return ::android::hardware::Void();
    }

    ::android::hardware::Return<void> getPresentationPosition(
            getPresentationPosition_cb callback) override {
        Aidl::PresentationPosition position;
        const auto status = host_->getPresentationPosition(&position);
        if (!status.isOk()) {
            logAidlFailure("getPresentationPosition", status);
            callback(Aosp20::Status::FAILURE, 0, 0, Aosp20::TimeSpec{});
            return ::android::hardware::Void();
        }
        Aosp20::TimeSpec timestamp{};
        timestamp.tvSec = nonNegative(position.transmittedOctetsTimestamp.tvSec);
        timestamp.tvNSec =
                nonNegative(position.transmittedOctetsTimestamp.tvNSec);
        callback(Aosp20::Status::SUCCESS,
                 nonNegative(position.remoteDeviceAudioDelayNanos),
                 nonNegative(position.transmittedOctets), timestamp);
        return ::android::hardware::Void();
    }

    ::android::hardware::Return<void> updateMetadata(
            const AudioCommon50::SourceMetadata& source_metadata) override {
        ::aidl::android::hardware::audio::common::SourceMetadata converted;
        converted.tracks.reserve(source_metadata.tracks.size());
        for (const auto& source_track : source_metadata.tracks) {
            ::aidl::android::hardware::audio::common::PlaybackTrackMetadata
                    track;
            track.usage = static_cast<
                    ::aidl::android::media::audio::common::AudioUsage>(
                    enumValue(source_track.usage));
            track.contentType = static_cast<
                    ::aidl::android::media::audio::common::AudioContentType>(
                    enumValue(source_track.contentType));
            track.gain = source_track.gain;
            converted.tracks.push_back(std::move(track));
        }
        logAidlFailure("updateSourceMetadata",
                       host_->updateSourceMetadata(converted));
        return ::android::hardware::Void();
    }

  private:
    static void logAidlFailure(const char* operation,
                               const ::ndk::ScopedAStatus& status) {
        if (!status.isOk()) {
            const std::string description = status.getDescription();
            LOGE("AIDL host %s failed: %s", operation, description.c_str());
        }
    }

    const std::shared_ptr<Aidl::IBluetoothAudioPort> host_;
    const std::shared_ptr<std::atomic_bool> low_latency_allowed_;
};

class DiagnosticBluetoothAudioPort final : public Aidl::BnBluetoothAudioPort {
  public:
    ::ndk::ScopedAStatus getPresentationPosition(
            Aidl::PresentationPosition* aidl_return) override {
        *aidl_return = Aidl::PresentationPosition{};
        return ::ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus startStream(bool) override {
        return ::ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus stopStream() override {
        return ::ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus suspendStream() override {
        return ::ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus updateSourceMetadata(
            const ::aidl::android::hardware::audio::common::SourceMetadata&)
            override {
        return ::ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus updateSinkMetadata(
            const ::aidl::android::hardware::audio::common::SinkMetadata&)
            override {
        return ::ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus setLatencyMode(Aidl::LatencyMode) override {
        return ::ndk::ScopedAStatus::ok();
    }
};

class StandardDiagnosticBluetoothAudioPort final
    : public Aosp20::IBluetoothAudioPort {
  public:
    ::android::hardware::Return<void> startStream() override {
        return ::android::hardware::Void();
    }

    ::android::hardware::Return<void> suspendStream() override {
        return ::android::hardware::Void();
    }

    ::android::hardware::Return<void> stopStream() override {
        return ::android::hardware::Void();
    }

    ::android::hardware::Return<void> getPresentationPosition(
            getPresentationPosition_cb callback) override {
        callback(Aosp20::Status::SUCCESS, 0, 0, Aosp20::TimeSpec{});
        return ::android::hardware::Void();
    }

    ::android::hardware::Return<void> updateMetadata(
            const AudioCommon50::SourceMetadata&) override {
        return ::android::hardware::Void();
    }
};

class StandardBluetoothAudioProviderBridge final
    : public Aidl::BnBluetoothAudioProvider {
  public:
    StandardBluetoothAudioProviderBridge(
            ::android::sp<Aosp20::IBluetoothAudioProvider> provider,
            Aidl::SessionType session_type)
        : provider_(std::move(provider)),
          session_type_(session_type),
          low_latency_allowed_(std::make_shared<std::atomic_bool>(false)) {}

    ~StandardBluetoothAudioProviderBridge() override {
        closeStandardSession();
    }

    ::ndk::ScopedAStatus startSession(
            const std::shared_ptr<Aidl::IBluetoothAudioPort>& host,
            const Aidl::AudioConfiguration& audio_config,
            const std::vector<Aidl::LatencyMode>&,
            DataMqDescriptor* aidl_return) override {
        *aidl_return = DataMqDescriptor{};
        if (host == nullptr) {
            return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
        }

        Aosp20::AudioConfiguration standard_config;
        Aidl::PcmConfiguration pcm_config;
        if (!convertStandardPcmConfiguration(audio_config, &standard_config,
                                             &pcm_config)) {
            LOGE("Rejected standard PCM configuration for %s",
                 Aidl::toString(session_type_).c_str());
            return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
        }

        std::lock_guard<std::mutex> backend_lock(backend_mutex_);
        bool replacing_stale_session = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (session_starting_) {
                return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
            }
            replacing_stale_session = session_active_;
            if (replacing_stale_session) {
                session_active_ = false;
                low_latency_allowed_->store(false);
                host_.reset();
                standard_port_.clear();
                pcm_config_ = {};
            }
            session_starting_ = true;
        }
        if (replacing_stale_session) {
            LOGW("Replacing stale standard session for %s",
                 Aidl::toString(session_type_).c_str());
            if (!provider_->endSession().isOk()) {
                std::lock_guard<std::mutex> lock(mutex_);
                session_starting_ = false;
                LOGE("Standard endSession failed while replacing stale "
                     "session");
                return ::ndk::ScopedAStatus::fromExceptionCode(
                        EX_ILLEGAL_STATE);
            }
        }

        ::android::sp<StandardBluetoothAudioPort> standard_port =
                new StandardBluetoothAudioPort(host, low_latency_allowed_);
        bool callback_called = false;
        Aosp20::Status standard_status = Aosp20::Status::FAILURE;
        bool data_mq_valid = false;
        DataMqDescriptor data_mq;
        const auto transport = provider_->startSession(
                standard_port, standard_config,
                [&](Aosp20::Status status,
                    const ::android::hardware::MQDescriptorSync<uint8_t>&
                            standard_data_mq) {
                    callback_called = true;
                    standard_status = status;
                    if (status == Aosp20::Status::SUCCESS) {
                        data_mq_valid = convertDataMqDescriptor(
                                standard_data_mq, &data_mq);
                    }
                });

        const bool standard_session_created = callback_called &&
                standard_status == Aosp20::Status::SUCCESS;
        const bool started = transport.isOk() && standard_session_created &&
                data_mq_valid;
        if (standard_session_created && !started) {
            const auto cleanup = provider_->endSession();
            if (!cleanup.isOk()) {
                LOGE("Standard endSession cleanup failed after partial start");
            }
        }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            session_starting_ = false;
            if (started) {
                host_ = host;
                standard_port_ = standard_port;
                pcm_config_ = std::move(pcm_config);
                session_active_ = true;
            }
        }
        if (started) {
            *aidl_return = std::move(data_mq);
            LOGI("Standard software session started: %s",
                 Aidl::toString(session_type_).c_str());
            return ::ndk::ScopedAStatus::ok();
        }

        LOGE("Standard startSession failed for %s: transport=%d callback=%d"
             " status=%u data_mq=%d",
             Aidl::toString(session_type_).c_str(), transport.isOk(),
             callback_called, enumValue(standard_status), data_mq_valid);
        return standard_status ==
                        Aosp20::Status::UNSUPPORTED_CODEC_CONFIGURATION
                ? ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT)
                : ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    }

    ::ndk::ScopedAStatus endSession() override {
        closeStandardSession();
        return ::ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus streamStarted(
            Aidl::BluetoothAudioStatus status) override {
        std::lock_guard<std::mutex> backend_lock(backend_mutex_);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!session_active_) {
                return ::ndk::ScopedAStatus::fromExceptionCode(
                        EX_ILLEGAL_STATE);
            }
        }
        return provider_->streamStarted(toAospStatus(status)).isOk()
                ? ::ndk::ScopedAStatus::ok()
                : ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    }

    ::ndk::ScopedAStatus streamSuspended(
            Aidl::BluetoothAudioStatus status) override {
        std::lock_guard<std::mutex> backend_lock(backend_mutex_);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!session_active_) {
                return ::ndk::ScopedAStatus::fromExceptionCode(
                        EX_ILLEGAL_STATE);
            }
        }
        return provider_->streamSuspended(toAospStatus(status)).isOk()
                ? ::ndk::ScopedAStatus::ok()
                : ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    }

    ::ndk::ScopedAStatus updateAudioConfiguration(
            const Aidl::AudioConfiguration& audio_config) override {
        Aosp20::AudioConfiguration ignored;
        Aidl::PcmConfiguration updated;
        if (!convertStandardPcmConfiguration(audio_config, &ignored, &updated)) {
            return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
        }
        std::lock_guard<std::mutex> backend_lock(backend_mutex_);
        std::lock_guard<std::mutex> lock(mutex_);
        if (!session_active_) {
            return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
        }
        if (pcm_config_ == updated) {
            return ::ndk::ScopedAStatus::ok();
        }
        pcm_config_ = std::move(updated);
        LOGI("Standard software PCM configuration updated");
        return ::ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus setLowLatencyModeAllowed(bool allowed) override {
        low_latency_allowed_->store(allowed);
        return ::ndk::ScopedAStatus::ok();
    }

  private:
    void closeStandardSession() {
        std::lock_guard<std::mutex> backend_lock(backend_mutex_);
        bool should_close = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            should_close = session_active_;
            session_active_ = false;
            session_starting_ = false;
            low_latency_allowed_->store(false);
            host_.reset();
            standard_port_.clear();
        }
        if (should_close && !provider_->endSession().isOk()) {
            LOGE("Standard endSession transport failed");
        }
    }

    const ::android::sp<Aosp20::IBluetoothAudioProvider> provider_;
    const Aidl::SessionType session_type_;
    const std::shared_ptr<std::atomic_bool> low_latency_allowed_;
    mutable std::mutex backend_mutex_;
    mutable std::mutex mutex_;
    bool session_starting_ = false;
    bool session_active_ = false;
    std::shared_ptr<Aidl::IBluetoothAudioPort> host_;
    ::android::sp<StandardBluetoothAudioPort> standard_port_;
    Aidl::PcmConfiguration pcm_config_;
};

class QtiBluetoothAudioProviderBridge final
    : public Aidl::BnBluetoothAudioProvider {
  public:
    QtiBluetoothAudioProviderBridge(
            ::android::sp<Qti21::IBluetoothAudioProvider> provider,
            Aidl::SessionType session_type)
        : provider_(std::move(provider)),
          session_type_(session_type),
          low_latency_allowed_(std::make_shared<std::atomic_bool>(false)) {}

    ~QtiBluetoothAudioProviderBridge() override {
        closeQtiSession();
    }

    ::ndk::ScopedAStatus startSession(
            const std::shared_ptr<Aidl::IBluetoothAudioPort>& host,
            const Aidl::AudioConfiguration& audio_config,
            const std::vector<Aidl::LatencyMode>&,
            DataMqDescriptor* aidl_return) override {
        *aidl_return = DataMqDescriptor{};
        if (host == nullptr) {
            return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
        }

        Qti21::AudioConfiguration qti_config{};
        Aidl::CodecConfiguration codec_config;
        Aidl::PcmConfiguration pcm_config;
        const bool pcm_session = isPcmSession(session_type_);
        const bool valid_configuration = pcm_session
                ? convertPcmConfiguration(audio_config, &qti_config, &pcm_config)
                : convertConfiguration(audio_config, &qti_config, &codec_config);
        if (!valid_configuration) {
            LOGE("Rejected configuration for session %s",
                 Aidl::toString(session_type_).c_str());
            return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
        }

        std::lock_guard<std::mutex> backend_lock(backend_mutex_);
        bool replacing_stale_session = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (session_starting_) {
                return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
            }
            replacing_stale_session = session_active_;
            if (replacing_stale_session) {
                session_active_ = false;
                low_latency_allowed_->store(false);
                host_.reset();
                qti_port_.clear();
                codec_config_ = {};
                pcm_config_ = {};
            }
            session_starting_ = true;
        }
        if (replacing_stale_session) {
            LOGW("Replacing stale QTI session for %s",
                 Aidl::toString(session_type_).c_str());
            if (!provider_->endSession().isOk()) {
                std::lock_guard<std::mutex> lock(mutex_);
                session_starting_ = false;
                LOGE("QTI endSession failed while replacing stale session");
                return ::ndk::ScopedAStatus::fromExceptionCode(
                        EX_ILLEGAL_STATE);
            }
        }

        ::android::sp<QtiBluetoothAudioPort> qti_port =
                new QtiBluetoothAudioPort(host, low_latency_allowed_);
        bool callback_called = false;
        Qti20::Status qti_status = Qti20::Status::FAILURE;
        bool data_mq_valid = !pcm_session;
        DataMqDescriptor data_mq;
        const auto transport = provider_->startSession_2_1(
                qti_port, qti_config,
                [&](Qti20::Status status,
                    const ::android::hardware::MQDescriptorSync<uint8_t>&
                            qti_data_mq) {
                    callback_called = true;
                    qti_status = status;
                    if (status == Qti20::Status::SUCCESS && pcm_session) {
                        data_mq_valid =
                                convertDataMqDescriptor(qti_data_mq, &data_mq);
                    }
                });

        const bool qti_session_created = callback_called &&
                qti_status == Qti20::Status::SUCCESS;
        const bool started = transport.isOk() && qti_session_created &&
                data_mq_valid;
        if (qti_session_created && !started) {
            const auto cleanup = provider_->endSession();
            if (!cleanup.isOk()) {
                LOGE("QTI endSession cleanup failed after partial start");
            }
        }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            session_starting_ = false;
            if (started) {
                host_ = host;
                qti_port_ = qti_port;
                if (pcm_session) {
                    pcm_config_ = std::move(pcm_config);
                } else {
                    codec_config_ = std::move(codec_config);
                }
                session_active_ = true;
            }
        }
        if (started) {
            if (pcm_session) {
                *aidl_return = std::move(data_mq);
            }
            LOGI("QTI session started: %s",
                 Aidl::toString(session_type_).c_str());
            return ::ndk::ScopedAStatus::ok();
        }

        LOGE("QTI startSession_2_1 failed for %s: transport=%d callback=%d"
             " status=%u data_mq=%d",
             Aidl::toString(session_type_).c_str(), transport.isOk(),
             callback_called, enumValue(qti_status), data_mq_valid);
        return qti_status == Qti20::Status::UNSUPPORTED_CODEC_CONFIGURATION
                ? ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT)
                : ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    }

    ::ndk::ScopedAStatus endSession() override {
        closeQtiSession();
        return ::ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus streamStarted(
            Aidl::BluetoothAudioStatus status) override {
        std::lock_guard<std::mutex> backend_lock(backend_mutex_);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!session_active_) {
                return ::ndk::ScopedAStatus::fromExceptionCode(
                        EX_ILLEGAL_STATE);
            }
        }
        const auto transport = provider_->streamStarted(toQtiStatus(status));
        return transport.isOk()
                ? ::ndk::ScopedAStatus::ok()
                : ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    }

    ::ndk::ScopedAStatus streamSuspended(
            Aidl::BluetoothAudioStatus status) override {
        std::lock_guard<std::mutex> backend_lock(backend_mutex_);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!session_active_) {
                return ::ndk::ScopedAStatus::fromExceptionCode(
                        EX_ILLEGAL_STATE);
            }
        }
        const auto transport = provider_->streamSuspended(toQtiStatus(status));
        return transport.isOk()
                ? ::ndk::ScopedAStatus::ok()
                : ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    }

    ::ndk::ScopedAStatus updateAudioConfiguration(
            const Aidl::AudioConfiguration& audio_config) override {
        if (isPcmSession(session_type_)) {
            Qti21::AudioConfiguration ignored{};
            Aidl::PcmConfiguration updated;
            if (!convertPcmConfiguration(audio_config, &ignored, &updated)) {
                return ::ndk::ScopedAStatus::fromExceptionCode(
                        EX_ILLEGAL_ARGUMENT);
            }
            std::lock_guard<std::mutex> backend_lock(backend_mutex_);
            std::lock_guard<std::mutex> lock(mutex_);
            if (!session_active_) {
                return ::ndk::ScopedAStatus::fromExceptionCode(
                        EX_ILLEGAL_STATE);
            }
            if (pcm_config_ == updated) {
                return ::ndk::ScopedAStatus::ok();
            }
            LOGW("QTI HIDL 2.1 cannot reconfigure PCM parameters in-place");
            return ::ndk::ScopedAStatus::fromExceptionCode(
                    EX_UNSUPPORTED_OPERATION);
        }

        Qti21::AudioConfiguration ignored{};
        Aidl::CodecConfiguration updated;
        if (!convertConfiguration(audio_config, &ignored, &updated)) {
            return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
        }

        std::lock_guard<std::mutex> backend_lock(backend_mutex_);
        Aidl::CodecConfiguration applied;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!session_active_) {
                return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
            }
            applied = codec_config_;
        }
        if (applied.codecType != updated.codecType ||
            applied.config != updated.config ||
            applied.isScmstEnabled != updated.isScmstEnabled) {
            LOGW("QTI HIDL 2.1 cannot reconfigure codec parameters in-place");
            return ::ndk::ScopedAStatus::fromExceptionCode(
                    EX_UNSUPPORTED_OPERATION);
        }
        if (applied.peerMtu != updated.peerMtu) {
            if (!updateSessionParameterLocked(
                        Qti20::SessionParamType::MTU,
                        static_cast<uint32_t>(updated.peerMtu))) {
                return ::ndk::ScopedAStatus::fromExceptionCode(
                        EX_ILLEGAL_STATE);
            }
            applied.peerMtu = updated.peerMtu;
        }
        if (applied.encodedAudioBitrate != updated.encodedAudioBitrate) {
            if (!updateSessionParameterLocked(
                        Qti20::SessionParamType::BITRATE,
                        static_cast<uint32_t>(updated.encodedAudioBitrate))) {
                std::lock_guard<std::mutex> lock(mutex_);
                codec_config_ = std::move(applied);
                return ::ndk::ScopedAStatus::fromExceptionCode(
                        EX_ILLEGAL_STATE);
            }
            applied.encodedAudioBitrate = updated.encodedAudioBitrate;
        }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            codec_config_ = std::move(updated);
        }
        return ::ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus setLowLatencyModeAllowed(bool allowed) override {
        low_latency_allowed_->store(allowed);
        return ::ndk::ScopedAStatus::ok();
    }

  private:
    // Callers hold backend_mutex_ so QTI HIDL session operations cannot overlap.
    bool updateSessionParameterLocked(Qti20::SessionParamType type,
                                      uint32_t value) {
        Qti20::SessionParams parameters{};
        parameters.paramType = type;
        if (type == Qti20::SessionParamType::MTU) {
            if (value > UINT16_MAX) {
                return false;
            }
            parameters.param.mtu = static_cast<uint16_t>(value);
        } else {
            parameters.param.encodedAudioBitrate = value;
        }
        return provider_->updateSessionParams(parameters).isOk();
    }

    void closeQtiSession() {
        std::lock_guard<std::mutex> backend_lock(backend_mutex_);
        bool should_close = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            should_close = session_active_;
            session_active_ = false;
            session_starting_ = false;
            low_latency_allowed_->store(false);
            host_.reset();
            qti_port_.clear();
        }
        if (should_close) {
            const auto transport = provider_->endSession();
            if (!transport.isOk()) {
                LOGE("QTI endSession transport failed");
            }
        }
    }

    const ::android::sp<Qti21::IBluetoothAudioProvider> provider_;
    const Aidl::SessionType session_type_;
    const std::shared_ptr<std::atomic_bool> low_latency_allowed_;
    mutable std::mutex backend_mutex_;
    mutable std::mutex mutex_;
    bool session_starting_ = false;
    bool session_active_ = false;
    std::shared_ptr<Aidl::IBluetoothAudioPort> host_;
    ::android::sp<QtiBluetoothAudioPort> qti_port_;
    Aidl::CodecConfiguration codec_config_;
    Aidl::PcmConfiguration pcm_config_;
};

class UnavailableBluetoothAudioProviderBridge final
    : public Aidl::BnBluetoothAudioProvider {
  public:
    explicit UnavailableBluetoothAudioProviderBridge(
            Aidl::SessionType session_type)
        : session_type_(session_type) {}

    ::ndk::ScopedAStatus startSession(
            const std::shared_ptr<Aidl::IBluetoothAudioPort>&,
            const Aidl::AudioConfiguration&,
            const std::vector<Aidl::LatencyMode>&,
            DataMqDescriptor* aidl_return) override {
        *aidl_return = DataMqDescriptor{};
        LOGW("Unsupported session requested: %s",
             Aidl::toString(session_type_).c_str());
        return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
    }

    ::ndk::ScopedAStatus endSession() override {
        return ::ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus streamStarted(Aidl::BluetoothAudioStatus) override {
        return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    }

    ::ndk::ScopedAStatus streamSuspended(Aidl::BluetoothAudioStatus) override {
        return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    }

    ::ndk::ScopedAStatus updateAudioConfiguration(
            const Aidl::AudioConfiguration&) override {
        return ::ndk::ScopedAStatus::fromExceptionCode(
                EX_UNSUPPORTED_OPERATION);
    }

    ::ndk::ScopedAStatus setLowLatencyModeAllowed(bool) override {
        return ::ndk::ScopedAStatus::ok();
    }

  private:
    const Aidl::SessionType session_type_;
};

class BluetoothAudioProviderFactoryBridge final
    : public Aidl::BnBluetoothAudioProviderFactory {
  public:
    BluetoothAudioProviderFactoryBridge(
            ::android::sp<Aosp20::IBluetoothAudioProvidersFactory>
                    standard_factory,
            ::android::sp<Qti21::IBluetoothAudioProvidersFactory> qti_factory)
        : standard_factory_(std::move(standard_factory)),
          qti_factory_(std::move(qti_factory)) {}

    ::ndk::ScopedAStatus getProviderCapabilities(
            Aidl::SessionType session_type,
            std::vector<Aidl::AudioCapabilities>* aidl_return) override {
        std::lock_guard<std::mutex> lock(providers_mutex_);
        aidl_return->clear();
        const auto standard_session_type = toAospSessionType(session_type);
        if (standard_session_type.has_value()) {
            for (int attempt = 0; attempt < 2; ++attempt) {
                if (!ensureStandardFactoryLocked()) {
                    break;
                }
                bool callback_called = false;
                const auto transport =
                        standard_factory_->getProviderCapabilities(
                                *standard_session_type,
                                [&](const ::android::hardware::hidl_vec<
                                        Aosp20::AudioCapabilities>&
                                                capabilities) {
                                    callback_called = true;
                                    aidl_return->reserve(capabilities.size());
                                    for (const auto& capability :
                                         capabilities) {
                                        if (capability.getDiscriminator() !=
                                            Aosp20::AudioCapabilities::
                                                    hidl_discriminator::
                                                            pcmCapabilities) {
                                            continue;
                                        }
                                        Aidl::PcmCapabilities converted;
                                        if (convertStandardPcmCapabilities(
                                                    capability.pcmCapabilities(),
                                                    &converted)) {
                                            aidl_return->push_back(
                                                    Aidl::AudioCapabilities::
                                                            make<
                                                                    Aidl::AudioCapabilities::
                                                                            pcmCapabilities>(
                                                                    std::move(
                                                                            converted)));
                                        }
                                    }
                                });
                if (transport.isOk() && callback_called) {
                    LOGI("Advertising %zu standard PCM capabilities for %s",
                         aidl_return->size(),
                         Aidl::toString(session_type).c_str());
                    return ::ndk::ScopedAStatus::ok();
                }
                aidl_return->clear();
                LOGW("Standard getProviderCapabilities transport failed; "
                     "reconnecting backend");
                invalidateStandardFactoryLocked();
            }
            LOGE("Standard getProviderCapabilities failed after reconnect");
            return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
        }

        const auto qti_session_type = toQtiSessionType(session_type);
        if (!qti_session_type.has_value() ||
            session_type != kA2dpOffloadSession) {
            return ::ndk::ScopedAStatus::ok();
        }

        for (int attempt = 0; attempt < 2; ++attempt) {
            if (!ensureQtiFactoryLocked()) {
                LOGW("QTI offload backend is unavailable; advertising no "
                     "offload capabilities");
                return ::ndk::ScopedAStatus::ok();
            }
            bool callback_called = false;
            const auto transport = qti_factory_->getProviderCapabilities_2_1(
                    *qti_session_type,
                    [&](const ::android::hardware::hidl_vec<
                            Qti21::AudioCapabilities>& capabilities) {
                        callback_called = true;
                        aidl_return->reserve(capabilities.size());
                        for (const auto& capability : capabilities) {
                            auto converted = convertCapabilities(capability);
                            if (converted.has_value()) {
                                aidl_return->push_back(std::move(*converted));
                            }
                        }
                    });
            if (transport.isOk() && callback_called) {
                LOGI("Advertising %zu capabilities for %s",
                     aidl_return->size(),
                     Aidl::toString(session_type).c_str());
                return ::ndk::ScopedAStatus::ok();
            }
            aidl_return->clear();
            LOGW("QTI getProviderCapabilities_2_1 transport failed; "
                 "reconnecting backend");
            invalidateQtiFactoryLocked();
        }
        LOGE("QTI getProviderCapabilities_2_1 failed after reconnect");
        return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    }

    ::ndk::ScopedAStatus openProvider(
            Aidl::SessionType session_type,
            std::shared_ptr<Aidl::IBluetoothAudioProvider>* aidl_return)
            override {
        aidl_return->reset();
        std::lock_guard<std::mutex> lock(providers_mutex_);
        const auto standard_session_type = toAospSessionType(session_type);
        if (standard_session_type.has_value()) {
            for (int attempt = 0; attempt < 2; ++attempt) {
                if (!ensureStandardFactoryLocked()) {
                    break;
                }
                const auto cached = providers_.find(session_type);
                if (cached != providers_.end()) {
                    *aidl_return = cached->second;
                    LOGI("Reusing provider for %s",
                         Aidl::toString(session_type).c_str());
                    return ::ndk::ScopedAStatus::ok();
                }

                bool callback_called = false;
                Aosp20::Status standard_status = Aosp20::Status::FAILURE;
                ::android::sp<Aosp20::IBluetoothAudioProvider>
                        standard_provider;
                const auto transport = standard_factory_->openProvider(
                        *standard_session_type,
                        [&](Aosp20::Status status,
                            const ::android::sp<
                                    Aosp20::IBluetoothAudioProvider>&
                                    provider) {
                            callback_called = true;
                            standard_status = status;
                            standard_provider = provider;
                        });
                if (transport.isOk() && callback_called &&
                    standard_status == Aosp20::Status::SUCCESS &&
                    standard_provider != nullptr) {
                    *aidl_return = ::ndk::SharedRefBase::make<
                            StandardBluetoothAudioProviderBridge>(
                            std::move(standard_provider), session_type);
                    providers_.emplace(session_type, *aidl_return);
                    return ::ndk::ScopedAStatus::ok();
                }
                LOGW("Standard openProvider failed: transport=%d callback=%d"
                     " status=%u; reconnecting backend",
                     transport.isOk(), callback_called,
                     enumValue(standard_status));
                invalidateStandardFactoryLocked();
            }
            LOGE("Standard openProvider failed after reconnect");
            return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
        }

        const auto qti_session_type = toQtiSessionType(session_type);
        if (!qti_session_type.has_value() ||
            session_type != kA2dpOffloadSession) {
            const auto cached = providers_.find(session_type);
            if (cached != providers_.end()) {
                *aidl_return = cached->second;
                return ::ndk::ScopedAStatus::ok();
            }
            *aidl_return = ::ndk::SharedRefBase::make<
                    UnavailableBluetoothAudioProviderBridge>(session_type);
            providers_.emplace(session_type, *aidl_return);
            LOGW("Opened placeholder provider for unsupported session %s",
                 Aidl::toString(session_type).c_str());
            return ::ndk::ScopedAStatus::ok();
        }

        for (int attempt = 0; attempt < 2; ++attempt) {
            if (!ensureQtiFactoryLocked()) {
                *aidl_return = ::ndk::SharedRefBase::make<
                        UnavailableBluetoothAudioProviderBridge>(session_type);
                LOGW("QTI offload backend is unavailable for %s",
                     Aidl::toString(session_type).c_str());
                return ::ndk::ScopedAStatus::ok();
            }
            const auto cached = providers_.find(session_type);
            if (cached != providers_.end()) {
                *aidl_return = cached->second;
                LOGI("Reusing provider for %s",
                     Aidl::toString(session_type).c_str());
                return ::ndk::ScopedAStatus::ok();
            }

            bool callback_called = false;
            Qti20::Status qti_status = Qti20::Status::FAILURE;
            ::android::sp<Qti21::IBluetoothAudioProvider> qti_provider;
            const auto transport = qti_factory_->openProvider_2_1(
                    *qti_session_type,
                    [&](Qti20::Status status,
                        const ::android::sp<Qti21::IBluetoothAudioProvider>&
                                provider) {
                        callback_called = true;
                        qti_status = status;
                        qti_provider = provider;
                    });
            if (transport.isOk() && callback_called &&
                qti_status == Qti20::Status::SUCCESS &&
                qti_provider != nullptr) {
                *aidl_return = ::ndk::SharedRefBase::make<
                        QtiBluetoothAudioProviderBridge>(
                        std::move(qti_provider), session_type);
                providers_.emplace(session_type, *aidl_return);
                return ::ndk::ScopedAStatus::ok();
            }
            LOGW("QTI openProvider_2_1 failed: transport=%d callback=%d "
                 "status=%u; reconnecting backend",
                 transport.isOk(), callback_called, enumValue(qti_status));
            invalidateQtiFactoryLocked();
        }
        LOGE("QTI openProvider_2_1 failed after reconnect");
        return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    }

  private:
    void invalidateStandardFactoryLocked() {
        standard_factory_.clear();
        providers_.erase(kA2dpSoftwareSession);
        providers_.erase(kHearingAidSoftwareSession);
    }

    void invalidateQtiFactoryLocked() {
        qti_factory_.clear();
        providers_.erase(kA2dpOffloadSession);
    }

    bool ensureStandardFactoryLocked() {
        if (standard_factory_ != nullptr) {
            const auto ping = standard_factory_->ping();
            if (ping.isOk()) {
                return true;
            }
            LOGW("Standard HIDL factory died; reconnecting");
            invalidateStandardFactoryLocked();
        }

        for (int attempt = 0; attempt < 10; ++attempt) {
            auto factory =
                    Aosp20::IBluetoothAudioProvidersFactory::tryGetService(
                            "default", false);
            if (factory != nullptr && factory->ping().isOk()) {
                standard_factory_ = std::move(factory);
                LOGI("Reconnected standard HIDL 2.0 factory");
                return true;
            }
            if (attempt != 9) {
                usleep(100000);
            }
        }
        LOGE("Standard HIDL 2.0 factory is unavailable after reconnect");
        return false;
    }

    bool ensureQtiFactoryLocked() {
        if (qti_factory_ != nullptr) {
            const auto ping = qti_factory_->ping();
            if (ping.isOk()) {
                return true;
            }
            LOGW("QTI HIDL factory died; reconnecting");
            invalidateQtiFactoryLocked();
        }

        auto factory = Qti21::IBluetoothAudioProvidersFactory::tryGetService(
                "default", false);
        if (factory != nullptr && factory->ping().isOk()) {
            qti_factory_ = std::move(factory);
            LOGI("Connected QTI HIDL 2.1 offload factory");
            return true;
        }
        LOGW("QTI HIDL 2.1 offload factory is unavailable");
        return false;
    }

    ::android::sp<Aosp20::IBluetoothAudioProvidersFactory> standard_factory_;
    ::android::sp<Qti21::IBluetoothAudioProvidersFactory> qti_factory_;
    std::mutex providers_mutex_;
    std::map<Aidl::SessionType,
             std::shared_ptr<Aidl::IBluetoothAudioProvider>> providers_;
};

int checkSoftwareSession(
        const ::android::sp<Qti21::IBluetoothAudioProvidersFactory>& factory) {
    bool callback_called = false;
    Qti20::Status qti_status = Qti20::Status::FAILURE;
    ::android::sp<Qti21::IBluetoothAudioProvider> qti_provider;
    const auto transport = factory->openProvider_2_1(
            kQtiA2dpSoftwareSession,
            [&](Qti20::Status status,
                const ::android::sp<Qti21::IBluetoothAudioProvider>& provider) {
                callback_called = true;
                qti_status = status;
                qti_provider = provider;
            });
    if (!transport.isOk() || !callback_called ||
        qti_status != Qti20::Status::SUCCESS || qti_provider == nullptr) {
        std::fprintf(stderr,
                     "QTI software provider open failed: transport=%d"
                     " callback=%d status=%u\n",
                     transport.isOk(), callback_called, enumValue(qti_status));
        return EXIT_FAILURE;
    }

    auto provider = ::ndk::SharedRefBase::make<QtiBluetoothAudioProviderBridge>(
            std::move(qti_provider), kA2dpSoftwareSession);
    auto host = ::ndk::SharedRefBase::make<DiagnosticBluetoothAudioPort>();
    Aidl::PcmConfiguration pcm_config;
    pcm_config.sampleRateHz = 48000;
    pcm_config.channelMode = Aidl::ChannelMode::STEREO;
    pcm_config.bitsPerSample = 16;
    pcm_config.dataIntervalUs = 20000;
    const auto audio_config = Aidl::AudioConfiguration::make<
            Aidl::AudioConfiguration::pcmConfig>(pcm_config);
    DataMqDescriptor descriptor;
    const auto start_status = provider->startSession(
            host, audio_config, {Aidl::LatencyMode::FREE}, &descriptor);
    if (!start_status.isOk()) {
        std::fprintf(stderr, "QTI software session start failed: %s\n",
                     start_status.getDescription().c_str());
        return EXIT_FAILURE;
    }

    ::android::AidlMessageQueue<
            int8_t,
            ::aidl::android::hardware::common::fmq::SynchronizedReadWrite>
            data_mq(descriptor);
    const bool data_mq_valid = data_mq.isValid();
    std::printf("software_session_data_mq_valid=%d grantors=%zu fds=%zu"
                " quantum=%d flags=%d\n",
                data_mq_valid, descriptor.grantors.size(),
                descriptor.handle.fds.size(), descriptor.quantum,
                descriptor.flags);
    const auto end_status = provider->endSession();
    if (!end_status.isOk()) {
        std::fprintf(stderr, "QTI software session end failed: %s\n",
                     end_status.getDescription().c_str());
        return EXIT_FAILURE;
    }
    return data_mq_valid ? EXIT_SUCCESS : EXIT_FAILURE;
}

int checkStandardSoftwareSession(
        const ::android::sp<Aosp20::IBluetoothAudioProvidersFactory>& factory,
        Aosp20::SampleRate sample_rate) {
    constexpr auto session_type =
            Aosp20::SessionType::A2DP_SOFTWARE_ENCODING_DATAPATH;
    bool capabilities_called = false;
    bool supports_sample_rate = false;
    const auto capabilities_transport = factory->getProviderCapabilities(
            session_type,
            [&](const ::android::hardware::hidl_vec<
                    Aosp20::AudioCapabilities>& capabilities) {
                capabilities_called = true;
                std::printf("standard_software_capability_count=%zu\n",
                            capabilities.size());
                for (size_t index = 0; index < capabilities.size(); ++index) {
                    if (capabilities[index].getDiscriminator() !=
                        Aosp20::AudioCapabilities::hidl_discriminator::
                                pcmCapabilities) {
                        std::printf(
                                "standard_software_capability[%zu] non_pcm\n",
                                index);
                        continue;
                    }
                    const auto& pcm = capabilities[index].pcmCapabilities();
                    std::printf(
                            "standard_software_capability[%zu]"
                            " sample_rate_mask=0x%x channel_mask=0x%x"
                            " bits_mask=0x%x\n",
                            index,
                            static_cast<unsigned>(enumValue(pcm.sampleRate)),
                            static_cast<unsigned>(enumValue(pcm.channelMode)),
                            static_cast<unsigned>(enumValue(pcm.bitsPerSample)));
                    supports_sample_rate |=
                            hasFlag(pcm.sampleRate, sample_rate);
                }
            });
    if (!capabilities_transport.isOk() || !capabilities_called ||
        !supports_sample_rate) {
        std::fprintf(stderr,
                     "Standard software capability check failed:"
                     " transport=%d callback=%d sample_rate=0x%x"
                     " supported=%d\n",
                     capabilities_transport.isOk(), capabilities_called,
                     static_cast<unsigned>(enumValue(sample_rate)),
                     supports_sample_rate);
        return EXIT_FAILURE;
    }

    bool open_called = false;
    Aosp20::Status open_status = Aosp20::Status::FAILURE;
    ::android::sp<Aosp20::IBluetoothAudioProvider> provider;
    const auto open_transport = factory->openProvider(
            session_type,
            [&](Aosp20::Status status,
                const ::android::sp<Aosp20::IBluetoothAudioProvider>& opened) {
                open_called = true;
                open_status = status;
                provider = opened;
            });
    if (!open_transport.isOk() || !open_called ||
        open_status != Aosp20::Status::SUCCESS || provider == nullptr) {
        std::fprintf(stderr,
                     "Standard software provider open failed: transport=%d"
                     " callback=%d status=%u\n",
                     open_transport.isOk(), open_called,
                     enumValue(open_status));
        return EXIT_FAILURE;
    }

    Aosp20::PcmParameters pcm_config{};
    pcm_config.sampleRate = sample_rate;
    pcm_config.channelMode = Aosp20::ChannelMode::STEREO;
    pcm_config.bitsPerSample = Aosp20::BitsPerSample::BITS_16;
    Aosp20::AudioConfiguration audio_config;
    audio_config.pcmConfig(pcm_config);
    ::android::sp<StandardDiagnosticBluetoothAudioPort> port =
            new StandardDiagnosticBluetoothAudioPort();
    bool start_called = false;
    Aosp20::Status start_status = Aosp20::Status::FAILURE;
    bool data_mq_valid = false;
    DataMqDescriptor descriptor;
    const auto start_transport = provider->startSession(
            port, audio_config,
            [&](Aosp20::Status status,
                const ::android::hardware::MQDescriptorSync<uint8_t>& data_mq) {
                start_called = true;
                start_status = status;
                if (status == Aosp20::Status::SUCCESS) {
                    data_mq_valid =
                            convertDataMqDescriptor(data_mq, &descriptor);
                }
            });
    const bool started = start_transport.isOk() && start_called &&
            start_status == Aosp20::Status::SUCCESS && data_mq_valid;
    std::printf(
            "standard_software_session transport=%d callback=%d status=%u"
            " data_mq_valid=%d grantors=%zu fds=%zu quantum=%d flags=%d\n",
            start_transport.isOk(), start_called, enumValue(start_status),
            data_mq_valid, descriptor.grantors.size(),
            descriptor.handle.fds.size(), descriptor.quantum,
            descriptor.flags);
    if (start_called && start_status == Aosp20::Status::SUCCESS) {
        const auto end_transport = provider->endSession();
        if (!end_transport.isOk()) {
            std::fprintf(stderr,
                         "Standard software session end failed\n");
            return EXIT_FAILURE;
        }
    }
    return started ? EXIT_SUCCESS : EXIT_FAILURE;
}

struct BinderRuntime {
    void* library = nullptr;
    binder_exception_t (*add_service)(AIBinder*, const char*) = nullptr;
    bool (*set_thread_pool_max_thread_count)(uint32_t) = nullptr;
    void (*join_thread_pool)() = nullptr;
    void (*mark_vintf_stability)(AIBinder*) = nullptr;
};

BinderRuntime g_binder;

bool loadSymbol(void* library, const char* name, void* destination,
                size_t destination_size) {
    void* symbol = dlsym(library, name);
    if (symbol == nullptr) {
        LOGE("Unable to resolve %s: %s", name, dlerror());
        return false;
    }
    if (destination_size != sizeof(symbol)) {
        LOGE("Unexpected function pointer size for %s", name);
        return false;
    }
    std::memcpy(destination, &symbol, sizeof(symbol));
    return true;
}

#define LOAD_BINDER_SYMBOL(member, symbol_name) \
    loadSymbol(g_binder.library, symbol_name, &g_binder.member, \
               sizeof(g_binder.member))

bool loadBinderRuntime() {
    g_binder.library = dlopen("libbinder_ndk.so", RTLD_NOW | RTLD_LOCAL);
    if (g_binder.library == nullptr) {
        LOGE("Unable to load libbinder_ndk.so: %s", dlerror());
        return false;
    }
    return LOAD_BINDER_SYMBOL(add_service, "AServiceManager_addService") &&
           LOAD_BINDER_SYMBOL(set_thread_pool_max_thread_count,
                              "ABinderProcess_setThreadPoolMaxThreadCount") &&
           LOAD_BINDER_SYMBOL(join_thread_pool,
                              "ABinderProcess_joinThreadPool") &&
           LOAD_BINDER_SYMBOL(mark_vintf_stability,
                               "AIBinder_markVintfStability");
}

bool checkPcmBackend(
        const ::android::sp<Qti21::IBluetoothAudioProvidersFactory>& factory,
        Qti20::SessionType session_type, const char* name) {
    bool callback_called = false;
    size_t supported_capabilities = 0;
    const auto transport = factory->getProviderCapabilities_2_1(
            session_type,
            [&](const ::android::hardware::hidl_vec<
                    Qti21::AudioCapabilities>& capabilities) {
                callback_called = true;
                std::printf("%s_capability_count=%zu\n", name,
                            capabilities.size());
                for (size_t index = 0; index < capabilities.size(); ++index) {
                    const auto& pcm = capabilities[index].pcmCapabilities;
                    std::printf(
                            "%s_capability[%zu] sample_rate_mask=0x%x"
                            " channel_mask=0x%x bits_mask=0x%x\n",
                            name, index,
                            static_cast<unsigned>(enumValue(pcm.sampleRate)),
                            static_cast<unsigned>(enumValue(pcm.channelMode)),
                            static_cast<unsigned>(enumValue(pcm.bitsPerSample)));
                    Aidl::PcmCapabilities converted;
                    if (convertPcmCapabilities(pcm, &converted)) {
                        ++supported_capabilities;
                    }
                }
            });
    if (!transport.isOk() || !callback_called ||
        supported_capabilities == 0) {
        std::fprintf(stderr,
                     "%s capability check failed: transport=%d callback=%d"
                     " supported=%zu\n",
                     name, transport.isOk(), callback_called,
                     supported_capabilities);
        return false;
    }
    return true;
}

int checkQtiBackend(
        const ::android::sp<Qti21::IBluetoothAudioProvidersFactory>& factory) {
    bool callback_called = false;
    size_t supported_codecs = 0;
    const auto transport = factory->getProviderCapabilities_2_1(
            kQtiA2dpOffloadSession,
            [&](const ::android::hardware::hidl_vec<
                    Qti21::AudioCapabilities>& capabilities) {
                callback_called = true;
                std::printf("capability_count=%zu\n", capabilities.size());
                const auto as_unsigned = [](auto value) {
                    return static_cast<unsigned>(enumValue(value));
                };
                for (size_t index = 0; index < capabilities.size(); ++index) {
                    const auto& codec = capabilities[index].codecCapabilities;
                    std::printf("capability[%zu] codec=0x%x", index,
                                as_unsigned(codec.codecType));
                    if (codec.codecType == Qti21::CodecType::SBC) {
                        const auto& sbc = codec.capabilities.sbcCapabilities;
                        std::printf(
                                " SBC sample_rate_mask=0x%x channel_mask=0x%x"
                                " block_mask=0x%x subband_mask=0x%x"
                                " alloc_mask=0x%x bits_mask=0x%x"
                                " bitpool=%u-%u",
                                as_unsigned(sbc.sampleRate),
                                as_unsigned(sbc.channelMode),
                                as_unsigned(sbc.blockLength),
                                as_unsigned(sbc.numSubbands),
                                as_unsigned(sbc.allocMethod),
                                as_unsigned(sbc.bitsPerSample),
                                static_cast<unsigned>(sbc.minBitpool),
                                static_cast<unsigned>(sbc.maxBitpool));
                        ++supported_codecs;
                    } else if (codec.codecType == Qti21::CodecType::AAC) {
                        const auto& aac = codec.capabilities.aacCapabilities;
                        std::printf(
                                " AAC object_mask=0x%x sample_rate_mask=0x%x"
                                " channel_mask=0x%x vbr=0x%x bits_mask=0x%x"
                                " frame_control=%d",
                                as_unsigned(aac.objectType),
                                as_unsigned(aac.sampleRate),
                                as_unsigned(aac.channelMode),
                                as_unsigned(aac.variableBitRateEnabled),
                                as_unsigned(aac.bitsPerSample),
                                aac.frameControlEnabled);
                        ++supported_codecs;
                    }
                    std::printf("\n");
                }
            });
    if (!transport.isOk() || !callback_called || supported_codecs == 0) {
        std::fprintf(stderr,
                     "QTI capability check failed: transport=%d callback=%d"
                     " supported=%zu\n",
                     transport.isOk(), callback_called, supported_codecs);
        return EXIT_FAILURE;
    }
    if (!checkPcmBackend(factory, kQtiA2dpSoftwareSession,
                         "a2dp_software")) {
        return EXIT_FAILURE;
    }
    // Hearing Aid is optional for the current A2DP repair.  Still print its
    // raw capability so device coverage is visible in --check output.
    (void)checkPcmBackend(factory, kQtiHearingAidSoftwareSession,
                          "hearing_aid_software");
    return EXIT_SUCCESS;
}

}  // namespace

int main(int argc, char** argv) {
    const bool check_only = argc == 2 && std::strcmp(argv[1], "--check") == 0;
    const bool check_software_session =
            argc == 2 &&
            std::strcmp(argv[1], "--check-software-session") == 0;
    const bool check_standard_software_session =
            argc == 2 &&
            std::strcmp(argv[1], "--check-standard-software-session") == 0;
    const bool check_standard_software_session_192 =
            argc == 2 &&
            std::strcmp(argv[1],
                        "--check-standard-software-session-192") == 0;
    if (argc > 1 && !check_only && !check_software_session &&
        !check_standard_software_session &&
        !check_standard_software_session_192) {
        std::fprintf(stderr,
                     "Usage: %s [--check|--check-software-session|"
                     "--check-standard-software-session|"
                     "--check-standard-software-session-192]\n",
                     argv[0]);
        return EXIT_FAILURE;
    }
    ::android::hardware::configureRpcThreadpool(4, false);
    if (check_standard_software_session ||
        check_standard_software_session_192) {
        const auto standard_factory =
                Aosp20::IBluetoothAudioProvidersFactory::tryGetService(
                        "default", false);
        if (standard_factory == nullptr) {
            std::fprintf(stderr,
                         "Standard Bluetooth Audio HIDL 2.0 factory is"
                         " unavailable\n");
            return EXIT_FAILURE;
        }
        return checkStandardSoftwareSession(
                standard_factory,
                check_standard_software_session_192
                        ? Aosp20::SampleRate::RATE_192000
                        : Aosp20::SampleRate::RATE_48000);
    }
    if (check_only) {
        const auto qti_factory =
                Qti21::IBluetoothAudioProvidersFactory::tryGetService(
                        "default", false);
        if (qti_factory == nullptr) {
            std::fprintf(stderr,
                         "QTI Bluetooth Audio HIDL 2.1 factory is unavailable\n");
            return EXIT_FAILURE;
        }
        return checkQtiBackend(qti_factory);
    }
    if (check_software_session) {
        const auto qti_factory =
                Qti21::IBluetoothAudioProvidersFactory::tryGetService(
                        "default", false);
        if (qti_factory == nullptr) {
            std::fprintf(stderr,
                         "QTI Bluetooth Audio HIDL 2.1 factory is unavailable\n");
            return EXIT_FAILURE;
        }
        return checkSoftwareSession(qti_factory);
    }
    const auto standard_factory =
            Aosp20::IBluetoothAudioProvidersFactory::getService("default",
                                                               false);
    if (standard_factory == nullptr) {
        LOGE("Standard Bluetooth Audio HIDL 2.0 factory is unavailable");
        std::fprintf(stderr,
                     "Standard Bluetooth Audio HIDL 2.0 factory is"
                     " unavailable\n");
        return EXIT_FAILURE;
    }
    const auto qti_factory =
            Qti21::IBluetoothAudioProvidersFactory::tryGetService("default",
                                                                  false);
    if (qti_factory == nullptr) {
        LOGW("QTI HIDL 2.1 offload backend is unavailable; software sessions "
             "remain enabled");
    }
    if (!loadBinderRuntime()) {
        return EXIT_FAILURE;
    }
    if (!g_binder.set_thread_pool_max_thread_count(4)) {
        LOGE("Unable to configure Binder thread pool");
        return EXIT_FAILURE;
    }

    auto service = ::ndk::SharedRefBase::make<
            BluetoothAudioProviderFactoryBridge>(standard_factory,
                                                 qti_factory);
    auto binder = service->asBinder();
    g_binder.mark_vintf_stability(binder.get());
    const binder_exception_t status =
            g_binder.add_service(binder.get(), kServiceName);
    if (status != EX_NONE) {
        LOGE("Unable to register %s: %d", kServiceName, status);
        return EXIT_FAILURE;
    }

    if (qti_factory != nullptr) {
        LOGI("Registered %s with standard HIDL 2.0 software and QTI HIDL 2.1"
             " offload backends",
             kServiceName);
    } else {
        LOGI("Registered %s with the standard HIDL 2.0 software backend",
             kServiceName);
    }
    g_binder.join_thread_pool();
    return EXIT_FAILURE;
}
