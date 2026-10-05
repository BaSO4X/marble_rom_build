// Restores the verified private initialization required by the marble Dolby
// AC-4 OMX decoder; it does not implement or replace the decoder itself.
#include <android/log.h>
#include <dlfcn.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define LOG_TAG "AC4Compat"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

typedef uint32_t OMX_U32;
typedef uint8_t OMX_U8;
typedef uint32_t OMX_INDEXTYPE;
typedef uint32_t OMX_ERRORTYPE;
typedef void *OMX_PTR;
typedef void *OMX_HANDLETYPE;

typedef union OMX_VERSIONTYPE {
    struct {
        uint8_t nVersionMajor;
        uint8_t nVersionMinor;
        uint8_t nRevision;
        uint8_t nStep;
    } s;
    uint32_t nVersion;
} OMX_VERSIONTYPE;

typedef struct OMX_CALLBACKTYPE OMX_CALLBACKTYPE;
typedef struct OMX_COMPONENTTYPE OMX_COMPONENTTYPE;

typedef OMX_ERRORTYPE (*omx_set_parameter_fn)(
        OMX_HANDLETYPE component, OMX_INDEXTYPE index, OMX_PTR params);
typedef OMX_ERRORTYPE (*omx_component_deinit_fn)(OMX_HANDLETYPE component);

struct OMX_COMPONENTTYPE {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_PTR pComponentPrivate;
    OMX_PTR pApplicationPrivate;
    OMX_PTR GetComponentVersion;
    OMX_PTR SendCommand;
    OMX_PTR GetParameter;
    omx_set_parameter_fn SetParameter;
    OMX_PTR GetConfig;
    OMX_PTR SetConfig;
    OMX_PTR GetExtensionIndex;
    OMX_PTR GetState;
    OMX_PTR ComponentTunnelRequest;
    OMX_PTR UseBuffer;
    OMX_PTR AllocateBuffer;
    OMX_PTR FreeBuffer;
    OMX_PTR EmptyThisBuffer;
    OMX_PTR FillThisBuffer;
    OMX_PTR SetCallbacks;
    omx_component_deinit_fn ComponentDeInit;
};

enum {
    OMX_ErrorNone = 0,
    OMX_ErrorUndefined = 0x80001001u,
    OMX_IndexParamAudioAndroidAc4 = 0x6f400007u,
    OMX_IndexParamAudioAndroidAc4Tbl = 0x6f400009u,
    kLutSize = 256,
    kTableDataSize = 84,
    kTableStorageSize = 84,
};

typedef struct Ac4TableParams {
    OMX_U32 nSize;
    OMX_U8 seedA;
    OMX_U8 seedB;
    OMX_U8 seedC;
    OMX_U8 idA;
    OMX_U8 idB;
    OMX_U8 idC;
    OMX_U8 maskA;
    OMX_U8 maskB;
    OMX_U8 maskC;
    OMX_U32 sizeA;
    OMX_U32 sizeB;
    OMX_U32 sizeC;
    OMX_U8 bufferA[kLutSize];
    OMX_U8 bufferB[kTableStorageSize];
    OMX_U8 bufferC[kTableStorageSize];
} Ac4TableParams;

_Static_assert(sizeof(void *) == 4, "this wrapper is for the 32-bit OMX service");
_Static_assert(offsetof(OMX_COMPONENTTYPE, SetParameter) == 28,
               "unexpected OMX_COMPONENTTYPE layout");
_Static_assert(offsetof(OMX_COMPONENTTYPE, ComponentDeInit) == 76,
               "unexpected OMX_COMPONENTTYPE layout");
_Static_assert(offsetof(Ac4TableParams, bufferA) == 0x1c,
               "unexpected AC4 table layout");
_Static_assert(offsetof(Ac4TableParams, bufferB) == 0x11c,
               "unexpected AC4 table layout");
_Static_assert(offsetof(Ac4TableParams, bufferC) == 0x170,
               "unexpected AC4 table layout");
_Static_assert(sizeof(Ac4TableParams) == 452,
               "unexpected AC4 table parameter size");

