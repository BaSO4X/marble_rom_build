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
#include <aidl/android/media/audio/common/AudioChannelLayout.h>
#include <aidl/android/media/audio/common/AudioDevice.h>
#include <aidl/android/media/audio/common/AudioSource.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl::android::media::audio::common {
class AudioDevice;
}  // namespace aidl::android::media::audio::common
namespace aidl {
namespace android {
namespace hardware {
namespace audio {
namespace common {
class RecordTrackMetadata {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  ::aidl::android::media::audio::common::AudioSource source = ::aidl::android::media::audio::common::AudioSource::SYS_RESERVED_INVALID;
  float gain = 0.000000f;
  std::optional<::aidl::android::media::audio::common::AudioDevice> destinationDevice;
  ::aidl::android::media::audio::common::AudioChannelLayout channelMask;
  std::vector<std::string> tags;

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator==(const RecordTrackMetadata& _rhs) const {
    return std::tie(source, gain, destinationDevice, channelMask, tags) == std::tie(_rhs.source, _rhs.gain, _rhs.destinationDevice, _rhs.channelMask, _rhs.tags);
  }
  inline bool operator<(const RecordTrackMetadata& _rhs) const {
    return std::tie(source, gain, destinationDevice, channelMask, tags) < std::tie(_rhs.source, _rhs.gain, _rhs.destinationDevice, _rhs.channelMask, _rhs.tags);
  }
  inline bool operator!=(const RecordTrackMetadata& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const RecordTrackMetadata& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const RecordTrackMetadata& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const RecordTrackMetadata& _rhs) const {
    return !(_rhs < *this);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_VINTF;
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "RecordTrackMetadata{";
    _aidl_os << "source: " << ::android::internal::ToString(source);
    _aidl_os << ", gain: " << ::android::internal::ToString(gain);
    _aidl_os << ", destinationDevice: " << ::android::internal::ToString(destinationDevice);
    _aidl_os << ", channelMask: " << ::android::internal::ToString(channelMask);
    _aidl_os << ", tags: " << ::android::internal::ToString(tags);
    _aidl_os << "}";
    return _aidl_os.str();
  }
};
}  // namespace common
}  // namespace audio
}  // namespace hardware
}  // namespace android
}  // namespace aidl
