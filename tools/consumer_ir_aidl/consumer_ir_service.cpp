/*
 * Copyright (C) 2021 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "consumer-ir-aidl-marble"

#include <aidl/android/hardware/ir/BnConsumerIr.h>
#include <android/log.h>
#include <dlfcn.h>

#include <cerrno>
#include <climits>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "legacy_consumer_ir.h"

namespace {

using aidl::android::hardware::ir::BnConsumerIr;
using aidl::android::hardware::ir::ConsumerIrFreqRange;
using HwGetModule = int (*)(const char*, const hw_module_t**);

constexpr char kServiceName[] = "android.hardware.ir.IConsumerIr/default";

struct BinderRuntime {
    void* library = nullptr;
    binder_exception_t (*add_service)(AIBinder*, const char*) = nullptr;
    bool (*set_thread_pool_max_thread_count)(uint32_t) = nullptr;
    void (*join_thread_pool)() = nullptr;
    void (*mark_vintf_stability)(AIBinder*) = nullptr;
};

BinderRuntime g_binder;

bool loadSymbol(void* library, const char* name, void* destination,
                size_t destination_size) {
    void* symbol = dlsym(library, name);
    if (symbol == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Unable to resolve %s: %s", name, dlerror());
        return false;
    }
    if (destination_size != sizeof(symbol)) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Unexpected function pointer size for %s", name);
        return false;
    }
    std::memcpy(destination, &symbol, sizeof(symbol));
    return true;
}

#define LOAD_BINDER_SYMBOL(member, symbol_name) \
    loadSymbol(g_binder.library, symbol_name, &g_binder.member, \
               sizeof(g_binder.member))

bool loadBinderRuntime() {
    g_binder.library = dlopen("libbinder_ndk.so", RTLD_NOW | RTLD_LOCAL);
    if (g_binder.library == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Unable to load libbinder_ndk.so: %s", dlerror());
        return false;
    }
    return LOAD_BINDER_SYMBOL(add_service, "AServiceManager_addService") &&
           LOAD_BINDER_SYMBOL(set_thread_pool_max_thread_count,
                              "ABinderProcess_setThreadPoolMaxThreadCount") &&
           LOAD_BINDER_SYMBOL(join_thread_pool,
                              "ABinderProcess_joinThreadPool") &&
           LOAD_BINDER_SYMBOL(mark_vintf_stability,
                              "AIBinder_markVintfStability");
}

class ConsumerIr final : public BnConsumerIr {
  public:
    ConsumerIr(consumerir_device_t* device, void* libhardware)
        : device_(device), libhardware_(libhardware) {}

    ~ConsumerIr() override {
        if (device_ != nullptr && device_->common.close != nullptr) {
            device_->common.close(&device_->common);
        }
        if (libhardware_ != nullptr) {
            dlclose(libhardware_);
        }
    }

    ::ndk::ScopedAStatus getCarrierFreqs(
            std::vector<ConsumerIrFreqRange>* aidl_return) override {
        std::lock_guard<std::mutex> lock(device_mutex_);
        const int range_count = device_->get_num_carrier_freqs(device_);
        if (range_count < 0) {
            logLegacyError("get_num_carrier_freqs", range_count);
            return ::ndk::ScopedAStatus::fromServiceSpecificError(range_count);
        }
        if (range_count == 0) {
            aidl_return->clear();
            return ::ndk::ScopedAStatus::ok();
        }
        if (range_count > 1024) {
            __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                                "Legacy HAL returned an invalid range count: %d",
                                range_count);
            return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
        }

        std::vector<consumerir_freq_range_t> legacy_ranges(range_count);
        const int status = device_->get_carrier_freqs(
                device_, legacy_ranges.size(), legacy_ranges.data());
        if (status < 0) {
            logLegacyError("get_carrier_freqs", status);
            return ::ndk::ScopedAStatus::fromServiceSpecificError(status);
        }

        aidl_return->clear();
        aidl_return->reserve(legacy_ranges.size());
        for (const auto& range : legacy_ranges) {
            if (range.min <= 0 || range.max < range.min) {
                __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                                    "Legacy HAL returned an invalid range: %d-%d",
                                    range.min, range.max);
                return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
            }
            aidl_return->push_back({range.min, range.max});
        }
        return ::ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus transmit(
            int32_t carrier_freq_hz,
            const std::vector<int32_t>& pattern) override {
        if (carrier_freq_hz <= 0) {
            return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
        }
        if (pattern.empty() || pattern.size() > static_cast<size_t>(INT_MAX)) {
            return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
        }
        for (const int32_t duration_us : pattern) {
            if (duration_us <= 0) {
                return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
            }
        }

        std::lock_guard<std::mutex> lock(device_mutex_);
        const int status = device_->transmit(
                device_, carrier_freq_hz, pattern.data(),
                static_cast<int>(pattern.size()));
        if (status == 0) {
            return ::ndk::ScopedAStatus::ok();
        }
        logLegacyError("transmit", status);
        if (status == -EINVAL || status == -ENOTSUP) {
            return ::ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
        }
        return ::ndk::ScopedAStatus::fromServiceSpecificError(status);
    }

  private:
    static void logLegacyError(const char* operation, int status) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Legacy HAL %s failed: %d", operation, status);
    }

    consumerir_device_t* const device_;
    void* const libhardware_;
    std::mutex device_mutex_;
};

std::shared_ptr<ConsumerIr> openConsumerIr() {
    void* libhardware = dlopen("libhardware.so", RTLD_NOW | RTLD_LOCAL);
    if (libhardware == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Unable to load libhardware.so: %s", dlerror());
        return nullptr;
    }

    dlerror();
    auto get_module = reinterpret_cast<HwGetModule>(dlsym(libhardware, "hw_get_module"));
    const char* symbol_error = dlerror();
    if (symbol_error != nullptr || get_module == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Unable to resolve hw_get_module: %s",
                            symbol_error != nullptr ? symbol_error : "missing symbol");
        dlclose(libhardware);
        return nullptr;
    }

    const hw_module_t* module = nullptr;
    int status = get_module(CONSUMERIR_HARDWARE_MODULE_ID, &module);
    if (status != 0 || module == nullptr || module->methods == nullptr ||
        module->methods->open == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Unable to load legacy Consumer IR module: %d", status);
        dlclose(libhardware);
        return nullptr;
    }

    hw_device_t* hardware_device = nullptr;
    status = module->methods->open(module, CONSUMERIR_TRANSMITTER, &hardware_device);
    auto* device = reinterpret_cast<consumerir_device_t*>(hardware_device);
    if (status != 0 || device == nullptr || device->transmit == nullptr ||
        device->get_num_carrier_freqs == nullptr ||
        device->get_carrier_freqs == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Unable to open legacy Consumer IR transmitter: %d", status);
        if (hardware_device != nullptr && hardware_device->close != nullptr) {
            hardware_device->close(hardware_device);
        }
        dlclose(libhardware);
        return nullptr;
    }

    return ::ndk::SharedRefBase::make<ConsumerIr>(device, libhardware);
}

}  // namespace

int main() {
    if (!loadBinderRuntime()) {
        return EXIT_FAILURE;
    }
    auto service = openConsumerIr();
    if (service == nullptr) {
        return EXIT_FAILURE;
    }

    if (!g_binder.set_thread_pool_max_thread_count(0)) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Unable to configure the Binder thread pool");
        return EXIT_FAILURE;
    }

    auto binder = service->asBinder();
    g_binder.mark_vintf_stability(binder.get());
    const binder_exception_t status =
            g_binder.add_service(binder.get(), kServiceName);
    if (status != EX_NONE) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Unable to register %s: %d", kServiceName, status);
        return EXIT_FAILURE;
    }

    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Registered %s", kServiceName);
    g_binder.join_thread_pool();
    return EXIT_FAILURE;
}
