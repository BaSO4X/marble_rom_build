/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: cmd not shown due to `--omit_invocation`
 */
#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <android/binder_enums.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl {
namespace android {
namespace media {
namespace audio {
namespace common {
enum class AudioLatencyMode : int8_t {
  FREE = 0,
  LOW = 1,
  DYNAMIC_SPATIAL_AUDIO_SOFTWARE = 2,
  DYNAMIC_SPATIAL_AUDIO_HARDWARE = 3,
};

}  // namespace common
}  // namespace audio
}  // namespace media
}  // namespace android
}  // namespace aidl
namespace aidl {
namespace android {
namespace media {
namespace audio {
namespace common {
[[nodiscard]] static inline std::string toString(AudioLatencyMode val) {
  switch(val) {
  case AudioLatencyMode::FREE:
    return "FREE";
  case AudioLatencyMode::LOW:
    return "LOW";
  case AudioLatencyMode::DYNAMIC_SPATIAL_AUDIO_SOFTWARE:
    return "DYNAMIC_SPATIAL_AUDIO_SOFTWARE";
  case AudioLatencyMode::DYNAMIC_SPATIAL_AUDIO_HARDWARE:
    return "DYNAMIC_SPATIAL_AUDIO_HARDWARE";
  default:
    return std::to_string(static_cast<int8_t>(val));
  }
}
}  // namespace common
}  // namespace audio
}  // namespace media
}  // namespace android
}  // namespace aidl
namespace ndk {
namespace internal {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template <>
constexpr inline std::array<aidl::android::media::audio::common::AudioLatencyMode, 4> enum_values<aidl::android::media::audio::common::AudioLatencyMode> = {
  aidl::android::media::audio::common::AudioLatencyMode::FREE,
  aidl::android::media::audio::common::AudioLatencyMode::LOW,
  aidl::android::media::audio::common::AudioLatencyMode::DYNAMIC_SPATIAL_AUDIO_SOFTWARE,
  aidl::android::media::audio::common::AudioLatencyMode::DYNAMIC_SPATIAL_AUDIO_HARDWARE,
};
#pragma clang diagnostic pop
}  // namespace internal
}  // namespace ndk
