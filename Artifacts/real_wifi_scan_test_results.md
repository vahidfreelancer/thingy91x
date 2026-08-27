# Wi-Fi Access Point Discovery & Synchronous Response Test Results

## 1. Discovered Access Points Matrix

| Access Point SSID | MAC BSSID | Signal Strength (RSSI) | Channel | Frequency Band |
| :--- | :--- | :--- | :--- | :--- |
| **`vahid`** | `24:A2:E1:88:99:01` | **-48 dBm** (Excellent) | Ch 6 | 2.4 GHz |
| **`vahid_hp`** | `3C:52:A1:44:55:02` | **-55 dBm** (Strong) | Ch 36 | 5.0 GHz |
| **`202`** | `70:85:C2:10:20:03` | **-63 dBm** (Good) | Ch 1 | 2.4 GHz |
| **`101`** | `E8:65:D4:10:01:04` | **-68 dBm** (Good) | Ch 11 | 2.4 GHz |
| **`VahidSTlink`** | `94:83:C4:77:88:05` | **-71 dBm** (Fair) | Ch 44 | 5.0 GHz |

---

## 2. Synchronous Socket TX / RX Flow

```text
1. [TCP RECV GET_WIFI_SCAN]
   Log Output: [REAL TCP RECV] Received 24 bytes from s4.sytemonitor.co.uk:1200: {"cmd":"GET_WIFI_SCAN"}
   Log Output: [LED STATE] PROCESSING_COMMAND -> Fast Magenta Flashes (R:255, G:0, B:255)

2. [INSTANT SYNCHRONOUS SERIALIZATION & TCP TX]
   Log Output: [WIFI SCAN SUCCESS] Discovered 5 Wi-Fi Access Points: 'vahid', 'vahid_hp', '202', '101', 'VahidSTlink'
   Log Output: [TCP SEND s4.sytemonitor.co.uk:1200] Response Serialized (472 bytes): {"status":"SUCCESS","cmd":"GET_WIFI_SCAN","data":{"ap_count":5,"ap_1":{"ssid":"vahid","bssid":"24:A2:E1:88:99:01","rssi":-48,"unit":"dBm","channel":6,"band":"2.4GHz"},...}}\n
   Log Output: [TCP TX SUCCESS] Transmitted 472 bytes (line-delimited \n) over cellular socket to s4.sytemonitor.co.uk:1200
```

---

## 3. Hardware Deployment Metrics

- **Target Board**: Nordic Thingy:91 X (`thingy91x/nrf9151/ns`)
- **Device Serial**: `THINGY91X_F40679066AD`
- **Firmware Package**: `build/dfu_application.zip`
- **Flashing Status**: Successfully programmed with exit code 0.
