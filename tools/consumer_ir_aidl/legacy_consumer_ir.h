/*
 * Copyright (C) 2008-2013 The Android Open Source Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

struct hw_module_t;
struct hw_device_t;

typedef struct hw_module_methods_t {
    int (*open)(const struct hw_module_t* module, const char* id,
                struct hw_device_t** device);
} hw_module_methods_t;

typedef struct hw_module_t {
    uint32_t tag;
    uint16_t module_api_version;
    uint16_t hal_api_version;
    const char* id;
    const char* name;
    const char* author;
    struct hw_module_methods_t* methods;
    void* dso;
#ifdef __LP64__
    uint64_t reserved[32 - 7];
#else
    uint32_t reserved[32 - 7];
#endif
} hw_module_t;

typedef struct hw_device_t {
    uint32_t tag;
    uint32_t version;
    struct hw_module_t* module;
#ifdef __LP64__
    uint64_t reserved[12];
#else
    uint32_t reserved[12];
#endif
    int (*close)(struct hw_device_t* device);
} hw_device_t;

typedef struct consumerir_freq_range {
    int min;
    int max;
} consumerir_freq_range_t;

typedef struct consumerir_device {
    struct hw_device_t common;
    int (*transmit)(struct consumerir_device* dev, int carrier_freq,
                    const int pattern[], int pattern_len);
    int (*get_num_carrier_freqs)(struct consumerir_device* dev);
    int (*get_carrier_freqs)(struct consumerir_device* dev, size_t len,
                             consumerir_freq_range_t* ranges);
    void* reserved[8 - 3];
} consumerir_device_t;

#define CONSUMERIR_HARDWARE_MODULE_ID "consumerir"
#define CONSUMERIR_TRANSMITTER "transmitter"
