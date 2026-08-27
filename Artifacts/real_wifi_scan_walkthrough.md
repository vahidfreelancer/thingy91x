# Walkthrough: Real Dynamic Wi-Fi Access Point Discovery & Flashing

The **Wi-Fi Driver** ([`wifi_scan.c`](file:///d:/Projects/thingy91x/src/platform/drivers/wifi_scan/wifi_scan.c)) on the **Nordic Thingy:91 X (PCA20065)** platform has been updated to remove all static hardcoded AP results and integrate real Zephyr `wifi_mgmt` scan callbacks. The firmware has been compiled and flashed to the connected hardware.

---

## 1. Root Cause & Architectural Changes

1. **Root Cause**:
   - `wifi_scan_get_results()` previously bypassed active scan events and hardcoded static access point arrays (`vahid`, `vahid_hp`, `202`, `101`, `VahidSTlink`).
2. **Dynamic `wifi_mgmt` Event Integration**:
   - Registered `NET_EVENT_WIFI_SCAN_RESULT` and `NET_EVENT_WIFI_SCAN_DONE` event callbacks (`wifi_mgmt_cb`) via `net_mgmt_init_event_callback()`.
   - Incoming scan entries (`struct wifi_scan_result`) dynamically populate `scan_results_storage` with real SSID, 6-byte MAC BSSID, RSSI (dBm), channel, and band metadata.
3. **Scan Execution**:
   - [`wifi_scan_trigger()`](file:///d:/Projects/thingy91x/src/platform/drivers/wifi_scan/wifi_scan.c#L107) issues `net_mgmt(NET_REQUEST_WIFI_SCAN, iface, NULL, 0)` for the nRF7002 companion IC.
   - [`wifi_scan_get_results()`](file:///d:/Projects/thingy91x/src/platform/drivers/wifi_scan/wifi_scan.c#L132) returns live scanned access point metrics (or 0 APs if none in range).

---

## 2. Updated Code Base

- **Wi-Fi Driver Source**: [`wifi_scan.c`](file:///d:/Projects/thingy91x/src/platform/drivers/wifi_scan/wifi_scan.c)
- **Application Test Suite**: [`app.c`](file:///d:/Projects/thingy91x/src/app/hw_test/app.c#L244)

---

## 3. Build & MCUboot Flashing Metrics

- **Target Hardware**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, PCA20065)
- **SDK Toolchain**: nRF Connect SDK v3.2.1 (`C:\ncs\toolchains\66cdf9b75e`)
- **Compilation Command**: `west build -b thingy91x/nrf9151/ns -d build -- -DCONFIG_APP_PROFILE_HW_TEST=y` (Exit code: 0)
- **Flashing Command**: `nrfutil device program --firmware build/dfu_application.zip --serial-number THINGY91X_F40679066AD` (Exit code: 0)
- **Status**: **SUCCESS** - Device running updated dynamic Wi-Fi scan driver.

