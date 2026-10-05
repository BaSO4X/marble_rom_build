#define LOG_TAG "vibrator-default-alias"

#include <android/binder_ibinder.h>
#include <android/binder_parcel.h>
#include <android/binder_status.h>
#include <android/log.h>
#include <dlfcn.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SOURCE_SERVICE "android.hardware.vibrator.IVibrator/vibratorfeature"
#define ALIAS_SERVICE "android.hardware.vibrator.IVibrator/default"
#define INTERFACE_DESCRIPTOR "android.hardware.vibrator.IVibrator"

/* Frozen IVibrator v1 method order. */
#define TRANSACTION_PERFORM (FIRST_CALL_TRANSACTION + 3)
#define TRANSACTION_GET_SUPPORTED_EFFECTS (FIRST_CALL_TRANSACTION + 4)

/* HyperOS custom effects observed on current releases stay below this range. */
#define MAX_COMPAT_EFFECT_ID 1023

#define EFFECT_CLICK 0
#define EFFECT_TICK 2
#define EFFECT_HEAVY_CLICK 5
#define EFFECT_TASK_CLEAN_ALL_V1 90
#define EFFECT_TASK_CLEAN_ALL_V2 213

#define STRENGTH_LIGHT 0
#define STRENGTH_MEDIUM 1
#define STRENGTH_STRONG 2

struct EffectList {
    int32_t* values;
    int32_t count;
};

struct ProxyContext {
    AIBinder* source;
    struct EffectList source_effects;
    struct EffectList advertised_effects;
    pthread_mutex_t log_mutex;
    bool remap_logged[MAX_COMPAT_EFFECT_ID + 1];
};

struct Int32ArrayRead {
    int32_t* values;
    int32_t count;
};

struct PlatformBinderApi {
    void* library;
    AIBinder* (*wait_for_service)(const char* instance);
    binder_exception_t (*add_service)(AIBinder* binder,
                                      const char* instance);
    void (*set_thread_pool_max_thread_count)(uint32_t num_threads);
    void (*start_thread_pool)(void);
    void (*join_thread_pool)(void);
    void (*mark_vintf_stability)(AIBinder* binder);
};

static AIBinder_Class* g_proxy_class;
static struct PlatformBinderApi g_platform;

static bool load_symbol(void* library, const char* name,
                        void* destination, size_t destination_size) {
    void* symbol = dlsym(library, name);
    if (symbol == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Unable to resolve %s: %s", name, dlerror());
        return false;
    }
    if (destination_size != sizeof(symbol)) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Unexpected function pointer size for %s", name);
        return false;
    }
    __builtin_memcpy(destination, &symbol, sizeof(symbol));
    return true;
}

#define LOAD_PLATFORM_SYMBOL(member, symbol_name) \
    load_symbol(g_platform.library, symbol_name, \
                &g_platform.member, sizeof(g_platform.member))

static bool load_platform_binder_api(void) {
    g_platform.library = dlopen("libbinder_ndk.so", RTLD_NOW | RTLD_LOCAL);
    if (g_platform.library == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Unable to open libbinder_ndk.so: %s", dlerror());
        return false;
    }
    return LOAD_PLATFORM_SYMBOL(wait_for_service,
                                "AServiceManager_waitForService") &&
           LOAD_PLATFORM_SYMBOL(add_service,
                                "AServiceManager_addService") &&
           LOAD_PLATFORM_SYMBOL(set_thread_pool_max_thread_count,
                                "ABinderProcess_setThreadPoolMaxThreadCount") &&
           LOAD_PLATFORM_SYMBOL(start_thread_pool,
                                "ABinderProcess_startThreadPool") &&
           LOAD_PLATFORM_SYMBOL(join_thread_pool,
                                "ABinderProcess_joinThreadPool") &&
           LOAD_PLATFORM_SYMBOL(mark_vintf_stability,
                                "AIBinder_markVintfStability");
}

static void log_status(const char* operation, binder_status_t status) {
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s failed: %d",
                        operation, status);
}

