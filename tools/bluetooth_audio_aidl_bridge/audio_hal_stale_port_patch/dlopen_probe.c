/* SPDX-License-Identifier: Apache-2.0 */

#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s AUDIO_BLUETOOTH_HAL\n", argv[0]);
    return 2;
  }

  void* library = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
  if (library == NULL) {
    fprintf(stderr, "dlopen failed: %s\n", dlerror());
    return 3;
  }
  dlerror();
  void* module = dlsym(library, "HMI");
  const char* error = dlerror();
  if (module == NULL || error != NULL) {
    fprintf(stderr, "HMI lookup failed: %s\n",
            error == NULL ? "null symbol" : error);
    return 4;
  }

  printf("audio_bluetooth_hal_dlopen_probe=PASS\n");
  fflush(NULL);
  _exit(0);
}
