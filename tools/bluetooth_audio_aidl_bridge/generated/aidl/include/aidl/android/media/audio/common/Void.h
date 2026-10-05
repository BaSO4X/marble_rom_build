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
class Void {
public:
  typedef std::true_type fixed_size;
  static const char* descriptor;


  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator==(const Void&) const {
    return std::tie() == std::tie();
  }
  inline bool operator<(const Void&) const {
    return std::tie() < std::tie();
  }
  inline bool operator!=(const Void& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const Void& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const Void& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const Void& _rhs) const {
    return !(_rhs < *this);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_VINTF;
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "Void{";
    _aidl_os << "}";
    return _aidl_os.str();
  }
};
static_assert(alignof(Void) == 1);
static_assert(sizeof(Void) == 1);
}  // namespace common
}  // namespace audio
}  // namespace media
}  // namespace android
}  // namespace aidl