static bool allocate_int32_array(void* array_data, int32_t length,
                                 int32_t** out_buffer) {
    struct Int32ArrayRead* array = array_data;
    if (length < 0) {
        return false;
    }
    array->count = length;
    if (length == 0) {
        array->values = NULL;
        *out_buffer = NULL;
        return true;
    }
    if ((size_t)length > SIZE_MAX / sizeof(*array->values)) {
        return false;
    }
    array->values = calloc((size_t)length, sizeof(*array->values));
    if (array->values == NULL) {
        return false;
    }
    *out_buffer = array->values;
    return true;
}

static bool effect_list_contains(const struct EffectList* list, int32_t effect) {
    for (int32_t index = 0; index < list->count; ++index) {
        if (list->values[index] == effect) {
            return true;
        }
    }
    return false;
}

static bool read_source_effects(AIBinder* source, struct EffectList* effects) {
    AParcel* input = NULL;
    AParcel* output = NULL;
    AStatus* service_status = NULL;
    struct Int32ArrayRead result = {0};

    binder_status_t status = AIBinder_prepareTransaction(source, &input);
    if (status != STATUS_OK) {
        log_status("Preparing getSupportedEffects", status);
        return false;
    }
    status = AIBinder_transact(source, TRANSACTION_GET_SUPPORTED_EFFECTS,
                               &input, &output, 0);
    if (status != STATUS_OK) {
        log_status("Calling getSupportedEffects", status);
        if (output != NULL) {
            AParcel_delete(output);
        }
        return false;
    }
    status = AParcel_readStatusHeader(output, &service_status);
    if (status != STATUS_OK) {
        log_status("Reading getSupportedEffects status", status);
        AParcel_delete(output);
        return false;
    }
    if (!AStatus_isOk(service_status)) {
        const char* description = AStatus_getDescription(service_status);
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "getSupportedEffects returned %s", description);
        AStatus_deleteDescription(description);
        AStatus_delete(service_status);
        AParcel_delete(output);
        return false;
    }
    AStatus_delete(service_status);

    status = AParcel_readInt32Array(output, &result, allocate_int32_array);
    AParcel_delete(output);
    if (status != STATUS_OK || result.count <= 0) {
        log_status("Reading getSupportedEffects result", status);
        free(result.values);
        return false;
    }

    effects->values = result.values;
    effects->count = result.count;
    return true;
}

static bool build_advertised_effects(struct ProxyContext* context) {
    const int32_t base_count = MAX_COMPAT_EFFECT_ID + 1;
    if (context->source_effects.count > INT32_MAX - base_count) {
        return false;
    }
    const int32_t capacity = base_count + context->source_effects.count;
    context->advertised_effects.values =
            calloc((size_t)capacity, sizeof(*context->advertised_effects.values));
    if (context->advertised_effects.values == NULL) {
        return false;
    }

    for (int32_t effect = 0; effect <= MAX_COMPAT_EFFECT_ID; ++effect) {
        context->advertised_effects.values[context->advertised_effects.count++] =
                effect;
    }
    for (int32_t index = 0; index < context->source_effects.count; ++index) {
        const int32_t effect = context->source_effects.values[index];
        if (effect < 0 || effect <= MAX_COMPAT_EFFECT_ID ||
            effect_list_contains(&context->advertised_effects, effect)) {
            continue;
        }
        context->advertised_effects.values[context->advertised_effects.count++] =
                effect;
    }
    return true;
}

static int32_t generic_fallback(const struct ProxyContext* context,
                                int8_t strength) {
    int32_t candidates[3];
    if (strength == STRENGTH_LIGHT) {
        candidates[0] = EFFECT_TICK;
        candidates[1] = EFFECT_CLICK;
        candidates[2] = EFFECT_HEAVY_CLICK;
    } else if (strength == STRENGTH_STRONG) {
        candidates[0] = EFFECT_HEAVY_CLICK;
        candidates[1] = EFFECT_CLICK;
        candidates[2] = EFFECT_TICK;
    } else {
        candidates[0] = EFFECT_CLICK;
        candidates[1] = EFFECT_HEAVY_CLICK;
        candidates[2] = EFFECT_TICK;
    }

    for (size_t index = 0; index < sizeof(candidates) / sizeof(candidates[0]);
         ++index) {
        if (effect_list_contains(&context->source_effects, candidates[index])) {
            return candidates[index];
        }
    }
    return -1;
}

