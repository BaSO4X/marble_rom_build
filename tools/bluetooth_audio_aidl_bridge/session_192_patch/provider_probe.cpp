/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Loads the stock HIDL 2.0 provider and a selected session library in one
 * process, then starts a software PCM session. This validates the complete
 * provider -> session -> FMQ path without registering or replacing a service.
 */

#include <android/hardware/bluetooth/audio/2.0/IBluetoothAudioPort.h>
#include <android/hardware/bluetooth/audio/2.0/IBluetoothAudioProvider.h>
#include <android/hardware/bluetooth/audio/2.0/IBluetoothAudioProvidersFactory.h>
#include <fmq/MessageQueue.h>

#include <dlfcn.h>

#include <cstdio>
#include <cstring>
#include <unistd.h>

namespace {

namespace V2_0 = ::android::hardware::bluetooth::audio::V2_0;
using DataMQ = ::android::hardware::MessageQueue<
    uint8_t, ::android::hardware::kSynchronizedReadWrite>;
using FetchFactory = V2_0::IBluetoothAudioProvidersFactory* (*)(const char*);

class ProbePort final : public V2_0::IBluetoothAudioPort {
 public:
  ::android::hardware::Return<void> startStream() override { return {}; }
  ::android::hardware::Return<void> suspendStream() override { return {}; }
  ::android::hardware::Return<void> stopStream() override { return {}; }
  ::android::hardware::Return<void> getPresentationPosition(
      getPresentationPosition_cb callback) override {
    callback(V2_0::Status::SUCCESS, 0, 0, {});
    return {};
  }
  ::android::hardware::Return<void> updateMetadata(
      const ::android::hardware::audio::common::V5_0::SourceMetadata&)
      override {
    return {};
  }
};

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

[[noreturn]] void finishWithoutProviderDestructors(int status) {
  std::fflush(nullptr);
  _exit(status);
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 2) {
    std::fprintf(stderr, "Usage: %s PROVIDER_LIBRARY\n", argv[0]);
    return 2;
  }

  void* provider_library = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
  if (provider_library == nullptr) {
    std::fprintf(stderr, "provider dlopen failed: %s\n", dlerror());
    return 3;
  }
  FetchFactory fetch_factory = nullptr;
  if (!loadSymbol(provider_library,
                  "HIDL_FETCH_IBluetoothAudioProvidersFactory",
                  &fetch_factory)) {
    finishWithoutProviderDestructors(4);
  }

  ::android::sp<V2_0::IBluetoothAudioProvidersFactory> factory =
      fetch_factory("default");
  if (factory == nullptr) {
    std::fprintf(stderr, "factory fetch returned null\n");
    finishWithoutProviderDestructors(5);
  }

  uint32_t sample_rate_mask = 0;
  auto capabilities_return = factory->getProviderCapabilities(
      V2_0::SessionType::A2DP_SOFTWARE_ENCODING_DATAPATH,
      [&](const ::android::hardware::hidl_vec<V2_0::AudioCapabilities>& caps) {
        if (caps.size() == 1 &&
            caps[0].getDiscriminator() ==
                V2_0::AudioCapabilities::hidl_discriminator::pcmCapabilities) {
          sample_rate_mask =
              static_cast<uint32_t>(caps[0].pcmCapabilities().sampleRate);
        }
      });
  if (!capabilities_return.isOk() || sample_rate_mask != 0xef) {
    std::fprintf(stderr, "unexpected provider capability: transport=%d mask=0x%x\n",
                 capabilities_return.isOk(), sample_rate_mask);
    finishWithoutProviderDestructors(6);
  }

  V2_0::Status open_status = V2_0::Status::FAILURE;
  ::android::sp<V2_0::IBluetoothAudioProvider> provider;
  auto open_return = factory->openProvider(
      V2_0::SessionType::A2DP_SOFTWARE_ENCODING_DATAPATH,
      [&](V2_0::Status status,
          const ::android::sp<V2_0::IBluetoothAudioProvider>& opened) {
        open_status = status;
        provider = opened;
      });
  if (!open_return.isOk() || open_status != V2_0::Status::SUCCESS ||
      provider == nullptr) {
    std::fprintf(stderr, "openProvider failed: transport=%d status=%d\n",
                 open_return.isOk(), static_cast<int>(open_status));
    finishWithoutProviderDestructors(7);
  }

  V2_0::PcmParameters pcm{};
  pcm.sampleRate = V2_0::SampleRate::RATE_192000;
  pcm.channelMode = V2_0::ChannelMode::STEREO;
  pcm.bitsPerSample = V2_0::BitsPerSample::BITS_24;
  V2_0::AudioConfiguration configuration;
  configuration.pcmConfig(pcm);
  ::android::sp<ProbePort> port = new ProbePort();
  V2_0::Status start_status = V2_0::Status::FAILURE;
  bool data_mq_valid = false;
  auto start_return = provider->startSession(
      port, configuration,
      [&](V2_0::Status status,
          const ::android::hardware::MQDescriptorSync<uint8_t>& descriptor) {
        start_status = status;
        DataMQ data_mq(descriptor);
        data_mq_valid = data_mq.isValid();
      });
  if (!start_return.isOk() || start_status != V2_0::Status::SUCCESS ||
      !data_mq_valid) {
    std::fprintf(stderr,
                 "startSession failed: transport=%d status=%d data_mq=%d\n",
                 start_return.isOk(), static_cast<int>(start_status),
                 data_mq_valid);
    if (start_status == V2_0::Status::SUCCESS) {
      provider->endSession();
    }
    finishWithoutProviderDestructors(8);
  }

  const auto end_return = provider->endSession();
  if (!end_return.isOk()) {
    std::fprintf(stderr, "endSession transport failed\n");
    finishWithoutProviderDestructors(9);
  }

  std::printf("provider_session_pcm192_probe=PASS mask=0x%x data_mq_valid=1\n",
              sample_rate_mask);
  finishWithoutProviderDestructors(0);
}
