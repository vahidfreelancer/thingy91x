# Hardware Diagnostic & Remote Test Suite Profile Test Results

## 1. Test Scenarios Executed

| Scenario ID | Test Scenario | Expected Outcome | Result |
| :--- | :--- | :--- | :--- |
| **TC-HW-01** | General Purpose Firmware Profile Initialization | `app_init()` wakes up and initializes all 9 onboard hardware drivers (`BME688`, `High-G`, `nPM1300`, `RGB LED`, `Buttons`, `BMM150`, `nRF7002 Wi-Fi`, `nRF9151 Cellular`, `EKF Fusion`). | **PASS** |
| **TC-HW-02** | User Button Socket Activation | Pressing **BUTTON1** or **BUTTON2** triggers TCP socket connection request to `s4.sytemonitor.co.uk:1200` and changes LED pattern to cyan blinking. | **PASS** |
| **TC-HW-03** | `PING` Command Diagnostic | Returns board model (`Thingy:91 X PCA20065`), system uptime (`sec`), and status `SUCCESS`. | **PASS** |
| **TC-HW-04** | `GET_ENV_DATA` Command | Returns raw ADC, calculated values, and units (`degC`, `%RH`, `hPa`, `Ohm`, `IAQ_0_500`). | **PASS** |
| **TC-HW-05** | `GET_MOTION_DATA` Command | Returns raw LSB, calculated `g`, calculated `m/s2`, peak hold `g`, and shock alert booleans. | **PASS** |
| **TC-HW-06** | `GET_MAG_DATA` Command | Returns raw LSB, calculated flux `uT`, vector magnitude `uT`, heading `deg`, and tamper alert. | **PASS** |
| **TC-HW-07** | `GET_EKF_FUSION` Command | Returns 4-element quaternion ($q_0,q_1,q_2,q_3$), fused Roll (`deg`), Pitch (`deg`), Yaw (`deg`), and gyro bias (`rad/s`). | **PASS** |
| **TC-HW-08** | `GET_BATTERY_DATA` Command | Returns raw `mV`, calculated `V`, current `mA`, power `mW`, SoC `%`, and capacity `mAh`. | **PASS** |
| **TC-HW-09** | `GET_WIFI_SCAN` Command | Returns scanned AP count, 6-byte MAC BSSIDs, RSSI (`dBm`), channels (1-165), and bands (2.4/5 GHz). | **PASS** |
| **TC-HW-10** | `GET_CELLULAR_INFO` Command | Returns RSRP (`dBm`), RSRQ (`dB`), SNR (`dB`), Cell ID, Operator, Band, IP, and neighbor cell stations. | **PASS** |
| **TC-HW-11** | MCUboot DFU Deployment | Signed firmware package (`dfu_application.zip`) programs cleanly to target serial `THINGY91X_F40679066AD`. | **PASS** |

---

## 2. Sample JSON Telemetry Payloads (`s4.sytemonitor.co.uk:1200`)

### `GET_ENV_DATA` Response:
```json
{
  "status": "SUCCESS",
  "cmd": "GET_ENV_DATA",
  "data": {
    "temperature": {"raw_adc": 2450, "calculated": 24.50, "unit": "degC"},
    "humidity": {"raw_adc": 485, "calculated": 48.50, "unit": "%RH"},
    "pressure": {"raw_pa": 101325, "calculated": 1013.25, "unit": "hPa"},
    "gas_resistance": {"raw_ohm": 125000, "calculated": 125000, "unit": "Ohm"},
    "iaq_index": {"calculated": 25, "unit": "IAQ_0_500"}
  }
}
```

### `GET_EKF_FUSION` Response:
```json
{
  "status": "SUCCESS",
  "cmd": "GET_EKF_FUSION",
  "data": {
    "quaternion": {"q0": 0.9982, "q1": 0.0125, "q2": -0.0184, "q3": 0.0542},
    "fused_roll": {"calculated": 1.45, "unit": "deg"},
    "fused_pitch": {"calculated": -2.10, "unit": "deg"},
    "fused_yaw_heading": {"calculated": 182.40, "unit": "deg"},
    "gyro_bias": {"bx": 0.0001, "by": -0.0002, "bz": 0.0005, "unit": "rad/s"}
  }
}
```

### `GET_BATTERY_DATA` Response:
```json
{
  "status": "SUCCESS",
  "cmd": "GET_BATTERY_DATA",
  "data": {
    "voltage": {"raw_mv": 3950, "calculated_v": 3.950, "unit": "mV"},
    "current": {"raw_ma": -45, "calculated_ma": -45, "unit": "mA"},
    "power": {"calculated_mw": 177.75, "unit": "mW"},
    "state_of_charge": {"calculated_pct": 82.5, "unit": "%"},
    "remaining_capacity": {"calculated_mah": 1114, "unit": "mAh"},
    "low_battery_alert": false
  }
}
```

---

## 3. Hardware Deployment Metrics

- **Target Board**: Nordic Thingy:91 X (`thingy91x/nrf9151/ns`)
- **Device Serial**: `THINGY91X_F40679066AD`
- **Firmware Package**: `build/dfu_application.zip`
- **Remote Host**: `s4.sytemonitor.co.uk:1200`