static const OMX_U8 kTableA[kLutSize] = {
    0x7f,0xf8,0xff,0xff, 0xfc,0xff,0xff,0xff, 0xf5,0xff,0xff,0xff, 0xfd,0xff,0xff,0xff,
    0x7f,0xf8,0xff,0xff, 0xfc,0xff,0xff,0xff, 0xf5,0xff,0xff,0xff, 0xfe,0xff,0xff,0xff,
    0xff,0xf7,0xff,0xff, 0xfb,0xff,0xff,0xff, 0xf7,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,
    0xff,0xf9,0xff,0xff, 0xf9,0xff,0xff,0xff, 0xfb,0xff,0xff,0xff, 0xfd,0xff,0xff,0xff,
    0xff,0xf9,0xff,0xff, 0xf9,0xff,0xff,0xff, 0xfb,0xff,0xff,0xff, 0xfe,0xff,0xff,0xff,
    0x3f,0xfc,0xff,0xff, 0xfc,0xff,0xff,0xff, 0xfa,0xff,0xff,0xff, 0xfd,0xff,0xff,0xff,
    0x3f,0xfc,0xff,0xff, 0xfc,0xff,0xff,0xff, 0xfa,0xff,0xff,0xff, 0xfe,0xff,0xff,0xff,
    0xff,0xfb,0xff,0xff, 0xfb,0xff,0xff,0xff, 0xfb,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,
    0xff,0xfc,0xff,0xff, 0xf9,0xff,0xff,0xff, 0xfd,0xff,0xff,0xff, 0xfd,0xff,0xff,0xff,
    0xff,0xfc,0xff,0xff, 0xf9,0xff,0xff,0xff, 0xfd,0xff,0xff,0xff, 0xfe,0xff,0xff,0xff,
    0xff,0xfd,0xff,0xff, 0xfb,0xff,0xff,0xff, 0xfd,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,
    0x7f,0xfe,0xff,0xff, 0xf9,0xff,0xff,0xff, 0xfe,0xff,0xff,0xff, 0xfd,0xff,0xff,0xff,
    0x7f,0xfe,0xff,0xff, 0xf9,0xff,0xff,0xff, 0xfe,0xff,0xff,0xff, 0xfe,0xff,0xff,0xff,
    0xff,0xf7,0xff,0xff, 0xfb,0xff,0xff,0xff, 0xf7,0xff,0xff,0xff, 0xf9,0xff,0xff,0xff,
    0xff,0xf7,0xff,0xff, 0xfb,0xff,0xff,0xff, 0xf7,0xff,0xff,0xff, 0xfc,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xfa,0xff,0xff,0xff,
};

static const OMX_U8 kTableB[kTableDataSize] = {
    /*  0 */ 0xf7,0xff,0xff,0xff,
    /*  1 */ 0xfa,0xff,0xff,0xff, /*  2 */ 0xfa,0xff,0xff,0xff,
    /*  3 */ 0xfa,0xff,0xff,0xff, /*  4 */ 0xfa,0xff,0xff,0xff,
    /*  5 */ 0xfa,0xff,0xff,0xff, /*  6 */ 0xfa,0xff,0xff,0xff,
    /*  7 */ 0xfa,0xff,0xff,0xff, /*  8 */ 0xfa,0xff,0xff,0xff,
    /*  9 */ 0xfa,0xff,0xff,0xff, /* 10 */ 0xfa,0xff,0xff,0xff,
    /* 11 */ 0xfa,0xff,0xff,0xff, /* 12 */ 0xfa,0xff,0xff,0xff,
    /* 13 */ 0xfa,0xff,0xff,0xff, /* 14 */ 0xfa,0xff,0xff,0xff,
    /* 15 */ 0xfa,0xff,0xff,0xff, /* 16 */ 0xfa,0xff,0xff,0xff,
    /* 17 */ 0xfa,0xff,0xff,0xff, /* 18 */ 0xf7,0xff,0xff,0xff,
    /* 19 */ 0xf7,0xff,0xff,0xff, /* 20 */ 0xfa,0xff,0xff,0xff,
};

