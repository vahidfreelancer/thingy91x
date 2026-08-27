# Implementation Plan: Real nRF7002 Wi-Fi Active SSID Scanning

Enable real hardware Wi-Fi SSID scanning on the **nRF7002 Wi-Fi 6 Companion IC** via Zephyr `wifi_mgmt` APIs to replace static simulated SSIDs with real surrounding Access Point data.

---

## 1. Feature Analysis

- **Current State**:
  `wifi_scan.c` returns hardcoded dummy Wi-Fi SSIDs (`Office_5G_HighSpeed`, `Nordic_Guest_2G`) when `GET_WIFI_SCAN` is called.
- **Goal**:
  Enable Nordic `nRF7002` Wi-Fi driver (`CONFIG_WIFI_NRF700X=y`, `CONFIG_NET_L2_WIFI_MGMT=y`) in `prj.conf` and update `wifi_scan.c` to perform real passive/active scans on 2.4 GHz & 5 GHz channels, returning actual nearby BSSIDs, SSIDs, RSSI values, and channels.

---

## Proposed Changes

### Kconfig Configuration (`prj.conf`)

#### [MODIFY] [prj.conf](file:///d:/Projects/thingy91x/prj.conf)
- Enable nRF7002 Wi-Fi companion IC driver and Wi-Fi management subsystem:
  ```ini
  CONFIG_WIFI=y
  CONFIG_WIFI_NRF700X=y
  CONFIG_NET_L2_WIFI_MGMT=y
  ```

---

### Platform Drivers (`src/platform/drivers/wifi_scan/`)

#### [MODIFY] [wifi_scan.c](file:///d:/Projects/thingy91x/src/platform/drivers/wifi_scan/wifi_scan.c)
- Include `<zephyr/net/wifi_mgmt.h>` and call `net_mgmt(NET_REQUEST_WIFI_SCAN, ...)` to request hardware scans from the nRF7002 chip.
- Register `NET_EVENT_WIFI_SCAN_RESULT` callback to capture live SSIDs, BSSIDs, RSSI, and channel numbers.
- Dynamic fallback: If `nRF7002` hardware is not attached or returns 0 APs, format a clear dynamic status message ("No Wi-Fi SSIDs in range") rather than static dummy SSIDs.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/real_wifi_scan_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/real_wifi_scan_implementation_plan.md)
#### [NEW] [Artifacts/real_wifi_scan_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/real_wifi_scan_walkthrough.md)
#### [NEW] [Artifacts/real_wifi_scan_test_results.md](file:///d:/Projects/thingy91x/Artifacts/real_wifi_scan_test_results.md)

---

## Verification Plan

### Automated Build Verification
- Compile with `west build -b thingy91x/nrf9151/ns -d build -- -DCONFIG_APP_PROFILE_HW_TEST=y`.

### Hardware Flashing & Live Testing
- Flash `build/dfu_application.zip` to **Thingy:91 X** (`THINGY91X_F40679066AD`).
- Run `Tools/Terminal/monitor.py`, press BUTTON1, send `GET_WIFI_SCAN`, and verify live local Wi-Fi SSIDs and BSSIDs!
