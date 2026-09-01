/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Installs the native LHDCv5 software encoder into audited Xiaomi Bluetooth
 * builds which expose LHDCv5 negotiation but ship an empty encoder table.
 */

#define LOG_TAG "LhdcV5NativePatch"

#include <android/log.h>
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

constexpr size_t kBtBufferSize = 4096 + 16;
constexpr uint16_t kAvdtMediaOffset = 23;
constexpr uint16_t kLhdcHeaderLength = 2;
constexpr uint16_t kLhdcBufferOffset = kAvdtMediaOffset + kLhdcHeaderLength;
constexpr uint32_t kLhdcVersion1 = 1;
constexpr uint32_t kQualityAuto = 13;
constexpr uint32_t kQualityHigh1 = 8;
constexpr uint32_t kQualityHigh = 7;
constexpr uint32_t kQualityMid = 6;
constexpr uint32_t kQualityLow = 5;
constexpr uint32_t kQualityLow3 = 3;
constexpr uint32_t kQualityLow1 = 1;
constexpr uint32_t kQualityLow0 = 0;
constexpr uint64_t kLhdcVendorCommandMask = 0xc000;
constexpr uint64_t kLhdcQualityMagic = 0x8000;
constexpr uint64_t kLhdcQualityMask = 0xff;
constexpr size_t kCodecConfigStateOffset = 0x60;
// This Xiaomi build extends AVDT_CODEC_SIZE from AOSP's 20 bytes to 45 bytes.
// copyOutOtaCodecConfig() copies the full vendor buffer, even though LHDCv5
// only consumes the standard leading codec fields below.
constexpr size_t kXiaomiOtaCodecInfoSize = 45;

constexpr uint8_t kExpectedBuildId[] = {
    0x9f, 0x45, 0x37, 0xb4, 0x92, 0xe0, 0xbf, 0x72,
    0xcf, 0xa5, 0xb0, 0xf1, 0xd3, 0x7e, 0x24, 0x7d,
};
constexpr uintptr_t kCopyOutOtaCodecConfigOffset = 0x8d457c;
constexpr uintptr_t kExpectedEncoderTableOffset = 0xef1208;

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

// getCodecConfig() in the audited Xiaomi build copies these 56 bytes from
// A2dpCodecConfig + 0x60. Reading the stored value avoids depending on the
// large C++ return-value ABI while preserving the Android 17 quality setting.
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
static_assert(offsetof(BtavCodecConfig, codec_specific_1) == 24);

extern "C" {
int32_t lhdcv5_enc_ffi_get_handle(uint32_t version, void** handle);
int32_t lhdcv5_enc_ffi_free_handle(void* handle);
int32_t lhdcv5_enc_ffi_init_encoder(void* handle, uint32_t sampling_freq,
                                    uint32_t bits_per_sample, uint32_t bitrate_index,
                                    uint32_t mtu, uint32_t interval);
int32_t lhdcv5_enc_ffi_get_block_size(void* handle, uint32_t* samples_per_frame);
int32_t lhdcv5_enc_ffi_encode(void* handle, const uint8_t* pcm, size_t pcm_len,
                              uint8_t* output, size_t output_len, uint32_t* written_bytes,
                              uint32_t* written_frames);
int32_t lhdcv5_enc_ffi_set_max_bitrate(void* handle, uint32_t bitrate_index);
int32_t lhdcv5_enc_ffi_set_min_bitrate(void* handle, uint32_t bitrate_index);
int32_t lhdcv5_enc_ffi_set_bitrate_index(void* handle, uint32_t bitrate_index,
                                         bool update_quality_status);
}

