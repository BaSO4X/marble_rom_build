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
#include <aidl/android/hardware/bluetooth/audio/CodecType.h>
#include <aidl/android/hardware/bluetooth/audio/LeAudioCodecConfiguration.h>
#include <aidl/android/hardware/bluetooth/audio/LeAudioConfiguration.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl {
namespace android {
namespace hardware {
namespace bluetooth {
namespace audio {
class LeAudioConfiguration {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  class StreamMap {
  public:
    typedef std::false_type fixed_size;
    static const char* descriptor;

    char16_t streamHandle = '\0';
    int32_t audioChannelAllocation = 0;
    bool isStreamActive = false;

    binder_status_t readFromParcel(const AParcel* parcel);
    binder_status_t writeToParcel(AParcel* parcel) const;

    inline bool operator==(const StreamMap& _rhs) const {
      return std::tie(streamHandle, audioChannelAllocation, isStreamActive) == std::tie(_rhs.streamHandle, _rhs.audioChannelAllocation, _rhs.isStreamActive);
    }
    inline bool operator<(const StreamMap& _rhs) const {
      return std::tie(streamHandle, audioChannelAllocation, isStreamActive) < std::tie(_rhs.streamHandle, _rhs.audioChannelAllocation, _rhs.isStreamActive);
    }
    inline bool operator!=(const StreamMap& _rhs) const {
      return !(*this == _rhs);
    }
    inline bool operator>(const StreamMap& _rhs) const {
      return _rhs < *this;
    }
    inline bool operator>=(const StreamMap& _rhs) const {
      return !(*this < _rhs);
    }
    inline bool operator<=(const StreamMap& _rhs) const {
      return !(_rhs < *this);
    }

    static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_VINTF;
    inline std::string toString() const {
      std::ostringstream _aidl_os;
      _aidl_os << "StreamMap{";
      _aidl_os << "streamHandle: " << ::android::internal::ToString(streamHandle);
      _aidl_os << ", audioChannelAllocation: " << ::android::internal::ToString(audioChannelAllocation);
      _aidl_os << ", isStreamActive: " << ::android::internal::ToString(isStreamActive);
      _aidl_os << "}";
      return _aidl_os.str();
    }
  };
  ::aidl::android::hardware::bluetooth::audio::CodecType codecType = ::aidl::android::hardware::bluetooth::audio::CodecType(0);
  std::vector<::aidl::android::hardware::bluetooth::audio::LeAudioConfiguration::StreamMap> streamMap;
  int32_t peerDelayUs = 0;
  ::aidl::android::hardware::bluetooth::audio::LeAudioCodecConfiguration leAudioCodecConfig;
  std::optional<std::vector<uint8_t>> vendorSpecificMetadata;

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator==(const LeAudioConfiguration& _rhs) const {
    return std::tie(codecType, streamMap, peerDelayUs, leAudioCodecConfig, vendorSpecificMetadata) == std::tie(_rhs.codecType, _rhs.streamMap, _rhs.peerDelayUs, _rhs.leAudioCodecConfig, _rhs.vendorSpecificMetadata);
  }
  inline bool operator<(const LeAudioConfiguration& _rhs) const {
    return std::tie(codecType, streamMap, peerDelayUs, leAudioCodecConfig, vendorSpecificMetadata) < std::tie(_rhs.codecType, _rhs.streamMap, _rhs.peerDelayUs, _rhs.leAudioCodecConfig, _rhs.vendorSpecificMetadata);
  }
  inline bool operator!=(const LeAudioConfiguration& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const LeAudioConfiguration& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const LeAudioConfiguration& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const LeAudioConfiguration& _rhs) const {
    return !(_rhs < *this);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_VINTF;
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "LeAudioConfiguration{";
    _aidl_os << "codecType: " << ::android::internal::ToString(codecType);
    _aidl_os << ", streamMap: " << ::android::internal::ToString(streamMap);
    _aidl_os << ", peerDelayUs: " << ::android::internal::ToString(peerDelayUs);
    _aidl_os << ", leAudioCodecConfig: " << ::android::internal::ToString(leAudioCodecConfig);
    _aidl_os << ", vendorSpecificMetadata: " << ::android::internal::ToString(vendorSpecificMetadata);
    _aidl_os << "}";
    return _aidl_os.str();
  }
};
}  // namespace audio
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
}  // namespace aidl