static const OMX_U8 kTableC[kTableDataSize] = {
    /*  0 */ 0xfe,0xff,0xff,0xff, /*  1 */ 0xfe,0xff,0xff,0xff,
    /*  2 */ 0xf4,0xff,0xff,0xff, /*  3 */ 0xf4,0xff,0xff,0xff,
    /*  4 */ 0xf4,0xff,0xff,0xff, /*  5 */ 0xfa,0xff,0xff,0xff,
    /*  6 */ 0xfa,0xff,0xff,0xff, /*  7 */ 0xfe,0xff,0xff,0xff,
    /*  8 */ 0xfc,0xff,0xff,0xff, /*  9 */ 0xfe,0xff,0xff,0xff,
    /* 10 */ 0xfe,0xff,0xff,0xff, /* 11 */ 0xfd,0xff,0xff,0xff,
    /* 12 */ 0xfd,0xff,0xff,0xff, /* 13 */ 0xfe,0xff,0xff,0xff,
    /* 14 */ 0xfe,0xff,0xff,0xff, /* 15 */ 0xfe,0xff,0xff,0xff,
    /* 16 */ 0xfe,0xff,0xff,0xff, /* 17 */ 0xf4,0xff,0xff,0xff,
    /* 18 */ 0xfe,0xff,0xff,0xff, /* 19 */ 0xfe,0xff,0xff,0xff,
    /* 20 */ 0xfe,0xff,0xff,0xff,
};

static const uint32_t kScrambleWords[64] = {
    0x18aa9205,0xb9953de4,0x6fc38e9e,0x6c44fe69,
    0x2dcf9356,0x0755aed1,0xf994015f,0x28dc5d10,
    0x0efc3170,0x2486ce80,0x1a4f683a,0xda7ea9bf,
    0xba6747ab,0x65cc8474,0x1b3049af,0xd89d380f,
    0x1535e9e3,0xb4d4ace6,0xbce2eb58,0xa7627839,
    0x21cb3cf3,0x2b52bd11,0x9023a173,0xd9793f51,
    0x22d3767f,0xf487f17a,0x40b02606,0x97d25c0d,
    0x2a6e4de7,0xb699dbb3,0xb15a64a8,0x539f00e0,
    0x43ea19cd,0x6dc24617,0x0298c027,0x50c13ed6,
    0x6b2c16d5,0xdf13710b,0xbee575ad,0x964863f5,
    0xfb29917c,0xec3461b5,0xca36b74a,0x5466a2bb,
    0x088104f6,0x2f8f7bb2,0x9c0cf233,0x208b7d8c,
    0x881d4ea5,0x6077144c,0xff0aa4ef,0x25de45e8,
    0xddb85985,0x5ed009a6,0xc8c5c6ed,0x2e82f7d7,
    0x4172a3fa,0x576ac4f0,0x37e18d1e,0xfd12034b,
    0x5bf8421c,0x9b1f9a83,0x3ba0c7c9,0x32898aee,
};

typedef OMX_PTR (*create_component_fn)(
        const char *name, const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR app_data, OMX_COMPONENTTYPE **component);

static const char kCreateComponentSymbol[] =
        "_Z22createSoftOMXComponentPKcPK16OMX_CALLBACKTYPEPvPP17OMX_COMPONENTTYPE";
static const char kExpectedComponentName[] = "OMX.dolby.ac4.decoder";

static pthread_once_t gLoadOnce = PTHREAD_ONCE_INIT;
static void *gRealLibrary;
static create_component_fn gCreateComponent;

__attribute__((constructor))
static void log_wrapper_loaded(void) {
    LOGI("compatibility wrapper DSO loaded (splitsec-v1)");
}

static void load_real_component(void) {
    gRealLibrary = dlopen("/vendor/lib/libstagefright_soft_ac4src.so",
                          RTLD_NOW | RTLD_LOCAL);
    if (gRealLibrary == NULL) {
        LOGE("dlopen real decoder failed: %s", dlerror());
        return;
    }
    gCreateComponent = (create_component_fn)dlsym(
            gRealLibrary, kCreateComponentSymbol);
    if (gCreateComponent == NULL) {
        LOGE("dlsym real decoder factory failed: %s", dlerror());
    }
}

static void encode_table(OMX_U8 *output, const OMX_U8 *plain,
                         size_t size, OMX_U8 seed, OMX_U8 mask) {
    const OMX_U8 *scramble = (const OMX_U8 *)kScrambleWords;
    for (size_t i = 0; i < size; ++i) {
        output[i] = plain[i]
                ^ scramble[(i + seed) % kLutSize]
                ^ scramble[(i + seed + mask) % kLutSize];
    }
}