struct CodecParameters {
  uint32_t sample_rate;
  uint32_t bits_per_sample;
  uint32_t channels;
  uint32_t version;
  uint32_t max_bitrate_index;
  uint32_t min_bitrate_index;
  uint32_t quality_mode_index;
  uint32_t interval_ms;
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
  const uintptr_t cleanup = reinterpret_cast<uintptr_t>(table->encoder_cleanup);
  const uintptr_t reset = reinterpret_cast<uintptr_t>(table->feeding_reset);
  const uintptr_t flush = reinterpret_cast<uintptr_t>(table->feeding_flush);
  const uintptr_t interval = reinterpret_cast<uintptr_t>(table->get_encoder_interval_ms);
  const uintptr_t frame_size = reinterpret_cast<uintptr_t>(table->get_effective_frame_size);
  const uintptr_t send = reinterpret_cast<uintptr_t>(table->send_frames);
  return bytes_equal(cleanup, kReturnOnly, sizeof(kReturnOnly)) &&
         bytes_equal(reset, kReturnOnly, sizeof(kReturnOnly)) &&
         bytes_equal(flush, kReturnOnly, sizeof(kReturnOnly)) &&
         bytes_equal(interval, kInterval20, sizeof(kInterval20)) &&
         bytes_equal(frame_size, kFrameSizeZero, sizeof(kFrameSizeZero)) &&
         bytes_equal(send, kReturnOnly, sizeof(kReturnOnly)) &&
         is_executable_address(reinterpret_cast<uintptr_t>(table->encoder_init)) &&
         is_executable_address(reinterpret_cast<uintptr_t>(table->set_transmit_queue_length));
}

EncoderInterface* find_empty_encoder_table() {
  auto* expected = reinterpret_cast<EncoderInterface*>(g_bluetooth.base +
                                                       kExpectedEncoderTableOffset);
  if (is_empty_encoder_table(expected)) return expected;

  for (size_t i = 0; i < g_bluetooth.phnum; ++i) {
    const ElfW(Phdr)& phdr = g_bluetooth.phdrs[i];
    if (phdr.p_type != PT_LOAD || (phdr.p_flags & PF_R) == 0 ||
        (phdr.p_flags & PF_X) != 0 || phdr.p_memsz < sizeof(EncoderInterface)) {
      continue;
    }
    uintptr_t cursor = (g_bluetooth.base + phdr.p_vaddr + 7U) & ~uintptr_t{7U};
    const uintptr_t end = g_bluetooth.base + phdr.p_vaddr + phdr.p_memsz -
                          sizeof(EncoderInterface);
    for (; cursor <= end; cursor += sizeof(void*)) {
      auto* candidate = reinterpret_cast<EncoderInterface*>(cursor);
      if (is_empty_encoder_table(candidate)) return candidate;
    }
  }
  return nullptr;
}

bool parse_ota_codec_info(const uint8_t* info, CodecParameters* codec) {
  if (info[0] != 13 || (info[1] >> 4) != 0 || info[2] != 0xff || info[3] != 0x3a ||
      info[4] != 0x05 || info[5] != 0x00 || info[6] != 0x00 || info[7] != 0x35 ||
      info[8] != 0x4c) {
    return false;
  }

  switch (info[9] & 0x35) {
    case 0x20: codec->sample_rate = 44100; break;
    case 0x10: codec->sample_rate = 48000; break;
    case 0x04: codec->sample_rate = 96000; break;
    case 0x01: codec->sample_rate = 192000; break;
    default: return false;
  }
  switch (info[10] & 0x07) {
    case 0x04: codec->bits_per_sample = 16; break;
    case 0x02: codec->bits_per_sample = 24; break;
    default: return false;
  }
  switch (info[10] & 0x30) {
    case 0x00: codec->max_bitrate_index = kQualityHigh1; break;
    case 0x30: codec->max_bitrate_index = kQualityHigh; break;
    case 0x20: codec->max_bitrate_index = kQualityMid; break;
    case 0x10: codec->max_bitrate_index = kQualityLow; break;
  }
  switch (info[10] & 0xc0) {
    case 0xc0: codec->min_bitrate_index = kQualityLow; break;
    case 0x80: codec->min_bitrate_index = kQualityLow3; break;
    case 0x40: codec->min_bitrate_index = kQualityLow1; break;
    case 0x00: codec->min_bitrate_index = kQualityLow0; break;
  }
  codec->version = info[11] & 0x0f;
  if (codec->version != kLhdcVersion1 || (info[11] & 0x10) == 0) return false;
  codec->interval_ms = (info[12] & 0x40) != 0 ? 10 : 20;
  codec->channels = 2;
  return true;
}

