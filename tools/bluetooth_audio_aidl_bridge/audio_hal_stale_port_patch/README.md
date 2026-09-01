# Bluetooth Audio stale-output isolation

This rebuilds Android 12L's standard `audio.bluetooth.default.so` for the
SM7475 vendor API 32 baseline.

During an A2DP codec reconfiguration, the stock HAL toggles an existing port
from `STARTED` to `DISABLED` and then back to `STANDBY` across the rapid
session-end/session-start callbacks. AudioFlinger can therefore resume the old
output and write stale PCM into the replacement session's FMQ before
AudioPolicy closes that output.

The `stale-port` mode changes only the software A2DP callback path: an existing
port stays `DISABLED` after any session replacement. Stock `out_write()` then
consumes and drops the old output's residual PCM, while the fresh AudioPolicy
output starts from `STANDBY` and binds to the new session normally. Other
Bluetooth session types retain the Android 12L state transition.

Build a stock baseline and the patch with `build.sh`, then compare the original
device HAL, baseline and patch with `audit.sh`. The audit requires the original
dependency order, CFI, Android packed RELA, standard RELR, BTI/PAC, and an
unchanged dynamic ABI between the baseline and patch.
