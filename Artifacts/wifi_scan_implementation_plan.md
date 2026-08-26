# Implementation Plan: Step 4 - Wi-Fi Location Scanner & Station Connectivity Driver (nRF7002)

Implement a comprehensive Wi-Fi Driver (`src/platform/drivers/wifi_scan/`) for the **nRF7002 Wi-Fi 6 companion IC** on the Nordic Thingy:91 X platform to support:
1. **Passive Location Scanning**: Scans nearby 2.4/5 GHz Wi-Fi BSSID MAC addresses and RSSI levels for indoor cloud location services (nRF Cloud LBS).
2. **Full Station Mode (STA) Connectivity**: Connects to Wi-Fi access points (WPA2/WPA3), retrieves DHCP IP address, and handles TCP/UDP socket data transfers.
3. **Power Management**: Soft sleep suspension (`CONFIG_PM_DEVICE`) for battery budget yielding.

## Proposed Changes

### Hardware Abstraction Layer (HAL) - Drivers

#### [NEW] [wifi_scan.h](file:///d:/Projects/thingy91x/src/platform/drivers/wifi_scan/wifi_scan.h)
- Declare connection state enum: `WIFI_STATE_DISCONNECTED`, `WIFI_STATE_SCANNING`, `WIFI_STATE_CONNECTING`, `WIFI_STATE_CONNECTED`.
- Declare `struct wifi_scan_result`: `ssid[33]`, `bssid[6]`, `rssi_dbm`, `channel`, `band`.
- Declare `struct wifi_scan_data`: `ap_count`, `results[8]`, `valid`.
- Declare `struct wifi_status`: `state`, `connected_ssid[33]`, `ip_addr[16]`, `rssi_dbm`, `channel`.
- Expose driver interface functions:
  - `int wifi_scan_init(void)`: Initialize nRF7002 Wi-Fi subsystem.
  - `int wifi_scan_trigger(void)`: Trigger passive 2.4/5 GHz access point channel scan.
  - `int wifi_scan_get_results(struct wifi_scan_data *data)`: Retrieve array of scanned BSSIDs and RSSIs.
  - `int wifi_connect(const char *ssid, const char *password)`: Connect to Wi-Fi Access Point (Station STA Mode).
  - `int wifi_disconnect(void)`: Disconnect from current Wi-Fi network.
  - `int wifi_get_status(struct wifi_status *status)`: Retrieve current connection status, IP address, and RSSI.
  - `int wifi_scan_sleep(void)`: Put nRF7002 companion IC into low-power sleep state.

#### [NEW] [wifi_scan.c](file:///d:/Projects/thingy91x/src/platform/drivers/wifi_scan/wifi_scan.c)
- Implement driver logic using Zephyr net / wifi driver APIs (`zephyr,wifi`), Devicetree macros (`DT_HAS_COMPAT_STATUS_OKAY`), BSSID formatting, WPA2/WPA3 authentication, POSIX error return codes, and software simulation fallback for off-target validation.

---

### Devicetree & Board Overlay

#### [MODIFY] [thingy91x_nrf9151_ns.overlay](file:///d:/Projects/thingy91x/boards/thingy91x_nrf9151_ns.overlay)
- Verify `nrf7002` SPI / QSPI companion IC devicetree node overlay configuration.

---

### Application Integration & Build System

#### [MODIFY] [CMakeLists.txt](file:///d:/Projects/thingy91x/src/app/asset_tracker/CMakeLists.txt)
- Include `../../platform/drivers/wifi_scan/wifi_scan.c` and `../../platform/drivers/wifi_scan` in `asset_tracker/CMakeLists.txt`.

#### [MODIFY] [app.c](file:///d:/Projects/thingy91x/src/app/asset_tracker/app.c)
- Call `wifi_scan_init()` during boot, trigger passive Wi-Fi scan, display scanned BSSIDs/RSSIs, and demonstrate Wi-Fi STA connection option inside `telemetry_work_handler()`.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/wifi_scan_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/wifi_scan_implementation_plan.md)
#### [NEW] [Artifacts/wifi_scan_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/wifi_scan_walkthrough.md)
#### [NEW] [Artifacts/wifi_scan_test_results.md](file:///d:/Projects/thingy91x/Artifacts/wifi_scan_test_results.md)
- Save design plans, execution walkthrough, and hardware test scenario results in `Artifacts/`.

---

## Verification Plan

### Automated / Build Verification
- Compile firmware using nRF Connect SDK toolchain (`west build -b thingy91x/nrf9151/ns -- -DCONFIG_APP_PROFILE_ASSET_TRACKER=y`) with `CONFIG_BOOTLOADER_MCUBOOT=y`.

### Hardware Flashing & Live Testing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Monitor live serial console logs to verify Wi-Fi scan trigger, scanned BSSIDs/RSSIs, and Station Mode connection status.
