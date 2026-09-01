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
class AudioHalCapCriterion {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  std::string name;
  std::string criterionTypeName;
  std::string defaultLiteralValue;

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator==(const AudioHalCapCriterion& _rhs) const {
    return std::tie(name, criterionTypeName, defaultLiteralValue) == std::tie(_rhs.name, _rhs.criterionTypeName, _rhs.defaultLiteralValue);
  }
  inline bool operator<(const AudioHalCapCriterion& _rhs) const {
    return std::tie(name, criterionTypeName, defaultLiteralValue) < std::tie(_rhs.name, _rhs.criterionTypeName, _rhs.defaultLiteralValue);
  }
  inline bool operator!=(const AudioHalCapCriterion& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const AudioHalCapCriterion& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const AudioHalCapCriterion& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const AudioHalCapCriterion& _rhs) const {
    return !(_rhs < *this);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_VINTF;
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "AudioHalCapCriterion{";
    _aidl_os << "name: " << ::android::internal::ToString(name);
    _aidl_os << ", criterionTypeName: " << ::android::internal::ToString(criterionTypeName);
    _aidl_os << ", defaultLiteralValue: " << ::android::internal::ToString(defaultLiteralValue);
    _aidl_os << "}";
    return _aidl_os.str();
  }
};
}  // namespace common
}  // namespace audio
}  // namespace media
}  // namespace android
}  // namespace aidl