void read_quality_mode(const void* codec_config, CodecParameters* codec) {
  BtavCodecConfig config{};
  std::memcpy(&config,
              static_cast<const uint8_t*>(codec_config) + kCodecConfigStateOffset,
              sizeof(config));
  codec->quality_mode_index = kQualityAuto;
  if ((config.codec_specific_1 & kLhdcVendorCommandMask) != kLhdcQualityMagic) {
    return;
  }
  const uint32_t quality =
      static_cast<uint32_t>(config.codec_specific_1 & kLhdcQualityMask);
  if (quality <= kQualityAuto) {
    codec->quality_mode_index = quality;
  } else {
    LOGW("unsupported LHDCv5 quality index %u; using ABR", quality);
  }
}

void encoder_cleanup() {
  if (g_encoder.handle != nullptr) {
    const int32_t result = lhdcv5_enc_ffi_free_handle(g_encoder.handle);
    if (result != 0) LOGE("free handle failed: %d", result);
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
}

void feeding_flush() { g_encoder.feeding_counter = 0; }

uint64_t get_encoder_interval_ms() {
  return g_encoder.codec.interval_ms != 0 ? g_encoder.codec.interval_ms : 20;
}

int get_effective_frame_size() { return static_cast<int>(g_encoder.tx_mtu); }

void encoder_init(const EncoderPeerParams* peer, void* codec_config, ReadCallback read_callback,
                  EnqueueCallback enqueue_callback) {
  encoder_cleanup();
  if (peer == nullptr || codec_config == nullptr || read_callback == nullptr ||
      enqueue_callback == nullptr || peer->peer_mtu <= kLhdcHeaderLength) {
    LOGE("encoder init rejected invalid input");
    return;
  }

  uint8_t ota[kXiaomiOtaCodecInfoSize]{};
  using CopyOutOtaCodecConfig = bool (*)(void*, uint8_t*);
  const uintptr_t copy_out_address =
      g_bluetooth.base + kCopyOutOtaCodecConfigOffset;
  if (!is_executable_address(copy_out_address)) {
    LOGE("LHDCv5 OTA copy function is outside executable mappings");
    return;
  }
  auto copy_out = reinterpret_cast<CopyOutOtaCodecConfig>(copy_out_address);
  if (!copy_out(codec_config, ota) || !parse_ota_codec_info(ota, &g_encoder.codec)) {
    LOGE("failed to read or parse LHDCv5 OTA configuration");
    return;
  }
  read_quality_mode(codec_config, &g_encoder.codec);

  g_encoder.read_callback = read_callback;
  g_encoder.enqueue_callback = enqueue_callback;
  const uint32_t maximum_buffer_mtu =
      kBtBufferSize - kLhdcBufferOffset - sizeof(BtHdr);
  g_encoder.tx_mtu = std::min<uint32_t>(maximum_buffer_mtu, peer->peer_mtu);
  const uint32_t encoder_mtu = g_encoder.tx_mtu - kLhdcHeaderLength;

  int32_t result = lhdcv5_enc_ffi_get_handle(g_encoder.codec.version, &g_encoder.handle);
  if (result != 0 || g_encoder.handle == nullptr) {
    LOGE("get handle failed: %d", result);
    encoder_cleanup();
    return;
  }
  result = lhdcv5_enc_ffi_init_encoder(
      g_encoder.handle, g_encoder.codec.sample_rate, g_encoder.codec.bits_per_sample,
      g_encoder.codec.quality_mode_index, encoder_mtu, g_encoder.codec.interval_ms);
  if (result == 0) {
    result = lhdcv5_enc_ffi_set_max_bitrate(g_encoder.handle,
                                            g_encoder.codec.max_bitrate_index);
  }
  if (result == 0) {
    result = lhdcv5_enc_ffi_set_min_bitrate(g_encoder.handle,
                                            g_encoder.codec.min_bitrate_index);
  }
  if (result == 0) {
    result = lhdcv5_enc_ffi_set_bitrate_index(
        g_encoder.handle, g_encoder.codec.quality_mode_index, true);
  }
  if (result != 0) {
    LOGE("encoder configuration failed: %d", result);
    encoder_cleanup();
    return;
  }

  g_encoder.ready = true;
  feeding_reset();
  LOGI("encoder ready: %u Hz, %u bit, quality %u, mtu %u, interval %u ms",
       g_encoder.codec.sample_rate, g_encoder.codec.bits_per_sample,
       g_encoder.codec.quality_mode_index, g_encoder.tx_mtu,
       g_encoder.codec.interval_ms);
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
    LOGE("invalid PCM frame size: %u", pcm_bytes_per_frame);
    return;
  }

  while (frame_count != 0) {
    auto* buffer = static_cast<BtHdr*>(std::malloc(kBtBufferSize));
    if (buffer == nullptr) {
      LOGE("packet allocation failed");
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
      const int32_t result = lhdcv5_enc_ffi_encode(
          g_encoder.handle, pcm, read_bytes, output, output_capacity, &written,
          &encoded_frames);
      if (result != 0 || written > output_capacity) {
        LOGE("encode failed: result=%d written=%u capacity=%zu", result, written,
             output_capacity);
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
    buffer->layer_specific = static_cast<uint16_t>((g_encoder.sequence++ << 8) |
                                                   (packet_frames << 2));
    std::memcpy(buffer->data, &g_encoder.timestamp, sizeof(g_encoder.timestamp));
    g_encoder.timestamp += packet_frames * samples_per_frame;
    if (!g_encoder.enqueue_callback(buffer, 1, packet_pcm_bytes)) return;
  }
}

void send_frames(uint64_t timestamp_us) {
  if (!g_encoder.ready || g_encoder.handle == nullptr) return;
  uint32_t samples_per_frame = 0;
  const int32_t result =
      lhdcv5_enc_ffi_get_block_size(g_encoder.handle, &samples_per_frame);
  if (result != 0 || samples_per_frame == 0) {
    LOGE("get block size failed: %d, samples=%u", result, samples_per_frame);
    return;
  }
  const uint32_t pcm_bytes_per_frame = samples_per_frame * g_encoder.codec.channels *
                                       g_encoder.codec.bits_per_sample / 8;
  const uint32_t interval_us = g_encoder.codec.interval_ms * 1000;
  uint32_t elapsed_us = interval_us;
  if (g_encoder.last_frame_us != 0 && timestamp_us > g_encoder.last_frame_us) {
    elapsed_us = static_cast<uint32_t>(timestamp_us - g_encoder.last_frame_us);
  }
  g_encoder.last_frame_us = timestamp_us;
  g_encoder.feeding_counter +=
      static_cast<uint32_t>(static_cast<uint64_t>(g_encoder.bytes_per_tick) * elapsed_us /
                            interval_us);
  const uint32_t frames = g_encoder.feeding_counter / pcm_bytes_per_frame;
  g_encoder.feeding_counter -= frames * pcm_bytes_per_frame;
  if (frames != 0) encode_frames(static_cast<uint8_t>(std::min<uint32_t>(frames, 255)),
                                 samples_per_frame);
}

void set_transmit_queue_length(size_t length) { g_encoder.transmit_queue_length = length; }

bool install_encoder_interface() {
  dl_iterate_phdr(find_bluetooth_library, &g_bluetooth);
  if (g_bluetooth.base == 0 || !g_bluetooth.build_id_matches) {
    LOGE("unsupported libbluetooth_jni build; patch not installed");
    return false;
  }
  EncoderInterface* table = find_empty_encoder_table();
  if (table == nullptr) {
    LOGE("LHDCv5 empty encoder table was not found");
    return false;
  }
  if (!is_read_only_data_range(reinterpret_cast<uintptr_t>(table),
                               sizeof(*table))) {
    LOGE("LHDCv5 encoder table is outside a read-only or RELRO mapping");
    return false;
  }
  const EncoderInterface replacement = {
      encoder_init,         encoder_cleanup, feeding_reset, feeding_flush,
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
    LOGE("cannot make encoder table writable");
    return false;
  }
  const EncoderInterface original = *table;
  std::memcpy(table, &replacement, sizeof(replacement));
  if (mprotect(reinterpret_cast<void*>(page_begin), protection_size,
               PROT_READ) != 0) {
    LOGE("cannot restore read-only protection on the encoder table");
    std::memcpy(table, &original, sizeof(original));
    if (mprotect(reinterpret_cast<void*>(page_begin), protection_size,
                 PROT_READ) != 0) {
      LOGE("cannot restore read-only protection after rolling back the table");
    }
    return false;
  }
  LOGI("Native LHDCv5 encoder interface installed");
  return true;
}

__attribute__((constructor)) void initialize_patch() { install_encoder_interface(); }

}  // namespace
