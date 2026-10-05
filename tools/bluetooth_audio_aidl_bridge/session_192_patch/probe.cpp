/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Runtime ABI probe for a rebuilt libbluetooth_audio_session.so. The probe
 * loads the library by path and exercises both the HIDL 2.0 and 2.1 PCM
 * capability/validation entry points through the device platform libc++ ABI.
 */

#include <android/hardware/bluetooth/audio/2.0/types.h>
#include <android/hardware/bluetooth/audio/2.1/types.h>

#include <dlfcn.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace {

namespace V2_0 = ::android::hardware::bluetooth::audio::V2_0;
namespace V2_1 = ::android::hardware::bluetooth::audio::V2_1;

using GetCapabilities2_0 = std::vector<V2_0::PcmParameters> (*)();
using GetCapabilities2_1 = std::vector<V2_1::PcmParameters> (*)();
using IsConfigurationValid2_0 = bool (*)(const V2_0::PcmParameters&);
using IsConfigurationValid2_1 = bool (*)(const V2_1::PcmParameters&);

constexpr char kGetCapabilities2_0Symbol[] =
    "_ZN7android9bluetooth5audio26GetSoftwarePcmCapabilitiesEv";
constexpr char kGetCapabilities2_1Symbol[] =
    "_ZN7android9bluetooth5audio30GetSoftwarePcmCapabilities_2_1Ev";
constexpr char kIsConfigurationValid2_0Symbol[] =
    "_ZN7android9bluetooth5audio31IsSoftwarePcmConfigurationValidERKNS_8hardware9bluetooth5audio4V2_013PcmParametersE";
constexpr char kIsConfigurationValid2_1Symbol[] =
    "_ZN7android9bluetooth5audio35IsSoftwarePcmConfigurationValid_2_1ERKNS_8hardware9bluetooth5audio4V2_113PcmParametersE";

template <typename Function>
bool loadSymbol(void* library, const char* name, Function* function) {
  dlerror();
  void* symbol = dlsym(library, name);
  const char* error = dlerror();
  if (symbol == nullptr || error != nullptr ||
      sizeof(*function) != sizeof(symbol)) {
    std::fprintf(stderr, "cannot resolve %s: %s\n", name,
                 error == nullptr ? "null" : error);
    return false;
  }
  std::memcpy(function, &symbol, sizeof(symbol));
  return true;
}

bool parseMask(const char* text, uint32_t* mask) {
  errno = 0;
  char* end = nullptr;
  const unsigned long value = std::strtoul(text, &end, 0);
  if (errno != 0 || end == text || *end != '\0' || value > UINT32_MAX) {
    return false;
  }
  *mask = static_cast<uint32_t>(value);
  return true;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 3) {
    std::fprintf(stderr, "Usage: %s SESSION_LIBRARY EXPECTED_MASK\n", argv[0]);
    return 2;
  }
  uint32_t expected_mask = 0;
  if (!parseMask(argv[2], &expected_mask)) {
    std::fprintf(stderr, "invalid expected mask: %s\n", argv[2]);
    return 3;
  }

  void* library = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
  if (library == nullptr) {
    std::fprintf(stderr, "dlopen failed: %s\n", dlerror());
    return 4;
  }

  GetCapabilities2_0 get_capabilities_2_0 = nullptr;
  GetCapabilities2_1 get_capabilities_2_1 = nullptr;
  IsConfigurationValid2_0 is_valid_2_0 = nullptr;
  IsConfigurationValid2_1 is_valid_2_1 = nullptr;
  if (!loadSymbol(library, kGetCapabilities2_0Symbol,
                  &get_capabilities_2_0) ||
      !loadSymbol(library, kGetCapabilities2_1Symbol,
                  &get_capabilities_2_1) ||
      !loadSymbol(library, kIsConfigurationValid2_0Symbol, &is_valid_2_0) ||
      !loadSymbol(library, kIsConfigurationValid2_1Symbol, &is_valid_2_1)) {
    dlclose(library);
    return 5;
  }

  const std::vector<V2_0::PcmParameters> capabilities_2_0 =
      get_capabilities_2_0();
  const std::vector<V2_1::PcmParameters> capabilities_2_1 =
      get_capabilities_2_1();
  const uint32_t mask_2_0 =
      capabilities_2_0.empty()
          ? 0
          : static_cast<uint32_t>(capabilities_2_0[0].sampleRate);
  const uint32_t mask_2_1 =
      capabilities_2_1.empty()
          ? 0
          : static_cast<uint32_t>(capabilities_2_1[0].sampleRate);
  if (capabilities_2_0.size() != 1 || capabilities_2_1.size() != 1 ||
      mask_2_0 != expected_mask || mask_2_1 != expected_mask) {
    std::fprintf(stderr,
                 "unexpected capability count/mask: 2.0=%zu/0x%x "
                 "2.1=%zu/0x%x expected=0x%x\n",
                 capabilities_2_0.size(), mask_2_0,
                 capabilities_2_1.size(), mask_2_1, expected_mask);
    dlclose(library);
    return 6;
  }

  V2_0::PcmParameters config_2_0{};
  config_2_0.sampleRate = V2_0::SampleRate::RATE_192000;
  config_2_0.channelMode = V2_0::ChannelMode::STEREO;
  config_2_0.bitsPerSample = V2_0::BitsPerSample::BITS_24;
  V2_1::PcmParameters config_2_1{};
  config_2_1.sampleRate = V2_1::SampleRate::RATE_192000;
  config_2_1.channelMode = V2_0::ChannelMode::STEREO;
  config_2_1.bitsPerSample = V2_0::BitsPerSample::BITS_24;
  config_2_1.dataIntervalUs = 20000;

  const bool expect_192 =
      (expected_mask & static_cast<uint32_t>(V2_0::SampleRate::RATE_192000)) !=
      0;
  if (is_valid_2_0(config_2_0) != expect_192 ||
      is_valid_2_1(config_2_1) != expect_192) {
    std::fprintf(stderr, "192 kHz validation mismatch: expected=%d\n",
                 expect_192);
    dlclose(library);
    return 7;
  }

  config_2_0.sampleRate = V2_0::SampleRate::RATE_48000;
  config_2_1.sampleRate = V2_1::SampleRate::RATE_48000;
  if (!is_valid_2_0(config_2_0) || !is_valid_2_1(config_2_1)) {
    std::fprintf(stderr, "48 kHz regression\n");
    dlclose(library);
    return 8;
  }
  config_2_0.sampleRate = V2_0::SampleRate::RATE_UNKNOWN;
  config_2_1.sampleRate = V2_1::SampleRate::RATE_UNKNOWN;
  if (is_valid_2_0(config_2_0) || is_valid_2_1(config_2_1)) {
    std::fprintf(stderr, "unknown sample rate accepted\n");
    dlclose(library);
    return 9;
  }

  std::printf("session_pcm_probe=PASS mask_2_0=0x%x mask_2_1=0x%x "
              "valid_192=%d\n",
              mask_2_0, mask_2_1, expect_192);
  dlclose(library);
  return 0;
}
