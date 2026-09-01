/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Connects the audited Xiaomi LHDC v3/v4 encoder interface to the matching
 * legacy Savitech encoder DSO. Unknown Bluetooth builds remain untouched.
 */

#define LOG_TAG "LhdcLegacyPatch"

#include <android/log.h>
#include <dlfcn.h>
#include <elf.h>
#include <link.h>
#include <sys/mman.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

namespace {

constexpr char kEncoderLibrary[] = "/system/lib64/liblhdcBT_enc.so";
constexpr size_t kBtBufferSize = 4096 + 16;
constexpr uint16_t kAvdtMediaOffset = 23;
constexpr uint16_t kLhdcHeaderLength = 2;
constexpr uint16_t kLhdcBufferOffset = kAvdtMediaOffset + kLhdcHeaderLength;
constexpr size_t kXiaomiOtaCodecInfoSize = 45;
constexpr size_t kCodecConfigStateOffset = 0x60;
constexpr uint64_t kVendorCommandMask = 0xc000;
constexpr uint64_t kQualityMagic = 0x8000;
constexpr uint64_t kQualityMask = 0xff;
constexpr int kQualityAuto = 8;
constexpr int kQualityResetAuto = 9;
constexpr int kQualityHigh = 7;
constexpr int kQualityMid = 6;
constexpr int kQualityLow = 5;
constexpr int kExtFunctionAr = 0;
constexpr int kExtFunctionLarc = 1;
constexpr int kExtFunctionJas = 3;

constexpr uint8_t kExpectedBuildId[] = {
    0x9f, 0x45, 0x37, 0xb4, 0x92, 0xe0, 0xbf, 0x72,
    0xcf, 0xa5, 0xb0, 0xf1, 0xd3, 0x7e, 0x24, 0x7d,
};
constexpr uintptr_t kCopyOutOtaCodecConfigOffset = 0x8d457c;
constexpr uintptr_t kExpectedEncoderTableOffset = 0xef12f8;

struct BtHdr {
  uint16_t event;
  uint16_t len;
  uint16_t offset;
  uint16_t layer_specific;
  uint8_t data[];
};

struct EncoderPeerParams {
  bool is_peer_edr;
  bool peer_supports_3mbps;
  uint16_t peer_mtu;
};
static_assert(sizeof(EncoderPeerParams) == 4);

using ReadCallback = uint32_t (*)(uint8_t*, uint32_t);
using EnqueueCallback = bool (*)(BtHdr*, size_t, uint32_t);
using EncoderInit = void (*)(const EncoderPeerParams*, void*, ReadCallback, EnqueueCallback);

struct EncoderInterface {
  EncoderInit encoder_init;
  void (*encoder_cleanup)();
  void (*feeding_reset)();
  void (*feeding_flush)();
  uint64_t (*get_encoder_interval_ms)();
  int (*get_effective_frame_size)();
  void (*send_frames)(uint64_t);
  void (*set_transmit_queue_length)(size_t);
};
static_assert(sizeof(EncoderInterface) == 8 * sizeof(void*));

struct BtavCodecConfig {
  int32_t codec_type;
  int32_t codec_priority;
  uint32_t sample_rate;
  uint32_t bits_per_sample;
  uint32_t channel_mode;
  uint32_t reserved;
  uint64_t codec_specific_1;
  uint64_t codec_specific_2;
  uint64_t codec_specific_3;
  uint64_t codec_specific_4;
};
static_assert(sizeof(BtavCodecConfig) == 56);

struct LegacyApi {
  void* library;
  void* (*get_handle)(int);
  void (*free_handle)(void*);
  int (*get_bitrate)(void*);
  int (*set_bitrate)(void*, int);
  int (*init_encoder)(void*, int, int, int, int, int, int, int);
  int (*encode)(void*, void*, unsigned char*, uint32_t*, uint32_t*);
  int (*adjust_bitrate)(void*, size_t);
  void (*set_max_bitrate)(void*, int);
  int (*get_block_size)(void*);
  int (*set_ext_function)(void*, int, bool, void*, int);
  int (*set_min_bitrate_limit)(void*, bool);
};

struct CodecParameters {
  uint32_t sample_rate;
  uint32_t bits_per_sample;
  uint32_t channels;
  uint32_t library_version;
  int quality_mode;
  int max_bitrate;
  int channel_split_mode;
  uint32_t interval_ms;
  bool has_ar;
  bool has_jas;
  bool has_larc;
  bool has_min_bitrate;
};

struct EncoderState {
  ReadCallback read_callback;
  EnqueueCallback enqueue_callback;
  void* handle;
  CodecParameters codec;
  uint32_t tx_mtu;
  uint32_t bytes_per_tick;
  uint32_t feeding_counter;
  uint64_t last_frame_us;
  uint32_t timestamp;
  uint32_t sequence;
  size_t transmit_queue_length;
  bool ready;
};

struct LoadedLibrary {
  uintptr_t base;
  const ElfW(Phdr)* phdrs;
  size_t phnum;
  bool build_id_matches;
};

LegacyApi g_api{};
EncoderState g_encoder{};
LoadedLibrary g_bluetooth{};

uintptr_t align4(uintptr_t value) { return (value + 3U) & ~uintptr_t{3U}; }

bool note_has_expected_build_id(uintptr_t segment, size_t size) {
  uintptr_t cursor = segment;
  const uintptr_t end = segment + size;
  while (cursor + sizeof(ElfW(Nhdr)) <= end) {
    const auto* note = reinterpret_cast<const ElfW(Nhdr)*>(cursor);
    cursor += sizeof(*note);
    const uintptr_t name = cursor;
    cursor = align4(cursor + note->n_namesz);
    const uintptr_t desc = cursor;
    cursor = align4(cursor + note->n_descsz);
    if (cursor > end) return false;
    if (note->n_type == NT_GNU_BUILD_ID && note->n_namesz >= 3 &&
        std::memcmp(reinterpret_cast<const void*>(name), "GNU", 3) == 0 &&
        note->n_descsz == sizeof(kExpectedBuildId) &&
        std::memcmp(reinterpret_cast<const void*>(desc), kExpectedBuildId,
                    sizeof(kExpectedBuildId)) == 0) {
      return true;
    }
  }
  return false;
}

int find_bluetooth_library(dl_phdr_info* info, size_t, void* data) {
  if (info->dlpi_name == nullptr ||
      std::strstr(info->dlpi_name, "/libbluetooth_jni.so") == nullptr) {
    return 0;
  }
  auto* loaded = static_cast<LoadedLibrary*>(data);
  loaded->base = static_cast<uintptr_t>(info->dlpi_addr);
  loaded->phdrs = info->dlpi_phdr;
  loaded->phnum = info->dlpi_phnum;
  for (size_t i = 0; i < loaded->phnum; ++i) {
    const ElfW(Phdr)& phdr = loaded->phdrs[i];
    if (phdr.p_type == PT_NOTE &&
        note_has_expected_build_id(loaded->base + phdr.p_vaddr, phdr.p_memsz)) {
      loaded->build_id_matches = true;
      break;
    }
  }
  return 1;
}

bool is_executable_address(uintptr_t address) {
  for (size_t i = 0; i < g_bluetooth.phnum; ++i) {
    const ElfW(Phdr)& phdr = g_bluetooth.phdrs[i];
    if (phdr.p_type != PT_LOAD || (phdr.p_flags & PF_X) == 0) continue;
    const uintptr_t begin = g_bluetooth.base + phdr.p_vaddr;
    if (address >= begin && address < begin + phdr.p_memsz) return true;
  }
  return false;
}

bool is_read_only_data_range(uintptr_t address, size_t size) {
  if (size == 0 || address > UINTPTR_MAX - size) return false;
  const uintptr_t range_end = address + size;
  for (size_t i = 0; i < g_bluetooth.phnum; ++i) {
    const ElfW(Phdr)& phdr = g_bluetooth.phdrs[i];
    const bool is_read_only_load =
        phdr.p_type == PT_LOAD && (phdr.p_flags & PF_R) != 0 &&
        (phdr.p_flags & (PF_W | PF_X)) == 0;
    const bool is_relro = phdr.p_type == PT_GNU_RELRO;
    if (!is_read_only_load && !is_relro) {
      continue;
    }
    const uintptr_t begin = g_bluetooth.base + phdr.p_vaddr;
    const uintptr_t end = begin + phdr.p_memsz;
    if (address >= begin && range_end <= end) return true;
  }
  return false;
}

bool bytes_equal(uintptr_t address, const uint8_t* expected, size_t size) {
  return is_executable_address(address) &&
         std::memcmp(reinterpret_cast<const void*>(address), expected, size) == 0;
}

bool is_empty_encoder_table(const EncoderInterface* table) {
  constexpr uint8_t kReturnOnly[] = {0x5f, 0x24, 0x03, 0xd5, 0xc0, 0x03, 0x5f, 0xd6};
  constexpr uint8_t kInterval20[] = {0x5f, 0x24, 0x03, 0xd5, 0x80, 0x02,
                                     0x80, 0x52, 0xc0, 0x03, 0x5f, 0xd6};
  constexpr uint8_t kFrameSizeZero[] = {0x5f, 0x24, 0x03, 0xd5, 0xe0, 0x03,
                                        0x1f, 0x2a, 0xc0, 0x03, 0x5f, 0xd6};
  return bytes_equal(reinterpret_cast<uintptr_t>(table->encoder_cleanup),
                     kReturnOnly, sizeof(kReturnOnly)) &&
         bytes_equal(reinterpret_cast<uintptr_t>(table->feeding_reset),
                     kReturnOnly, sizeof(kReturnOnly)) &&
         bytes_equal(reinterpret_cast<uintptr_t>(table->feeding_flush),
                     kReturnOnly, sizeof(kReturnOnly)) &&
         bytes_equal(reinterpret_cast<uintptr_t>(table->get_encoder_interval_ms),
                     kInterval20, sizeof(kInterval20)) &&
         bytes_equal(reinterpret_cast<uintptr_t>(table->get_effective_frame_size),
                     kFrameSizeZero, sizeof(kFrameSizeZero)) &&
         bytes_equal(reinterpret_cast<uintptr_t>(table->send_frames),
                     kReturnOnly, sizeof(kReturnOnly)) &&
         is_executable_address(reinterpret_cast<uintptr_t>(table->encoder_init)) &&
         is_executable_address(reinterpret_cast<uintptr_t>(table->set_transmit_queue_length));
}

template <typename Function>
bool load_symbol(Function* target, const char* name) {
  dlerror();
  *target = reinterpret_cast<Function>(dlsym(g_api.library, name));
  const char* error = dlerror();
  if (*target != nullptr && error == nullptr) return true;
  LOGE("missing legacy encoder symbol %s: %s", name,
       error == nullptr ? "null" : error);
  return false;
}

bool load_legacy_api() {
  g_api.library = dlopen(kEncoderLibrary, RTLD_NOW | RTLD_LOCAL);
  if (g_api.library == nullptr) {
    LOGE("cannot load %s: %s", kEncoderLibrary, dlerror());
    return false;
  }
  const bool loaded =
      load_symbol(&g_api.get_handle, "lhdcBT_get_handle") &&
      load_symbol(&g_api.free_handle, "lhdcBT_free_handle") &&
      load_symbol(&g_api.get_bitrate, "lhdcBT_get_bitrate") &&
      load_symbol(&g_api.set_bitrate, "lhdcBT_set_bitrate") &&
      load_symbol(&g_api.init_encoder, "lhdcBT_init_encoder") &&
      load_symbol(&g_api.encode, "lhdcBT_encodeV3") &&
      load_symbol(&g_api.adjust_bitrate, "lhdcBT_adjust_bitrate") &&
      load_symbol(&g_api.set_max_bitrate, "lhdcBT_set_max_bitrate") &&
      load_symbol(&g_api.get_block_size, "lhdcBT_get_block_Size") &&
      load_symbol(&g_api.set_ext_function, "lhdcBT_set_ext_func_state") &&
      load_symbol(&g_api.set_min_bitrate_limit, "lhdcBT_set_hasMinBitrateLimit");
  if (loaded) return true;
  dlclose(g_api.library);
  std::memset(&g_api, 0, sizeof(g_api));
  return false;
}

bool parse_codec_info(const uint8_t* info, const void* codec_config,
                      CodecParameters* codec) {
  if (info[0] != 11 || (info[1] >> 4) != 0 || info[2] != 0xff ||
      info[3] != 0x3a || info[4] != 0x05 || info[5] != 0x00 ||
      info[6] != 0x00 || info[7] != 0x33 || info[8] != 0x4c) {
    return false;
  }
  switch (info[9] & 0x0f) {
    case 0x08: codec->sample_rate = 44100; break;
    case 0x04: codec->sample_rate = 48000; break;
    case 0x02: codec->sample_rate = 88200; break;
    case 0x01: codec->sample_rate = 96000; break;
    default: return false;
  }
  switch (info[9] & 0x30) {
    case 0x20: codec->bits_per_sample = 16; break;
    case 0x10: codec->bits_per_sample = 24; break;
    default: return false;
  }
  if ((info[10] & 0x0f) != 0x01 || (info[10] & 0x80) != 0) return false;
  codec->library_version = (info[11] & 0x80) != 0 ? 3 : 2;
  switch (info[10] & 0x30) {
    case 0x00: codec->max_bitrate = kQualityHigh; break;
    case 0x10: codec->max_bitrate = kQualityMid; break;
    case 0x20: codec->max_bitrate = kQualityLow; break;
    default: return false;
  }
  codec->channel_split_mode = info[11] & 0x0f;
  codec->has_ar = (info[9] & 0x80) != 0;
  codec->has_jas = (info[9] & 0x40) != 0;
  codec->has_min_bitrate = (info[11] & 0x20) != 0;
  codec->has_larc = (info[11] & 0x40) != 0;
  codec->channels = 2;

  BtavCodecConfig config{};
  std::memcpy(&config,
              static_cast<const uint8_t*>(codec_config) + kCodecConfigStateOffset,
              sizeof(config));
  codec->quality_mode = kQualityAuto;
  if ((config.codec_specific_1 & kVendorCommandMask) == kQualityMagic) {
    const uint32_t quality =
        static_cast<uint32_t>(config.codec_specific_1 & kQualityMask);
    if (quality <= static_cast<uint32_t>(kQualityAuto)) {
      codec->quality_mode = static_cast<int>(quality);
    } else if (quality == static_cast<uint32_t>(kQualityResetAuto)) {
      codec->quality_mode = kQualityAuto;
    } else {
      LOGW("unsupported legacy LHDC quality index %u; using ABR", quality);
    }
  }
  codec->interval_ms = (config.codec_specific_2 & 1U) != 0 ? 10 : 20;
  return true;
}

void encoder_cleanup() {
  if (g_encoder.handle != nullptr && g_api.free_handle != nullptr) {
    g_api.free_handle(g_encoder.handle);
  }
  std::memset(&g_encoder, 0, sizeof(g_encoder));
}

void feeding_reset() {
  g_encoder.feeding_counter = 0;
  g_encoder.last_frame_us = 0;
  g_encoder.timestamp = 0;
  g_encoder.sequence = 0;
  if (g_encoder.codec.interval_ms != 0) {
    g_encoder.bytes_per_tick =
        g_encoder.codec.sample_rate * g_encoder.codec.bits_per_sample / 8 *
        g_encoder.codec.channels * g_encoder.codec.interval_ms / 1000;
  }
  if (g_encoder.ready && g_encoder.codec.quality_mode == kQualityAuto) {
    g_api.set_bitrate(g_encoder.handle, kQualityResetAuto);
  }
}

void feeding_flush() { g_encoder.feeding_counter = 0; }

uint64_t get_encoder_interval_ms() {
  return g_encoder.codec.interval_ms != 0 ? g_encoder.codec.interval_ms : 20;
}

int get_effective_frame_size() { return static_cast<int>(g_encoder.tx_mtu); }

void encoder_init(const EncoderPeerParams* peer, void* codec_config,
                  ReadCallback read_callback, EnqueueCallback enqueue_callback) {
  encoder_cleanup();
  if (peer == nullptr || codec_config == nullptr || read_callback == nullptr ||
      enqueue_callback == nullptr || peer->peer_mtu <= kLhdcHeaderLength) {
    LOGE("legacy encoder init rejected invalid input");
    return;
  }

  uint8_t ota[kXiaomiOtaCodecInfoSize]{};
  using CopyOutOtaCodecConfig = bool (*)(void*, uint8_t*);
  const uintptr_t copy_out_address =
      g_bluetooth.base + kCopyOutOtaCodecConfigOffset;
  if (!is_executable_address(copy_out_address)) {
    LOGE("LHDC v3/v4 OTA copy function is outside executable mappings");
    return;
  }
  auto copy_out =
      reinterpret_cast<CopyOutOtaCodecConfig>(copy_out_address);
  if (!copy_out(codec_config, ota) ||
      !parse_codec_info(ota, codec_config, &g_encoder.codec)) {
    LOGE("failed to read or parse LHDC v3/v4 OTA configuration");
    return;
  }

  g_encoder.read_callback = read_callback;
  g_encoder.enqueue_callback = enqueue_callback;
  const uint32_t maximum_buffer_mtu =
      kBtBufferSize - kLhdcBufferOffset - sizeof(BtHdr);
  g_encoder.tx_mtu = std::min<uint32_t>(maximum_buffer_mtu, peer->peer_mtu);
  const uint32_t encoder_mtu = g_encoder.tx_mtu - kLhdcHeaderLength;

  g_encoder.handle = g_api.get_handle(static_cast<int>(g_encoder.codec.library_version));
  if (g_encoder.handle == nullptr) {
    LOGE("cannot get LHDC v%u encoder handle", g_encoder.codec.library_version);
    encoder_cleanup();
    return;
  }
  g_api.set_ext_function(g_encoder.handle, kExtFunctionAr, g_encoder.codec.has_ar,
                         nullptr, 0);
  g_api.set_ext_function(g_encoder.handle, kExtFunctionJas, g_encoder.codec.has_jas,
                         nullptr, 0);
  g_api.set_ext_function(g_encoder.handle, kExtFunctionLarc,
                         g_encoder.codec.has_larc, nullptr, 0);
  g_api.set_min_bitrate_limit(g_encoder.handle, g_encoder.codec.has_min_bitrate);
  const int result = g_api.init_encoder(
      g_encoder.handle, static_cast<int>(g_encoder.codec.sample_rate),
      static_cast<int>(g_encoder.codec.bits_per_sample), g_encoder.codec.quality_mode,
      g_encoder.codec.channel_split_mode > 1 ? 1 : 0, 0,
      static_cast<int>(encoder_mtu), static_cast<int>(g_encoder.codec.interval_ms));
  if (result != 0) {
    LOGE("LHDC v%u encoder initialization failed: %d",
         g_encoder.codec.library_version, result);
    encoder_cleanup();
    return;
  }
  g_api.set_max_bitrate(g_encoder.handle, g_encoder.codec.max_bitrate);
  if (g_api.set_bitrate(g_encoder.handle, g_encoder.codec.quality_mode) < 0) {
    LOGE("LHDC v%u bitrate configuration failed", g_encoder.codec.library_version);
    encoder_cleanup();
    return;
  }

  g_encoder.ready = true;
  feeding_reset();
  LOGI("LHDC v%u encoder ready: %u Hz, %u bit, quality %d, mtu %u, interval %u ms",
       g_encoder.codec.library_version + 1, g_encoder.codec.sample_rate,
       g_encoder.codec.bits_per_sample, g_encoder.codec.quality_mode,
       g_encoder.tx_mtu, g_encoder.codec.interval_ms);
}

bool read_one_frame(uint8_t* pcm, uint32_t bytes_requested, uint32_t* bytes_read) {
  uint32_t actual = g_encoder.read_callback(pcm, bytes_requested);
  if (actual == 0) return false;
  if (actual < bytes_requested) {
    std::memset(pcm + actual, 0, bytes_requested - actual);
    actual = bytes_requested;
  }
  *bytes_read = actual;
  return true;
}

void encode_frames(uint8_t frame_count, uint32_t samples_per_frame) {
  constexpr size_t kMaxPcmFrameBytes = 1920 * 2 * 4;
  uint8_t pcm[kMaxPcmFrameBytes];
  const uint32_t pcm_bytes_per_frame = samples_per_frame * g_encoder.codec.channels *
                                       g_encoder.codec.bits_per_sample / 8;
  if (pcm_bytes_per_frame == 0 || pcm_bytes_per_frame > sizeof(pcm)) {
    LOGE("invalid legacy PCM frame size: %u", pcm_bytes_per_frame);
    return;
  }

  while (frame_count != 0) {
    auto* buffer = static_cast<BtHdr*>(std::malloc(kBtBufferSize));
    if (buffer == nullptr) {
      LOGE("legacy packet allocation failed");
      return;
    }
    buffer->event = 0;
    buffer->len = 0;
    buffer->offset = kLhdcBufferOffset;
    buffer->layer_specific = 0;
    uint32_t packet_pcm_bytes = 0;
    uint32_t packet_frames = 0;
    uint32_t written = 0;

    do {
      uint32_t read_bytes = 0;
      if (!read_one_frame(pcm, pcm_bytes_per_frame, &read_bytes)) {
        g_encoder.feeding_counter += frame_count * pcm_bytes_per_frame;
        frame_count = 0;
        break;
      }
      uint32_t encoded_frames = 0;
      uint8_t* output = buffer->data + buffer->offset + buffer->len;
      const size_t output_capacity =
          kBtBufferSize - sizeof(BtHdr) - buffer->offset - buffer->len;
      written = 0;
      const int result = g_api.encode(g_encoder.handle, pcm, output, &written,
                                      &encoded_frames);
      if (result < 0 || written > output_capacity) {
        LOGE("legacy encode failed: result=%d written=%u capacity=%zu", result,
             written, output_capacity);
        std::free(buffer);
        return;
      }
      buffer->len = static_cast<uint16_t>(buffer->len + written);
      packet_pcm_bytes += read_bytes;
      packet_frames += encoded_frames;
      --frame_count;
    } while (written == 0 && frame_count != 0);

    if (buffer->len == 0) {
      std::free(buffer);
      continue;
    }
    buffer->layer_specific = static_cast<uint16_t>(
        (g_encoder.sequence++ << 8) | (packet_frames << 2));
    std::memcpy(buffer->data, &g_encoder.timestamp, sizeof(g_encoder.timestamp));
    g_encoder.timestamp += packet_frames * samples_per_frame;
    if (!g_encoder.enqueue_callback(buffer, 1, packet_pcm_bytes)) return;
  }
}

void send_frames(uint64_t timestamp_us) {
  if (!g_encoder.ready || g_encoder.handle == nullptr) return;
  const int block_size = g_api.get_block_size(g_encoder.handle);
  if (block_size <= 0) {
    LOGE("legacy get block size failed: %d", block_size);
    return;
  }
  const uint32_t samples_per_frame = static_cast<uint32_t>(block_size);
  const uint32_t pcm_bytes_per_frame = samples_per_frame * g_encoder.codec.channels *
                                       g_encoder.codec.bits_per_sample / 8;
  const uint32_t interval_us = g_encoder.codec.interval_ms * 1000;
  uint32_t elapsed_us = interval_us;
  if (g_encoder.last_frame_us != 0 && timestamp_us > g_encoder.last_frame_us) {
    elapsed_us = static_cast<uint32_t>(timestamp_us - g_encoder.last_frame_us);
  }
  g_encoder.last_frame_us = timestamp_us;
  g_encoder.feeding_counter += static_cast<uint32_t>(
      static_cast<uint64_t>(g_encoder.bytes_per_tick) * elapsed_us / interval_us);
  const uint32_t frames = g_encoder.feeding_counter / pcm_bytes_per_frame;
  g_encoder.feeding_counter -= frames * pcm_bytes_per_frame;
  if (frames != 0) {
    encode_frames(static_cast<uint8_t>(std::min<uint32_t>(frames, 255)),
                  samples_per_frame);
  }
}

void set_transmit_queue_length(size_t length) {
  g_encoder.transmit_queue_length = length;
  if (g_encoder.ready && g_encoder.codec.quality_mode == kQualityAuto) {
    g_api.adjust_bitrate(g_encoder.handle, length);
  }
}

bool install_legacy_encoder_interface() {
  dl_iterate_phdr(find_bluetooth_library, &g_bluetooth);
  if (g_bluetooth.base == 0 || !g_bluetooth.build_id_matches) {
    LOGE("unsupported libbluetooth_jni build; legacy patch not installed");
    return false;
  }
  auto* table = reinterpret_cast<EncoderInterface*>(
      g_bluetooth.base + kExpectedEncoderTableOffset);
  if (!is_empty_encoder_table(table)) {
    LOGE("LHDC v3/v4 empty encoder table does not match audited build");
    return false;
  }
  if (!is_read_only_data_range(reinterpret_cast<uintptr_t>(table),
                               sizeof(*table))) {
    LOGE("LHDC v3/v4 encoder table is outside a read-only or RELRO mapping");
    return false;
  }
  if (!load_legacy_api()) return false;

  const EncoderInterface replacement = {
      encoder_init, encoder_cleanup, feeding_reset, feeding_flush,
      get_encoder_interval_ms, get_effective_frame_size, send_frames,
      set_transmit_queue_length,
  };
  const long page_size = sysconf(_SC_PAGESIZE);
  if (page_size <= 0) {
    LOGE("cannot determine page size");
    return false;
  }
  const uintptr_t page_mask = static_cast<uintptr_t>(page_size - 1);
  const uintptr_t page_begin =
      reinterpret_cast<uintptr_t>(table) & ~page_mask;
  const uintptr_t page_end =
      (reinterpret_cast<uintptr_t>(table) + sizeof(*table) + page_mask) &
      ~page_mask;
  const size_t protection_size = page_end - page_begin;
  if (mprotect(reinterpret_cast<void*>(page_begin), protection_size,
               PROT_READ | PROT_WRITE) != 0) {
    LOGE("cannot make LHDC v3/v4 encoder table writable");
    return false;
  }
  const EncoderInterface original = *table;
  std::memcpy(table, &replacement, sizeof(replacement));
  if (mprotect(reinterpret_cast<void*>(page_begin), protection_size,
               PROT_READ) != 0) {
    LOGE("cannot restore read-only protection on the LHDC v3/v4 encoder table");
    std::memcpy(table, &original, sizeof(original));
    if (mprotect(reinterpret_cast<void*>(page_begin), protection_size,
                 PROT_READ) != 0) {
      LOGE("cannot restore read-only protection after rolling back the legacy table");
    }
    return false;
  }
  LOGI("LHDC v3/v4 encoder interface installed");
  return true;
}

__attribute__((constructor)) void initialize_legacy_patch() {
  install_legacy_encoder_interface();
}

}  // namespace
