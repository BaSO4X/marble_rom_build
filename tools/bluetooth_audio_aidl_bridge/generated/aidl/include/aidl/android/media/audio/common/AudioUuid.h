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
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl {
namespace android {
namespace media {
namespace audio {
namespace common {
class AudioUuid {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  int32_t timeLow = 0;
  int32_t timeMid = 0;
  int32_t timeHiAndVersion = 0;
  int32_t clockSeq = 0;
  std::vector<uint8_t> node;

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator==(const AudioUuid& _rhs) const {
    return std::tie(timeLow, timeMid, timeHiAndVersion, clockSeq, node) == std::tie(_rhs.timeLow, _rhs.timeMid, _rhs.timeHiAndVersion, _rhs.clockSeq, _rhs.node);
  }
  inline bool operator<(const AudioUuid& _rhs) const {
    return std::tie(timeLow, timeMid, timeHiAndVersion, clockSeq, node) < std::tie(_rhs.timeLow, _rhs.timeMid, _rhs.timeHiAndVersion, _rhs.clockSeq, _rhs.node);
  }
  inline bool operator!=(const AudioUuid& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const AudioUuid& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const AudioUuid& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const AudioUuid& _rhs) const {
    return !(_rhs < *this);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_VINTF;
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "AudioUuid{";
    _aidl_os << "timeLow: " << ::android::internal::ToString(timeLow);
    _aidl_os << ", timeMid: " << ::android::internal::ToString(timeMid);
    _aidl_os << ", timeHiAndVersion: " << ::android::internal::ToString(timeHiAndVersion);
    _aidl_os << ", clockSeq: " << ::android::internal::ToString(clockSeq);
    _aidl_os << ", node: " << ::android::internal::ToString(node);
    _aidl_os << "}";
    return _aidl_os.str();
  }
};
}  // namespace common
}  // namespace audio
}  // namespace media
}  // namespace android
}  // namespace aidl