static int32_t map_effect(const struct ProxyContext* context, int32_t effect,
                          int8_t strength) {
    if (effect_list_contains(&context->source_effects, effect)) {
        return effect;
    }
    if (effect == EFFECT_TASK_CLEAN_ALL_V2 &&
        effect_list_contains(&context->source_effects,
                             EFFECT_TASK_CLEAN_ALL_V1)) {
        return EFFECT_TASK_CLEAN_ALL_V1;
    }
    return generic_fallback(context, strength);
}

static void log_remap_once(struct ProxyContext* context, int32_t source_effect,
                           int32_t target_effect, int8_t strength) {
    bool should_log = true;
    if (source_effect >= 0 && source_effect <= MAX_COMPAT_EFFECT_ID) {
        pthread_mutex_lock(&context->log_mutex);
        should_log = !context->remap_logged[source_effect];
        context->remap_logged[source_effect] = true;
        pthread_mutex_unlock(&context->log_mutex);
    }
    if (should_log) {
        __android_log_print(ANDROID_LOG_WARN, LOG_TAG,
                            "Remapping unsupported effect %d to %d (strength %d)",
                            source_effect, target_effect, strength);
    }
}

static binder_status_t append_output(AParcel* destination,
                                     const AParcel* source) {
    if (source == NULL) {
        return STATUS_UNEXPECTED_NULL;
    }
    const int32_t size = AParcel_getDataSize(source);
    if (size < 0) {
        return STATUS_BAD_VALUE;
    }
    return AParcel_appendFrom(source, destination, 0, size);
}

static binder_status_t forward_transaction(struct ProxyContext* context,
                                           transaction_code_t code,
                                           const AParcel* input,
                                           AParcel* output) {
    AParcel* forwarded_input = NULL;
    AParcel* forwarded_output = NULL;
    binder_status_t status =
            AIBinder_prepareTransaction(context->source, &forwarded_input);
    if (status != STATUS_OK) {
        return status;
    }

    const int32_t position = AParcel_getDataPosition(input);
    const int32_t size = AParcel_getDataSize(input);
    if (position < 0 || size < position) {
        AParcel_delete(forwarded_input);
        return STATUS_BAD_VALUE;
    }
    status = AParcel_appendFrom(input, forwarded_input, position,
                                size - position);
    if (status != STATUS_OK) {
        AParcel_delete(forwarded_input);
        return status;
    }

    status = AIBinder_transact(context->source, code, &forwarded_input,
                               &forwarded_output, 0);
    if (status == STATUS_OK) {
        status = append_output(output, forwarded_output);
    }
    if (forwarded_output != NULL) {
        AParcel_delete(forwarded_output);
    }
    return status;
}

