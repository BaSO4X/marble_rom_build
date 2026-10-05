/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: cmd not shown due to `--omit_invocation`
 */
#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <android/binder_interface_utils.h>
#include <android/binder_parcelable_utils.h>
#include <android/binder_to_string.h>
#include <aidl/android/hardware/bluetooth/audio/ChannelMode.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl {
namespace android {
namespace hardware {
namespace bluetooth {
namespace audio {
class OpusConfiguration {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  int8_t pcmBitDepth = 0;
  int32_t samplingFrequencyHz = 0;
  int32_t frameDurationUs = 0;
  int32_t octetsPerFrame = 0;
  int8_t blocksPerSdu = 0;
  ::aidl::android::hardware::bluetooth::audio::ChannelMode channelMode = ::aidl::android::hardware::bluetooth::audio::ChannelMode(0);

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator==(const OpusConfiguration& _rhs) const {
    return std::tie(pcmBitDepth, samplingFrequencyHz, frameDurationUs, octetsPerFrame, blocksPerSdu, channelMode) == std::tie(_rhs.pcmBitDepth, _rhs.samplingFrequencyHz, _rhs.frameDurationUs, _rhs.octetsPerFrame, _rhs.blocksPerSdu, _rhs.channelMode);
  }
  inline bool operator<(const OpusConfiguration& _rhs) const {
    return std::tie(pcmBitDepth, samplingFrequencyHz, frameDurationUs, octetsPerFrame, blocksPerSdu, channelMode) < std::tie(_rhs.pcmBitDepth, _rhs.samplingFrequencyHz, _rhs.frameDurationUs, _rhs.octetsPerFrame, _rhs.blocksPerSdu, _rhs.channelMode);
  }
  inline bool operator!=(const OpusConfiguration& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const OpusConfiguration& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const OpusConfiguration& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const OpusConfiguration& _rhs) const {
    return !(_rhs < *this);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_VINTF;
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "OpusConfiguration{";
    _aidl_os << "pcmBitDepth: " << ::android::internal::ToString(pcmBitDepth);
    _aidl_os << ", samplingFrequencyHz: " << ::android::internal::ToString(samplingFrequencyHz);
    _aidl_os << ", frameDurationUs: " << ::android::internal::ToString(frameDurationUs);
    _aidl_os << ", octetsPerFrame: " << ::android::internal::ToString(octetsPerFrame);
    _aidl_os << ", blocksPerSdu: " << ::android::internal::ToString(blocksPerSdu);
    _aidl_os << ", channelMode: " << ::android::internal::ToString(channelMode);
    _aidl_os << "}";
    return _aidl_os.str();
  }
};
}  // namespace audio
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
}  // namespace aidl