static void init_table_params(Ac4TableParams *params) {
    memset(params, 0, sizeof(*params));
    params->nSize = sizeof(*params);
    params->seedA = 0x31;
    params->seedB = 0x73;
    params->seedC = 0xc7;
    params->idA = 0x09;
    params->idB = 0x0a;
    params->idC = 0x0b;
    params->maskA = 0x00;
    params->maskB = 0x04;
    params->maskC = 0x08;
    params->sizeA = kLutSize;
    params->sizeB = kTableDataSize;
    params->sizeC = kTableDataSize;
    encode_table(params->bufferA, kTableA, kLutSize,
                 params->seedA, params->maskA);
    encode_table(params->bufferB, kTableB, kTableDataSize,
                 params->seedB, params->maskB);
    encode_table(params->bufferC, kTableC, kTableDataSize,
                 params->seedC, params->maskC);
}

static OMX_ERRORTYPE inject_tables(OMX_COMPONENTTYPE *component) {
    Ac4TableParams table_params;
    init_table_params(&table_params);
    LOGI("factory injection: component=%p private=%p set=%p abi=splitsec-v1",
         (void *)component, component->pComponentPrivate,
         (void *)component->SetParameter);
    OMX_ERRORTYPE table_result = component->SetParameter(
            component, OMX_IndexParamAudioAndroidAc4Tbl, &table_params);
    LOGI("AC4 table injection returned 0x%08x (sizes %u/%u/%u)",
         table_result, table_params.sizeA, table_params.sizeB,
         table_params.sizeC);
    memset(&table_params, 0, sizeof(table_params));
    return table_result;
}

static OMX_PTR reject_component(
        OMX_PTR real_object, OMX_COMPONENTTYPE **component,
        const char *reason) {
    LOGE("rejecting real decoder component: %s", reason);
    if (component != NULL && *component != NULL) {
        OMX_COMPONENTTYPE *real_component = *component;
        if (real_component->nSize >=
                    offsetof(OMX_COMPONENTTYPE, ComponentDeInit) +
                            sizeof(real_component->ComponentDeInit) &&
                real_component->ComponentDeInit != NULL) {
            OMX_ERRORTYPE deinit_result =
                    real_component->ComponentDeInit(real_component);
            if (deinit_result != OMX_ErrorNone) {
                LOGW("ComponentDeInit after rejection returned 0x%08x",
                     deinit_result);
            }
        } else {
            LOGW("cannot deinitialize rejected component safely");
        }
        *component = NULL;
    }
    (void)real_object;
    return NULL;
}

__attribute__((visibility("default")))
OMX_PTR createSoftOMXComponent(
        const char *name, const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR app_data, OMX_COMPONENTTYPE **component)
        __asm__("_Z22createSoftOMXComponentPKcPK16OMX_CALLBACKTYPEPvPP17OMX_COMPONENTTYPE");

OMX_PTR createSoftOMXComponent(
        const char *name, const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR app_data, OMX_COMPONENTTYPE **component) {
    LOGI("factory entry: name=%s callbacks=%p app_data=%p out=%p",
         name == NULL ? "(null)" : name, (void *)callbacks,
         app_data, (void *)component);
    if (name == NULL || strcmp(name, kExpectedComponentName) != 0) {
        LOGE("unexpected component name");
        return NULL;
    }
    pthread_once(&gLoadOnce, load_real_component);
    if (gCreateComponent == NULL) {
        return NULL;
    }

    OMX_PTR real_object = gCreateComponent(name, callbacks, app_data, component);
    LOGI("real factory returned object=%p component=%p",
         real_object, component == NULL ? NULL : (void *)*component);
    if (real_object == NULL) {
        return NULL;
    }
    if (component == NULL || *component == NULL) {
        return reject_component(real_object, component,
                                "factory returned no OMX component");
    }
    if ((*component)->nSize <
            offsetof(OMX_COMPONENTTYPE, SetParameter) + sizeof((*component)->SetParameter)) {
        LOGE("real decoder returned a short OMX component table: %u",
             (*component)->nSize);
        return reject_component(real_object, component,
                                "short OMX component table");
    }
    if ((*component)->SetParameter == NULL) {
        return reject_component(real_object, component,
                                "missing SetParameter callback");
    }
    OMX_ERRORTYPE injection_result = inject_tables(*component);
    if (injection_result != OMX_ErrorNone) {
        LOGE("private AC4 table injection rejected: 0x%08x", injection_result);
        return reject_component(real_object, component,
                                "private table injection failed");
    }
    return real_object;
}
