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
namespace hardware {
namespace common {
namespace fmq {
class GrantorDescriptor {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  int32_t fdIndex = 0;
  int32_t offset = 0;
  int64_t extent = 0L;

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator==(const GrantorDescriptor& _rhs) const {
    return std::tie(fdIndex, offset, extent) == std::tie(_rhs.fdIndex, _rhs.offset, _rhs.extent);
  }
  inline bool operator<(const GrantorDescriptor& _rhs) const {
    return std::tie(fdIndex, offset, extent) < std::tie(_rhs.fdIndex, _rhs.offset, _rhs.extent);
  }
  inline bool operator!=(const GrantorDescriptor& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const GrantorDescriptor& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const GrantorDescriptor& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const GrantorDescriptor& _rhs) const {
    return !(_rhs < *this);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_VINTF;
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "GrantorDescriptor{";
    _aidl_os << "fdIndex: " << ::android::internal::ToString(fdIndex);
    _aidl_os << ", offset: " << ::android::internal::ToString(offset);
    _aidl_os << ", extent: " << ::android::internal::ToString(extent);
    _aidl_os << "}";
    return _aidl_os.str();
  }
};
}  // namespace fmq
}  // namespace common
}  // namespace hardware
}  // namespace android
}  // namespace aidl
