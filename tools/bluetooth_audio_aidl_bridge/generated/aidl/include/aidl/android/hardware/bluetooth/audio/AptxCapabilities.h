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
class AptxCapabilities {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  std::vector<int32_t> sampleRateHz;
  std::vector<::aidl::android::hardware::bluetooth::audio::ChannelMode> channelMode;
  std::vector<uint8_t> bitsPerSample;

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator==(const AptxCapabilities& _rhs) const {
    return std::tie(sampleRateHz, channelMode, bitsPerSample) == std::tie(_rhs.sampleRateHz, _rhs.channelMode, _rhs.bitsPerSample);
  }
  inline bool operator<(const AptxCapabilities& _rhs) const {
    return std::tie(sampleRateHz, channelMode, bitsPerSample) < std::tie(_rhs.sampleRateHz, _rhs.channelMode, _rhs.bitsPerSample);
  }
  inline bool operator!=(const AptxCapabilities& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const AptxCapabilities& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const AptxCapabilities& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const AptxCapabilities& _rhs) const {
    return !(_rhs < *this);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_VINTF;
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "AptxCapabilities{";
    _aidl_os << "sampleRateHz: " << ::android::internal::ToString(sampleRateHz);
    _aidl_os << ", channelMode: " << ::android::internal::ToString(channelMode);
    _aidl_os << ", bitsPerSample: " << ::android::internal::ToString(bitsPerSample);
    _aidl_os << "}";
    return _aidl_os.str();
  }
};
}  // namespace audio
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
}  // namespace aidl
