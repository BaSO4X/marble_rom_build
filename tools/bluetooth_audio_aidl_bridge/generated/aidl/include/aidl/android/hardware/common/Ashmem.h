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
class Ashmem {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  ::ndk::ScopedFileDescriptor fd;
  int64_t size = 0L;

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator==(const Ashmem& _rhs) const {
    return std::tie(fd, size) == std::tie(_rhs.fd, _rhs.size);
  }
  inline bool operator<(const Ashmem& _rhs) const {
    return std::tie(fd, size) < std::tie(_rhs.fd, _rhs.size);
  }
  inline bool operator!=(const Ashmem& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const Ashmem& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const Ashmem& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const Ashmem& _rhs) const {
    return !(_rhs < *this);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_VINTF;
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "Ashmem{";
    _aidl_os << "fd: " << ::android::internal::ToString(fd);
    _aidl_os << ", size: " << ::android::internal::ToString(size);
    _aidl_os << "}";
    return _aidl_os.str();
  }
};
}  // namespace common
}  // namespace hardware
}  // namespace android
}  // namespace aidl