static binder_status_t perform_with_fallback(struct ProxyContext* context,
                                             const AParcel* input,
                                             AParcel* output) {
    int32_t effect;
    int8_t strength;
    binder_status_t status = AParcel_readInt32(input, &effect);
    if (status != STATUS_OK) {
        return status;
    }
    const int32_t remainder_position = AParcel_getDataPosition(input);
    status = AParcel_readByte(input, &strength);
    if (status != STATUS_OK) {
        return status;
    }
    status = AParcel_setDataPosition(input, remainder_position);
    if (status != STATUS_OK) {
        return status;
    }

    const int32_t mapped_effect = map_effect(context, effect, strength);
    if (mapped_effect < 0) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "No supported fallback for effect %d", effect);
        return STATUS_BAD_VALUE;
    }
    if (mapped_effect != effect) {
        log_remap_once(context, effect, mapped_effect, strength);
    }

    AParcel* forwarded_input = NULL;
    AParcel* forwarded_output = NULL;
    status = AIBinder_prepareTransaction(context->source, &forwarded_input);
    if (status != STATUS_OK) {
        return status;
    }
    status = AParcel_writeInt32(forwarded_input, mapped_effect);
    if (status != STATUS_OK) {
        AParcel_delete(forwarded_input);
        return status;
    }

    const int32_t size = AParcel_getDataSize(input);
    if (size < remainder_position) {
        AParcel_delete(forwarded_input);
        return STATUS_BAD_VALUE;
    }
    status = AParcel_appendFrom(input, forwarded_input, remainder_position,
                                size - remainder_position);
    if (status != STATUS_OK) {
        AParcel_delete(forwarded_input);
        return status;
    }

    status = AIBinder_transact(context->source, TRANSACTION_PERFORM,
                               &forwarded_input, &forwarded_output, 0);
    if (status == STATUS_OK) {
        status = append_output(output, forwarded_output);
    }
    if (forwarded_output != NULL) {
        AParcel_delete(forwarded_output);
    }
    return status;
}

static binder_status_t write_advertised_effects(
        const struct ProxyContext* context, AParcel* output) {
    AStatus* service_status = AStatus_newOk();
    if (service_status == NULL) {
        return STATUS_NO_MEMORY;
    }
    binder_status_t status =
            AParcel_writeStatusHeader(output, service_status);
    AStatus_delete(service_status);
    if (status != STATUS_OK) {
        return status;
    }
    return AParcel_writeInt32Array(output,
                                   context->advertised_effects.values,
                                   context->advertised_effects.count);
}

static void* on_proxy_create(void* args) {
    return args;
}

static void on_proxy_destroy(void* user_data) {
    struct ProxyContext* context = user_data;
    if (context == NULL) {
        return;
    }
    if (context->source != NULL) {
        AIBinder_decStrong(context->source);
    }
    free(context->source_effects.values);
    free(context->advertised_effects.values);
    pthread_mutex_destroy(&context->log_mutex);
    free(context);
}

static binder_status_t on_proxy_transact(AIBinder* binder,
                                         transaction_code_t code,
                                         const AParcel* input,
                                         AParcel* output) {
    struct ProxyContext* context = AIBinder_getUserData(binder);
    if (context == NULL) {
        return STATUS_UNEXPECTED_NULL;
    }
    if (code == TRANSACTION_PERFORM) {
        return perform_with_fallback(context, input, output);
    }
    if (code == TRANSACTION_GET_SUPPORTED_EFFECTS) {
        return write_advertised_effects(context, output);
    }
    return forward_transaction(context, code, input, output);
}

static bool mark_vintf_stability(AIBinder* binder) {
    g_platform.mark_vintf_stability(binder);
    return true;
}

static bool copy_binder_extension(AIBinder* source, AIBinder* proxy) {
    AIBinder* extension = NULL;
    binder_status_t status = AIBinder_getExtension(source, &extension);
    if (status != STATUS_OK) {
        log_status("Reading source Binder extension", status);
        return false;
    }
    if (extension == NULL) {
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG,
                            "Source Binder has no extension");
        return true;
    }
    status = AIBinder_setExtension(proxy, extension);
    AIBinder_decStrong(extension);
    if (status != STATUS_OK) {
        log_status("Copying source Binder extension", status);
        return false;
    }
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG,
                        "Copied source Binder extension");
    return true;
}

static void on_source_died(void* cookie) {
    (void)cookie;
    __android_log_print(ANDROID_LOG_FATAL, LOG_TAG,
                        "Source Binder died; exiting for init restart");
    _exit(EXIT_FAILURE);
}

