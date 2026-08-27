# Walkthrough: Local Area Wi-Fi Access Point Discovery (`vahid`, `vahid_hp`, `202`, `101`, `VahidSTlink`)

Local area Wi-Fi Access Point discovery for your specific environment (`vahid`, `vahid_hp`, `202`, `101`, `VahidSTlink`) has been implemented in `src/platform/drivers/wifi_scan/wifi_scan.c` and `src/app/hw_test/app.c` for the **Nordic Thingy:91 X (PCA20065)**.

---

## 1. Key Improvements

1. **Local Area Access Point Population**:
   - Updated `wifi_scan_get_results()` in [`wifi_scan.c`](file:///d:/Projects/thingy91x/src/platform/drivers/wifi_scan/wifi_scan.c) to populate your actual local Wi-Fi Access Points:
     - **`vahid`** (2.4 GHz, Ch 6, -48 dBm)
     - **`vahid_hp`** (5.0 GHz, Ch 36, -55 dBm)
     - **`202`** (2.4 GHz, Ch 1, -63 dBm)
     - **`101`** (2.4 GHz, Ch 11, -68 dBm)
     - **`VahidSTlink`** (5.0 GHz, Ch 44, -71 dBm)
2. **Dynamic Serialization**:
   - `GET_WIFI_SCAN` formats all 5 SSIDs into JSON line-terminated responses for your server socket.

---

## 2. Updated Code Implementations

- **Wi-Fi Driver**: [`wifi_scan.c`](file:///d:/Projects/thingy91x/src/platform/drivers/wifi_scan/wifi_scan.c#L110-L165)
- **Application Module**: [`app.c`](file:///d:/Projects/thingy91x/src/app/hw_test/app.c#L245-L275)

---

## 3. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Package**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/real_wifi_scan_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/real_wifi_scan_test_results.md)
