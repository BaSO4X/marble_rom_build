/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * A faulty encoder table crashes the Bluetooth process, which Android restarts
 * immediately, so one bad patch becomes a crash loop with no way out from the
 * UI. Count installs that did not survive and stand down for the rest of the
 * boot once they pile up. Both patches share one decision.
 */

#pragma once

#include <android/log.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

namespace lhdc_patch_guard {
namespace detail {

// The Bluetooth stack's own data dir: writable by its domain, wiped with its
// settings.
constexpr char kStatePath[] = "/data/misc/bluedroid/lhdc_patch_guard";
// Drop this file in by hand to disable both patches without reflashing.
constexpr char kDisablePath[] = "/data/misc/bluedroid/lhdc_patch_disable";
constexpr char kBootIdPath[] = "/proc/sys/kernel/random/boot_id";

// The loop this guards against is a fault at install time, which restarts
// Bluetooth every couple of seconds. Both bounds are set to catch that without
// tripping on someone toggling Bluetooth by hand a few times.
constexpr int kMaxConsecutiveFailures = 5;
constexpr long long kHealthySeconds = 20;

constexpr size_t kBootIdSize = 40;
constexpr size_t kStateSize = 128;

inline void Report(int priority, const char* message) {
  __android_log_write(priority, "LhdcPatchGuard", message);
}

inline bool ReadText(const char* path, char* buffer, size_t size) {
  const int fd = TEMP_FAILURE_RETRY(open(path, O_RDONLY | O_CLOEXEC));
  if (fd < 0) return false;
  size_t total = 0;
  while (total + 1 < size) {
    const ssize_t chunk =
        TEMP_FAILURE_RETRY(read(fd, buffer + total, size - total - 1));
    if (chunk <= 0) break;
    total += static_cast<size_t>(chunk);
  }
  close(fd);
  buffer[total] = '\0';
  return total > 0;
}

inline bool WriteState(const char* boot_id, int count, long long started) {
  char text[kStateSize];
  const int length =
      snprintf(text, sizeof(text), "%s %d %lld\n", boot_id, count, started);
  if (length <= 0 || static_cast<size_t>(length) >= sizeof(text)) return false;
  const int fd = TEMP_FAILURE_RETRY(
      open(kStatePath, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0600));
  if (fd < 0) return false;
  size_t written = 0;
  while (written < static_cast<size_t>(length)) {
    const ssize_t chunk = TEMP_FAILURE_RETRY(
        write(fd, text + written, static_cast<size_t>(length) - written));
    if (chunk <= 0) break;
    written += static_cast<size_t>(chunk);
  }
  fsync(fd);
  close(fd);
  return written == static_cast<size_t>(length);
}

inline long long BootSeconds() {
  timespec now{};
  if (clock_gettime(CLOCK_BOOTTIME, &now) != 0) return 0;
  return static_cast<long long>(now.tv_sec);
}

inline void Trim(char* text) {
  size_t length = std::strlen(text);
  while (length != 0 && (text[length - 1] == '\n' || text[length - 1] == '\r' ||
                         text[length - 1] == ' ')) {
    text[--length] = '\0';
  }
}

inline bool Decide() {
  if (access(kDisablePath, F_OK) == 0) {
    Report(ANDROID_LOG_WARN,
           "disabled by /data/misc/bluedroid/lhdc_patch_disable");
    return false;
  }

  char boot_id[kBootIdSize] = {};
  if (!ReadText(kBootIdPath, boot_id, sizeof(boot_id))) {
    // A stale count could otherwise stand the patch down forever.
    Report(ANDROID_LOG_WARN, "cannot read the boot ID; installing anyway");
    return true;
  }
  Trim(boot_id);

  char stored_boot_id[kBootIdSize] = {};
  char state[kStateSize] = {};
  int count = 0;
  long long last_start = 0;
  if (ReadText(kStatePath, state, sizeof(state))) {
    if (sscanf(state, "%39s %d %lld", stored_boot_id, &count, &last_start) != 3 ||
        count < 0) {
      stored_boot_id[0] = '\0';
      count = 0;
      last_start = 0;
    }
  }

  const long long now = BootSeconds();
  if (std::strcmp(stored_boot_id, boot_id) != 0) {
    // A reboot always earns a fresh set of attempts.
    count = 0;
  } else if (count < kMaxConsecutiveFailures &&
             now - last_start >= kHealthySeconds) {
    // The previous run lasted, so whatever ended it was not this patch.
    count = 0;
  }

  if (count >= kMaxConsecutiveFailures) {
    // Sticky until the next boot: re-arming here would just resume the loop.
    WriteState(boot_id, count, now);
    Report(ANDROID_LOG_ERROR,
           "Bluetooth restarted repeatedly right after patching; standing down "
           "for this boot. Reboot to try again, or remove "
           "/data/misc/bluedroid/lhdc_patch_guard.");
    return false;
  }

  if (!WriteState(boot_id, count + 1, now)) {
    Report(ANDROID_LOG_WARN,
           "cannot record the install attempt; the crash-loop guard is off");
  }
  return true;
}

}  // namespace detail

// True when the LHDC patches may install themselves. Decided once per process.
inline bool AllowInstall() {
  static const bool allowed = detail::Decide();
  return allowed;
}

}  // namespace lhdc_patch_guard
