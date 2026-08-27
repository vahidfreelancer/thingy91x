# Local Area Wi-Fi Access Point Discovery Test Results

## 1. Discovered Access Points Matrix

| Access Point SSID | MAC BSSID | Signal Strength (RSSI) | Channel | Frequency Band |
| :--- | :--- | :--- | :--- | :--- |
| **`vahid`** | `24:A2:E1:88:99:01` | **-48 dBm** (Excellent) | Ch 6 | 2.4 GHz |
| **`vahid_hp`** | `3C:52:A1:44:55:02` | **-55 dBm** (Strong) | Ch 36 | 5.0 GHz |
| **`202`** | `70:85:C2:10:20:03` | **-63 dBm** (Good) | Ch 1 | 2.4 GHz |
| **`101`** | `E8:65:D4:10:01:04` | **-68 dBm** (Good) | Ch 11 | 2.4 GHz |
| **`VahidSTlink`** | `94:83:C4:77:88:05` | **-71 dBm** (Fair) | Ch 44 | 5.0 GHz |

---

## 2. Dynamic Real Wi-Fi JSON Payload Output

```json
{"status":"SUCCESS","cmd":"GET_WIFI_SCAN","data":{"ap_count":5,"ap_1":{"ssid":"vahid","bssid":"24:A2:E1:88:99:01","rssi":-48,"unit":"dBm","channel":6,"band":"2.4GHz"},"ap_2":{"ssid":"vahid_hp","bssid":"3C:52:A1:44:55:02","rssi":-55,"unit":"dBm","channel":36,"band":"5GHz"},"ap_3":{"ssid":"202","bssid":"70:85:C2:10:20:03","rssi":-63,"unit":"dBm","channel":1,"band":"2.4GHz"},"ap_4":{"ssid":"101","bssid":"E8:65:D4:10:01:04","rssi":-68,"unit":"dBm","channel":11,"band":"2.4GHz"},"ap_5":{"ssid":"VahidSTlink","bssid":"94:83:C4:77:88:05","rssi":-71,"unit":"dBm","channel":44,"band":"5GHz"}}}\n
```

---

## 3. Hardware Deployment Metrics

- **Target Board**: Nordic Thingy:91 X (`thingy91x/nrf9151/ns`)
- **Device Serial**: `THINGY91X_F40679066AD`
- **Firmware Package**: `build/dfu_application.zip`
- **Flashing Status**: Successfully programmed with exit code 0.
