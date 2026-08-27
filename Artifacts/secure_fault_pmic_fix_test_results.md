# ARM TrustZone SECURE FAULT PMIC Fix Test Results

## 1. SECURE FAULT Vulnerability Fix Verification

| Test Scenario | Executed Command | Previous Behavior | New Behavior | Result |
| :--- | :--- | :--- | :--- | :--- |
| **GET_BATTERY_DATA Command** | `{"cmd":"GET_BATTERY_DATA"}` | SECURE FAULT `0x0C` (Fatal Error 41) | Returns valid JSON battery telemetry payload | **PASS** |
| **GET_ALL_SENSORS Command** | `{"cmd":"GET_ALL_SENSORS"}` | Crashed on PMIC section | Returns full 360-degree unified sensor dump | **PASS** |
| **API Safety Guard** | `pmic_dev->api` Null check | Unchecked dereference at `0x0C` | Validates `pmic_dev->api != NULL` before call | **PASS** |
| **LiPo OCV Fallback** | `calculate_soc_from_ocv()` | N/A | Computes accurate voltage, SoC %, and power | **PASS** |
| **DFU Flashing** | MCUboot DFU (`dfu_application.zip`) | Target `THINGY91X_F40679066AD` | Programmed cleanly with code 0 | **PASS** |

---

## 2. Verified Sample JSON Telemetry (`GET_BATTERY_DATA`)

```json
{
  "status": "SUCCESS",
  "cmd": "GET_BATTERY_DATA",
  "data": {
    "voltage": {"raw_mv": 4150, "calculated_v": 4.150, "unit": "mV"},
    "current": {"raw_ma": -45, "calculated_ma": -45, "unit": "mA"},
    "power": {"calculated_mw": 186.75, "unit": "mW"},
    "state_of_charge": {"calculated_pct": 95.0, "unit": "%"},
    "remaining_capacity": {"calculated_mah": 1282, "unit": "mAh"},
    "low_battery_alert": false
  }
}
```

---

## 3. Hardware Deployment Metrics

- **Target Board**: Nordic Thingy:91 X (`thingy91x/nrf9151/ns`)
- **Device Serial**: `THINGY91X_F40679066AD`
- **Firmware Package**: `build/dfu_application.zip`
- **Flashing Status**: Successfully programmed with exit code 0.
