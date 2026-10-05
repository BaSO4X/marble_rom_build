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
#include <aidl/android/hardware/bluetooth/audio/AudioCapabilities.h>
#include <aidl/android/hardware/bluetooth/audio/IBluetoothAudioProvider.h>
#include <aidl/android/hardware/bluetooth/audio/SessionType.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl::android::hardware::bluetooth::audio {
class IBluetoothAudioProvider;
}  // namespace aidl::android::hardware::bluetooth::audio
namespace aidl {
namespace android {
namespace hardware {
namespace bluetooth {
namespace audio {
class IBluetoothAudioProviderFactoryDelegator;

class IBluetoothAudioProviderFactory : public ::ndk::ICInterface {
public:
  typedef IBluetoothAudioProviderFactoryDelegator DefaultDelegator;
  static const char* descriptor;
  IBluetoothAudioProviderFactory();
  virtual ~IBluetoothAudioProviderFactory();

  static inline const int32_t version = 3;
  static inline const std::string hash = "fead4df60244a5440283617064f184690414a685";
  static constexpr uint32_t TRANSACTION_getProviderCapabilities = FIRST_CALL_TRANSACTION + 0;
  static constexpr uint32_t TRANSACTION_openProvider = FIRST_CALL_TRANSACTION + 1;

  static std::shared_ptr<IBluetoothAudioProviderFactory> fromBinder(const ::ndk::SpAIBinder& binder);
  static binder_status_t writeToParcel(AParcel* parcel, const std::shared_ptr<IBluetoothAudioProviderFactory>& instance);
  static binder_status_t readFromParcel(const AParcel* parcel, std::shared_ptr<IBluetoothAudioProviderFactory>* instance);
  static bool setDefaultImpl(const std::shared_ptr<IBluetoothAudioProviderFactory>& impl);
  static const std::shared_ptr<IBluetoothAudioProviderFactory>& getDefaultImpl();
  virtual ::ndk::ScopedAStatus getProviderCapabilities(::aidl::android::hardware::bluetooth::audio::SessionType in_sessionType, std::vector<::aidl::android::hardware::bluetooth::audio::AudioCapabilities>* _aidl_return) = 0;
  virtual ::ndk::ScopedAStatus openProvider(::aidl::android::hardware::bluetooth::audio::SessionType in_sessionType, std::shared_ptr<::aidl::android::hardware::bluetooth::audio::IBluetoothAudioProvider>* _aidl_return) = 0;
  virtual ::ndk::ScopedAStatus getInterfaceVersion(int32_t* _aidl_return) = 0;
  virtual ::ndk::ScopedAStatus getInterfaceHash(std::string* _aidl_return) = 0;
private:
  static std::shared_ptr<IBluetoothAudioProviderFactory> default_impl;
};
class IBluetoothAudioProviderFactoryDefault : public IBluetoothAudioProviderFactory {
public:
  ::ndk::ScopedAStatus getProviderCapabilities(::aidl::android::hardware::bluetooth::audio::SessionType in_sessionType, std::vector<::aidl::android::hardware::bluetooth::audio::AudioCapabilities>* _aidl_return) override;
  ::ndk::ScopedAStatus openProvider(::aidl::android::hardware::bluetooth::audio::SessionType in_sessionType, std::shared_ptr<::aidl::android::hardware::bluetooth::audio::IBluetoothAudioProvider>* _aidl_return) override;
  ::ndk::ScopedAStatus getInterfaceVersion(int32_t* _aidl_return) override;
  ::ndk::ScopedAStatus getInterfaceHash(std::string* _aidl_return) override;
  ::ndk::SpAIBinder asBinder() override;
  bool isRemote() override;
};
}  // namespace audio
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
}  // namespace aidl
