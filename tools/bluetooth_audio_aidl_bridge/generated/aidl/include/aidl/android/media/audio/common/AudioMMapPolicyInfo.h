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
#include <aidl/android/media/audio/common/AudioDevice.h>
#include <aidl/android/media/audio/common/AudioMMapPolicy.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl::android::media::audio::common {
class AudioDevice;
}  // namespace aidl::android::media::audio::common
namespace aidl {
namespace android {
namespace media {
namespace audio {
namespace common {
class AudioMMapPolicyInfo {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  ::aidl::android::media::audio::common::AudioDevice device;
  ::aidl::android::media::audio::common::AudioMMapPolicy mmapPolicy = ::aidl::android::media::audio::common::AudioMMapPolicy::UNSPECIFIED;

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator==(const AudioMMapPolicyInfo& _rhs) const {
    return std::tie(device, mmapPolicy) == std::tie(_rhs.device, _rhs.mmapPolicy);
  }
  inline bool operator<(const AudioMMapPolicyInfo& _rhs) const {
    return std::tie(device, mmapPolicy) < std::tie(_rhs.device, _rhs.mmapPolicy);
  }
  inline bool operator!=(const AudioMMapPolicyInfo& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const AudioMMapPolicyInfo& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const AudioMMapPolicyInfo& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const AudioMMapPolicyInfo& _rhs) const {
    return !(_rhs < *this);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_VINTF;
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "AudioMMapPolicyInfo{";
    _aidl_os << "device: " << ::android::internal::ToString(device);
    _aidl_os << ", mmapPolicy: " << ::android::internal::ToString(mmapPolicy);
    _aidl_os << "}";
    return _aidl_os.str();
  }
};
}  // namespace common
}  // namespace audio
}  // namespace media
}  // namespace android
}  // namespace aidl
