#define LOG_TAG "vendor.qti.hardware.bluetooth_audio@2.1::BluetoothAudioPort"

#include <log/log.h>
#include <cutils/trace.h>
#include <hidl/HidlTransportSupport.h>

#include <hidl/Static.h>
#include <hwbinder/ProcessState.h>
#include <utils/Trace.h>
#include <android/hidl/manager/1.0/IServiceManager.h>
#include <vendor/qti/hardware/bluetooth_audio/2.1/BpHwBluetoothAudioPort.h>
#include <vendor/qti/hardware/bluetooth_audio/2.1/BnHwBluetoothAudioPort.h>
#include <vendor/qti/hardware/bluetooth_audio/2.1/BsBluetoothAudioPort.h>
#include <vendor/qti/hardware/bluetooth_audio/2.0/BpHwBluetoothAudioPort.h>
#include <android/hidl/base/1.0/BpHwBase.h>
#include <hidl/ServiceManagement.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace bluetooth_audio {
namespace V2_1 {

const char* IBluetoothAudioPort::descriptor("vendor.qti.hardware.bluetooth_audio@2.1::IBluetoothAudioPort");

__attribute__((constructor)) static void static_constructor() {
    ::android::hardware::details::getBnConstructorMap().set(IBluetoothAudioPort::descriptor,
            [](void *iIntf) -> ::android::sp<::android::hardware::IBinder> {
                return new BnHwBluetoothAudioPort(static_cast<IBluetoothAudioPort *>(iIntf));
            });
    ::android::hardware::details::getBsConstructorMap().set(IBluetoothAudioPort::descriptor,
            [](void *iIntf) -> ::android::sp<::android::hidl::base::V1_0::IBase> {
                return new BsBluetoothAudioPort(static_cast<IBluetoothAudioPort *>(iIntf));
            });
}

__attribute__((destructor))static void static_destructor() {
    ::android::hardware::details::getBnConstructorMap().erase(IBluetoothAudioPort::descriptor);
    ::android::hardware::details::getBsConstructorMap().erase(IBluetoothAudioPort::descriptor);
}

// Methods from ::vendor::qti::hardware::bluetooth_audio::V2_0::IBluetoothAudioPort follow.
// no default implementation for: ::android::hardware::Return<void> IBluetoothAudioPort::startStream()
// no default implementation for: ::android::hardware::Return<void> IBluetoothAudioPort::suspendStream()
// no default implementation for: ::android::hardware::Return<void> IBluetoothAudioPort::stopStream()
// no default implementation for: ::android::hardware::Return<void> IBluetoothAudioPort::getPresentationPosition(getPresentationPosition_cb _hidl_cb)
// no default implementation for: ::android::hardware::Return<void> IBluetoothAudioPort::updateAptxMode(uint16_t aptxMode)

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> IBluetoothAudioPort::interfaceChain(interfaceChain_cb _hidl_cb){
    _hidl_cb({
        ::vendor::qti::hardware::bluetooth_audio::V2_1::IBluetoothAudioPort::descriptor,
        ::vendor::qti::hardware::bluetooth_audio::V2_0::IBluetoothAudioPort::descriptor,
        ::android::hidl::base::V1_0::IBase::descriptor,
    });
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IBluetoothAudioPort::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    (void)fd;
    (void)options;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IBluetoothAudioPort::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    _hidl_cb(::vendor::qti::hardware::bluetooth_audio::V2_1::IBluetoothAudioPort::descriptor);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IBluetoothAudioPort::getHashChain(getHashChain_cb _hidl_cb){
    _hidl_cb({
        (uint8_t[32]){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* 0000000000000000000000000000000000000000000000000000000000000000 */,
        (uint8_t[32]){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* 0000000000000000000000000000000000000000000000000000000000000000 */,
        (uint8_t[32]){120,233,176,62,32,72,41,29,11,168,127,128,30,169,1,141,212,222,243,135,6,69,150,222,221,194,147,185,162,93,181,205} /* 78e9b03e2048291d0ba87f801ea9018dd4def387064596deddc293b9a25db5cd */});
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IBluetoothAudioPort::setHALInstrumentation(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IBluetoothAudioPort::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    (void)cookie;
    return (recipient != nullptr);
}

::android::hardware::Return<void> IBluetoothAudioPort::ping(){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IBluetoothAudioPort::getDebugInfo(getDebugInfo_cb _hidl_cb){
    ::android::hidl::base::V1_0::DebugInfo info = {};
    info.pid = -1;
    info.ptr = 0;
    info.arch =
    #if defined(__LP64__)
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_64BIT
    #else
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_32BIT
    #endif
    ;
    _hidl_cb(info);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IBluetoothAudioPort::notifySyspropsChanged(){
    ::android::report_sysprop_change();
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IBluetoothAudioPort::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    return (recipient != nullptr);
}


::android::hardware::Return<::android::sp<::vendor::qti::hardware::bluetooth_audio::V2_1::IBluetoothAudioPort>> IBluetoothAudioPort::castFrom(const ::android::sp<::vendor::qti::hardware::bluetooth_audio::V2_1::IBluetoothAudioPort>& parent, bool /* emitError */) {
    return parent;
}

::android::hardware::Return<::android::sp<::vendor::qti::hardware::bluetooth_audio::V2_1::IBluetoothAudioPort>> IBluetoothAudioPort::castFrom(const ::android::sp<::vendor::qti::hardware::bluetooth_audio::V2_0::IBluetoothAudioPort>& parent, bool emitError) {
    return ::android::hardware::details::castInterface<IBluetoothAudioPort, ::vendor::qti::hardware::bluetooth_audio::V2_0::IBluetoothAudioPort, BpHwBluetoothAudioPort>(
            parent, "vendor.qti.hardware.bluetooth_audio@2.1::IBluetoothAudioPort", emitError);
}

::android::hardware::Return<::android::sp<::vendor::qti::hardware::bluetooth_audio::V2_1::IBluetoothAudioPort>> IBluetoothAudioPort::castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError) {
    return ::android::hardware::details::castInterface<IBluetoothAudioPort, ::android::hidl::base::V1_0::IBase, BpHwBluetoothAudioPort>(
            parent, "vendor.qti.hardware.bluetooth_audio@2.1::IBluetoothAudioPort", emitError);
}

BpHwBluetoothAudioPort::BpHwBluetoothAudioPort(const ::android::sp<::android::hardware::IBinder> &_hidl_impl)
        : BpInterface<IBluetoothAudioPort>(_hidl_impl),
          ::android::hardware::details::HidlInstrumentor("vendor.qti.hardware.bluetooth_audio@2.1", "IBluetoothAudioPort") {
}

void BpHwBluetoothAudioPort::onLastStrongRef(const void* id) {
    {
        std::unique_lock<std::mutex> lock(_hidl_mMutex);
        _hidl_mDeathRecipients.clear();
    }

    BpInterface<IBluetoothAudioPort>::onLastStrongRef(id);
}

// Methods from ::vendor::qti::hardware::bluetooth_audio::V2_0::IBluetoothAudioPort follow.
::android::hardware::Return<void> BpHwBluetoothAudioPort::startStream(){
    ::android::hardware::Return<void>  _hidl_out = ::vendor::qti::hardware::bluetooth_audio::V2_0::BpHwBluetoothAudioPort::_hidl_startStream(this, this);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwBluetoothAudioPort::suspendStream(){
    ::android::hardware::Return<void>  _hidl_out = ::vendor::qti::hardware::bluetooth_audio::V2_0::BpHwBluetoothAudioPort::_hidl_suspendStream(this, this);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwBluetoothAudioPort::stopStream(){
    ::android::hardware::Return<void>  _hidl_out = ::vendor::qti::hardware::bluetooth_audio::V2_0::BpHwBluetoothAudioPort::_hidl_stopStream(this, this);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwBluetoothAudioPort::getPresentationPosition(getPresentationPosition_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::vendor::qti::hardware::bluetooth_audio::V2_0::BpHwBluetoothAudioPort::_hidl_getPresentationPosition(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwBluetoothAudioPort::updateAptxMode(uint16_t aptxMode){
    ::android::hardware::Return<void>  _hidl_out = ::vendor::qti::hardware::bluetooth_audio::V2_0::BpHwBluetoothAudioPort::_hidl_updateAptxMode(this, this, aptxMode);

    return _hidl_out;
}


// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> BpHwBluetoothAudioPort::interfaceChain(interfaceChain_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_interfaceChain(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwBluetoothAudioPort::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_debug(this, this, fd, options);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwBluetoothAudioPort::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_interfaceDescriptor(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwBluetoothAudioPort::getHashChain(getHashChain_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_getHashChain(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwBluetoothAudioPort::setHALInstrumentation(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_setHALInstrumentation(this, this);

    return _hidl_out;
}

::android::hardware::Return<bool> BpHwBluetoothAudioPort::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    ::android::hardware::ProcessState::self()->startThreadPool();
    ::android::hardware::hidl_binder_death_recipient *binder_recipient = new ::android::hardware::hidl_binder_death_recipient(recipient, cookie, this);
    std::unique_lock<std::mutex> lock(_hidl_mMutex);
    _hidl_mDeathRecipients.push_back(binder_recipient);
    return (remote()->linkToDeath(binder_recipient) == ::android::OK);
}

::android::hardware::Return<void> BpHwBluetoothAudioPort::ping(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_ping(this, this);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwBluetoothAudioPort::getDebugInfo(getDebugInfo_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_getDebugInfo(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwBluetoothAudioPort::notifySyspropsChanged(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_notifySyspropsChanged(this, this);

    return _hidl_out;
}

::android::hardware::Return<bool> BpHwBluetoothAudioPort::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    std::unique_lock<std::mutex> lock(_hidl_mMutex);
    for (auto it = _hidl_mDeathRecipients.rbegin();it != _hidl_mDeathRecipients.rend();++it) {
        if ((*it)->getRecipient() == recipient) {
            ::android::status_t status = remote()->unlinkToDeath(*it);
            _hidl_mDeathRecipients.erase(it.base()-1);
            return status == ::android::OK;
        }
    }
    return false;
}


BnHwBluetoothAudioPort::BnHwBluetoothAudioPort(const ::android::sp<IBluetoothAudioPort> &_hidl_impl)
        : ::android::hidl::base::V1_0::BnHwBase(_hidl_impl, "vendor.qti.hardware.bluetooth_audio@2.1", "IBluetoothAudioPort") {
            _hidl_mImpl = _hidl_impl;
            auto prio = ::android::hardware::getMinSchedulerPolicy(_hidl_impl);
            mSchedPolicy = prio.sched_policy;
            mSchedPriority = prio.prio;
            setRequestingSid(::android::hardware::getRequestingSid(_hidl_impl));
}

BnHwBluetoothAudioPort::~BnHwBluetoothAudioPort() {
    ::android::hardware::details::gBnMap->eraseIfEqual(_hidl_mImpl.get(), this);
}


// Methods from ::vendor::qti::hardware::bluetooth_audio::V2_0::IBluetoothAudioPort follow.

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> BnHwBluetoothAudioPort::ping() {
    return ::android::hardware::Void();
}
::android::hardware::Return<void> BnHwBluetoothAudioPort::getDebugInfo(getDebugInfo_cb _hidl_cb) {
    ::android::hidl::base::V1_0::DebugInfo info = {};
    info.pid = ::android::hardware::details::getPidIfSharable();
    info.ptr = ::android::hardware::details::debuggable()? reinterpret_cast<uint64_t>(this) : 0;
    info.arch =
    #if defined(__LP64__)
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_64BIT
    #else
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_32BIT
    #endif
    ;
    _hidl_cb(info);
    return ::android::hardware::Void();
}

::android::status_t BnHwBluetoothAudioPort::onTransact(
        uint32_t _hidl_code,
        const ::android::hardware::Parcel &_hidl_data,
        ::android::hardware::Parcel *_hidl_reply,
        uint32_t _hidl_flags,
        TransactCallback _hidl_cb) {
    ::android::status_t _hidl_err = ::android::OK;

    switch (_hidl_code) {
        case 1 /* startStream */:
        {
            _hidl_err = ::vendor::qti::hardware::bluetooth_audio::V2_0::BnHwBluetoothAudioPort::_hidl_startStream(this, _hidl_data, _hidl_reply, _hidl_cb);
            break;
        }

        case 2 /* suspendStream */:
        {
            _hidl_err = ::vendor::qti::hardware::bluetooth_audio::V2_0::BnHwBluetoothAudioPort::_hidl_suspendStream(this, _hidl_data, _hidl_reply, _hidl_cb);
            break;
        }

        case 3 /* stopStream */:
        {
            _hidl_err = ::vendor::qti::hardware::bluetooth_audio::V2_0::BnHwBluetoothAudioPort::_hidl_stopStream(this, _hidl_data, _hidl_reply, _hidl_cb);
            break;
        }

        case 4 /* getPresentationPosition */:
        {
            _hidl_err = ::vendor::qti::hardware::bluetooth_audio::V2_0::BnHwBluetoothAudioPort::_hidl_getPresentationPosition(this, _hidl_data, _hidl_reply, _hidl_cb);
            break;
        }

        case 5 /* updateAptxMode */:
        {
            _hidl_err = ::vendor::qti::hardware::bluetooth_audio::V2_0::BnHwBluetoothAudioPort::_hidl_updateAptxMode(this, _hidl_data, _hidl_reply, _hidl_cb);
            break;
        }

        default:
        {
            return ::android::hidl::base::V1_0::BnHwBase::onTransact(
                    _hidl_code, _hidl_data, _hidl_reply, _hidl_flags, _hidl_cb);
        }
    }

    if (_hidl_err == ::android::UNEXPECTED_NULL) {
        _hidl_err = ::android::hardware::writeToParcel(
                ::android::hardware::Status::fromExceptionCode(::android::hardware::Status::EX_NULL_POINTER),
                _hidl_reply);
    }return _hidl_err;
}

BsBluetoothAudioPort::BsBluetoothAudioPort(const ::android::sp<::vendor::qti::hardware::bluetooth_audio::V2_1::IBluetoothAudioPort> impl) : ::android::hardware::details::HidlInstrumentor("vendor.qti.hardware.bluetooth_audio@2.1", "IBluetoothAudioPort"), mImpl(impl) {
    mOnewayQueue.start(3000 /* similar limit to binderized */);
}

::android::hardware::Return<void> BsBluetoothAudioPort::addOnewayTask(std::function<void(void)> fun) {
    if (!mOnewayQueue.push(fun)) {
        return ::android::hardware::Status::fromExceptionCode(
                ::android::hardware::Status::EX_TRANSACTION_FAILED,
                "Passthrough oneway function queue exceeds maximum size.");
    }
    return ::android::hardware::Status();
}

::android::sp<IBluetoothAudioPort> IBluetoothAudioPort::tryGetService(const std::string &serviceName, const bool getStub) {
    return ::android::hardware::details::getServiceInternal<BpHwBluetoothAudioPort>(serviceName, false, getStub);
}

::android::sp<IBluetoothAudioPort> IBluetoothAudioPort::getService(const std::string &serviceName, const bool getStub) {
    return ::android::hardware::details::getServiceInternal<BpHwBluetoothAudioPort>(serviceName, true, getStub);
}

::android::status_t IBluetoothAudioPort::registerAsService(const std::string &serviceName) {
    return ::android::hardware::details::registerAsServiceInternal(this, serviceName);
}

bool IBluetoothAudioPort::registerForNotifications(
        const std::string &serviceName,
        const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification) {
    const ::android::sp<::android::hidl::manager::V1_0::IServiceManager> sm
            = ::android::hardware::defaultServiceManager();
    if (sm == nullptr) {
        return false;
    }
    ::android::hardware::Return<bool> success =
            sm->registerForNotifications("vendor.qti.hardware.bluetooth_audio@2.1::IBluetoothAudioPort",
                    serviceName, notification);
    return success.isOk() && success;
}

static_assert(sizeof(::android::hardware::MQDescriptor<char, ::android::hardware::kSynchronizedReadWrite>) == 32, "wrong size");
static_assert(sizeof(::android::hardware::hidl_handle) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_memory) == 40, "wrong size");
static_assert(sizeof(::android::hardware::hidl_string) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_vec<char>) == 16, "wrong size");

}  // namespace V2_1
}  // namespace bluetooth_audio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
