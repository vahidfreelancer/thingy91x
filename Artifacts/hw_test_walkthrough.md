# Walkthrough: Hardware Diagnostic & Remote Test Suite Profile (`APP_PROFILE_HW_TEST`)

The **Hardware Diagnostic & Remote Test Suite Profile** (`APP_PROFILE_HW_TEST`) has been implemented as a modular application profile under `src/app/hw_test/`. When the user pushes **BUTTON1** or **BUTTON2**, the device connects to remote server **`s4.sytemonitor.co.uk`** on port **`1200`** via a TCP socket, listens for incoming JSON diagnostic test commands, and responds with structured telemetry containing **both raw and calculated values** along with **measurement units** for all onboard hardware.

---

## 1. Supported Command Registry Specification (`s4.sytemonitor.co.uk:1200`)

| JSON Command (`"cmd"`) | Target Hardware Subsystem | Reported Metrics (Raw + Calculated + Units) |
| :--- | :--- | :--- |
| `PING` | System Core | Board ID (`Thingy:91 X PCA20065`), Uptime (`sec`), Status |
| `GET_SYSTEM_INFO` | Multi-SoC Architecture | nRF9151, nRF5340, nRF7002, nPM1300, MCUboot DFU |
| `GET_ENV_DATA` | Bosch BME688 | Temp (raw/`degC`), Humidity (raw/`%RH`), Pressure (raw `Pa`/calc `hPa`), Gas (raw `Ohm`/calc `IAQ 0-500`) |
| `GET_MOTION_DATA` | High-G Accel / IMU | Accel 3D (raw LSB / calc `g`, `m/s2`), Peak hold (`g`), Impact & Freefall alerts |
| `GET_MAG_DATA` | Bosch BMM150 | Mag 3D (raw LSB / calc `uT`), Magnitude (`uT`), Heading (`deg`), Tamper alert |
| `GET_EKF_FUSION` | EKF 9-DOF AHRS Service | Quaternion ($q_0,q_1,q_2,q_3$), Fused Roll (`deg`), Pitch (`deg`), Yaw (`deg`), Gyro bias (`rad/s`) |
| `GET_BATTERY_DATA` | Nordic nPM1300 PMIC | Voltage (raw `mV`/calc `V`), Current (`mA`), Power (`mW`), SoC (`%`), Capacity (`mAh`), Low alert |
| `GET_WIFI_SCAN` | nRF7002 Wi-Fi 6 | Scanned AP count, BSSID MACs, RSSI (`dBm`), Channels (1-165), Bands (2.4/5 GHz) |
| `GET_CELLULAR_INFO`| nRF9151 LTE-M Modem | RSRP (`dBm`), RSRQ (`dB`), SNR (`dB`), Cell ID, Operator, Band, IP, Neighboring Cell Stations |
| `SET_LED_PATTERN` | RGB LED Controller | Set color ($R,G,B \in [0,255]$) and pattern (`SOLID`, `BLINK_SLOW`, `BLINK_FAST`, `BREATHE`, `OFF`) |
| `GET_ALL_SENSORS` | Complete Telemetry | Full 360-degree unified JSON payload combining all 9 hardware subsystems |

---

## 2. Implemented Application Profile Code

- **App Implementation**: [`app.c`](file:///d:/Projects/thingy91x/src/app/hw_test/app.c)
- **App Header**: [`app.h`](file:///d:/Projects/thingy91x/src/app/hw_test/app.h)
- **App Configuration**: [`app_config.h`](file:///d:/Projects/thingy91x/src/app/hw_test/app_config.h)
- **CMake Build Config**: [`CMakeLists.txt`](file:///d:/Projects/thingy91x/src/app/hw_test/CMakeLists.txt)
- **Profile Selector**: [`Kconfig`](file:///d:/Projects/thingy91x/Kconfig#L41-L45)

---

## 3. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Flashed**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/hw_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/hw_test_results.md)