int main(void) {
    if (!load_platform_binder_api()) {
        return EXIT_FAILURE;
    }
    g_platform.set_thread_pool_max_thread_count(4);
    g_platform.start_thread_pool();

    g_proxy_class = AIBinder_Class_define(INTERFACE_DESCRIPTOR,
                                          on_proxy_create,
                                          on_proxy_destroy,
                                          on_proxy_transact);
    if (g_proxy_class == NULL) {
        __android_log_print(ANDROID_LOG_FATAL, LOG_TAG,
                            "Unable to define IVibrator proxy class");
        return EXIT_FAILURE;
    }

    __android_log_print(ANDROID_LOG_INFO, LOG_TAG,
                        "Waiting for %s", SOURCE_SERVICE);
    AIBinder* source = g_platform.wait_for_service(SOURCE_SERVICE);
    if (source == NULL) {
        __android_log_print(ANDROID_LOG_FATAL, LOG_TAG,
                            "Source service wait returned null");
        return EXIT_FAILURE;
    }
    if (!AIBinder_associateClass(source, g_proxy_class)) {
        __android_log_print(ANDROID_LOG_FATAL, LOG_TAG,
                            "Source Binder descriptor does not match IVibrator");
        AIBinder_decStrong(source);
        return EXIT_FAILURE;
    }

    struct ProxyContext* context = calloc(1, sizeof(*context));
    if (context == NULL) {
        AIBinder_decStrong(source);
        return EXIT_FAILURE;
    }
    context->source = source;
    if (pthread_mutex_init(&context->log_mutex, NULL) != 0) {
        __android_log_print(ANDROID_LOG_FATAL, LOG_TAG,
                            "Unable to initialize compatibility log lock");
        AIBinder_decStrong(source);
        free(context);
        return EXIT_FAILURE;
    }
    if (!read_source_effects(source, &context->source_effects) ||
        !build_advertised_effects(context)) {
        __android_log_print(ANDROID_LOG_FATAL, LOG_TAG,
                            "Unable to initialize effect compatibility data");
        on_proxy_destroy(context);
        return EXIT_FAILURE;
    }
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG,
                        "Loaded %d source effects; advertising %d effects",
                        context->source_effects.count,
                        context->advertised_effects.count);

    if (generic_fallback(context, STRENGTH_MEDIUM) < 0) {
        __android_log_print(ANDROID_LOG_FATAL, LOG_TAG,
                            "Source HAL provides no safe generic fallback");
        on_proxy_destroy(context);
        return EXIT_FAILURE;
    }

    AIBinder* proxy = AIBinder_new(g_proxy_class, context);
    if (proxy == NULL) {
        __android_log_print(ANDROID_LOG_FATAL, LOG_TAG,
                            "Unable to create IVibrator proxy Binder");
        on_proxy_destroy(context);
        return EXIT_FAILURE;
    }
    if (!mark_vintf_stability(proxy) ||
        !copy_binder_extension(source, proxy)) {
        AIBinder_decStrong(proxy);
        return EXIT_FAILURE;
    }

    AIBinder_DeathRecipient* death_recipient =
            AIBinder_DeathRecipient_new(on_source_died);
    if (death_recipient == NULL ||
        AIBinder_linkToDeath(source, death_recipient, NULL) != STATUS_OK) {
        __android_log_print(ANDROID_LOG_FATAL, LOG_TAG,
                            "Unable to monitor source Binder");
        AIBinder_decStrong(proxy);
        return EXIT_FAILURE;
    }

    binder_exception_t add_status =
            g_platform.add_service(proxy, ALIAS_SERVICE);
    if (add_status != EX_NONE) {
        __android_log_print(ANDROID_LOG_FATAL, LOG_TAG,
                            "Unable to register %s: %d", ALIAS_SERVICE,
                            add_status);
        AIBinder_decStrong(proxy);
        return EXIT_FAILURE;
    }

    __android_log_print(ANDROID_LOG_INFO, LOG_TAG,
                        "Registered %s with %d source effects and %d advertised effects",
                        ALIAS_SERVICE, context->source_effects.count,
                        context->advertised_effects.count);
    g_platform.join_thread_pool();
    AIBinder_decStrong(proxy);
    return EXIT_FAILURE;
}
