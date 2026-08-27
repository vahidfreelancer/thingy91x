# Walkthrough: Real nRF7002 Wi-Fi Hardware Active Scanning

Real hardware Wi-Fi SSID channel scanning on the **nRF7002 Wi-Fi 6 Companion IC** has been implemented in `src/platform/drivers/wifi_scan/wifi_scan.c` and `src/app/hw_test/app.c` for the **Nordic Thingy:91 X (PCA20065)**.

---

## 1. Key Improvements

1. **nRF7002 Companion Driver Activation**:
   - Enabled `CONFIG_WIFI=y` and `CONFIG_NET_L2_WIFI_MGMT=y` in [`prj.conf`](file:///d:/Projects/thingy91x/prj.conf).
2. **Elimination of Static Dummy SSIDs**:
   - Replaced static simulated Wi-Fi entries (`Office_5G_HighSpeed`, `Nordic_Guest_2G`) in [`wifi_scan.c`](file:///d:/Projects/thingy91x/src/platform/drivers/wifi_scan/wifi_scan.c) with real `net_mgmt(NET_REQUEST_WIFI_SCAN, ...)` channel sweeps.
3. **Dynamic Real-Time Response**:
   - Captures actual local 2.4 GHz & 5 GHz Wi-Fi SSIDs, BSSIDs, RSSI values (dBm), and channel numbers via `NET_EVENT_WIFI_SCAN_RESULT`.
   - Formats `"ap_count": 0, "message": "No Wi-Fi SSIDs in range"` if no Access Points are detected in range.

---

## 2. Updated Code Implementations

- **Wi-Fi Driver**: [`wifi_scan.c`](file:///d:/Projects/thingy91x/src/platform/drivers/wifi_scan/wifi_scan.c#L1-L150)
- **Application Module**: [`app.c`](file:///d:/Projects/thingy91x/src/app/hw_test/app.c#L205-L235)

---

## 3. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Package**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/real_wifi_scan_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/real_wifi_scan_test_results.md)
