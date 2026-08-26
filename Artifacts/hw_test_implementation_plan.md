# Implementation Plan: Hardware Diagnostic & Test Suite Profile (`APP_PROFILE_HW_TEST`)

Implement a dedicated **Hardware Diagnostic & Test Suite Profile** (`src/app/hw_test/`) for the Nordic Thingy:91 X platform.
Upon pressing **BUTTON1** or **BUTTON2**, the device opens a TCP socket connection to server **`s4.sytemonitor.co.uk:1200`**, listens for incoming JSON commands, and responds with detailed telemetry containing **both raw and calculated values** along with **measurement units** for all onboard hardware.

---

## 1. Supported Command Registry Specification (`s4.sytemonitor.co.uk:1200`)

| JSON Command (`"cmd"`) | Hardware Target | Reported Metrics (Raw + Calculated + Units) |
| :--- | :--- | :--- |
| `PING` | System Core | Board ID (`Thingy:91 X PCA20065`), Uptime (`sec`), Connection state |
| `GET_SYSTEM_INFO` | Multi-SoC Architecture | nRF9151, nRF5340, nRF7002, nPM1300, MCUboot DFU flag status |
| `GET_ENV_DATA` | Bosch BME688 | Temp (raw/`degC`), Humidity (raw/`%RH`), Pressure (raw `Pa`/calc `hPa`), Gas (raw `Ohm`/calc `IAQ 0-500`) |
| `GET_MOTION_DATA` | Low-Power Accel / ADXL372 | Accel 3D (raw LSB / calc `g`, `m/s2`), Peak hold (`g`), Shock & Drop alerts |
| `GET_MAG_DATA` | Bosch BMM150 | Mag 3D (raw LSB / calc `uT`), Vector Magnitude (`uT`), Heading (`deg`), Tamper alert |
| `GET_EKF_FUSION` | 9-DOF AHRS Service | Quaternion ($q_0,q_1,q_2,q_3$), Fused Roll (`deg`), Pitch (`deg`), Yaw (`deg`), Gyro bias (`rad/s`) |
| `GET_BATTERY_DATA` | Nordic nPM1300 PMIC | Voltage (raw `mV`/calc `V`), Current (`mA`), Power (`mW`), SoC (`%`), Capacity (`mAh`), Status |
| `GET_WIFI_SCAN` | Companion nRF7002 Wi-Fi 6 | Scanned AP count, BSSIDs, RSSI (`dBm`), Channels (1-165), Bands (2.4/5 GHz) |
| `GET_CELLULAR_INFO`| nRF9151 LTE-M Modem | RSRP (`dBm`), RSRQ (`dB`), SNR (`dB`), Cell ID, Operator, Band, IP, Neighboring Cell Stations |
| `SET_LED_PATTERN` | RGB LED Controller | Set color ($R,G,B \in [0,255]$) and animation pattern (`BREATHE`, `BLINK_FAST`, `SOLID`, `OFF`) |
| `GET_ALL_SENSORS` | Complete Telemetry | Full 360-degree unified JSON telemetry payload combining all 9 hardware subsystems |

---

## Proposed Changes

### Application Layer (`src/app/hw_test/`)

#### [NEW] [app.c](file:///d:/Projects/thingy91x/src/app/hw_test/app.c)
- Implement `hw_test` application module:
  - Initializes all 9 hardware drivers (`environment`, `high_g`, `pmic`, `led`, `buttons`, `magnetometer`, `wifi_scan`, `cellular`, `ekf_fusion`).
  - Registers button press interrupt callback to open TCP socket connection to **`s4.sytemonitor.co.uk`** on port **`1200`**.
  - Implements socket RX command listener loop, JSON command parser, and response serializer.

#### [NEW] [app_config.h](file:///d:/Projects/thingy91x/src/app/hw_test/app_config.h)
- Define `SERVER_HOST "s4.sytemonitor.co.uk"`, `SERVER_PORT 1200`, buffer sizes, and profile configurations.

#### [NEW] [CMakeLists.txt](file:///d:/Projects/thingy91x/src/app/hw_test/CMakeLists.txt)
- Include application sources and all 9 HAL driver paths.

---

### Root Configuration & Kconfig

#### [MODIFY] [Kconfig](file:///d:/Projects/thingy91x/Kconfig)
- Add Kconfig profile choice `CONFIG_APP_PROFILE_HW_TEST` ("Hardware Diagnostic & Remote Test Suite Profile").

#### [MODIFY] [CMakeLists.txt](file:///d:/Projects/thingy91x/CMakeLists.txt)
- Conditionally include `src/app/hw_test` when `CONFIG_APP_PROFILE_HW_TEST=y`.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/hw_test_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/hw_test_implementation_plan.md)
#### [NEW] [Artifacts/hw_test_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/hw_test_walkthrough.md)
#### [NEW] [Artifacts/hw_test_results.md](file:///d:/Projects/thingy91x/Artifacts/hw_test_results.md)
- Save design plans, execution walkthrough, JSON API schemas, and hardware test scenario results in `Artifacts/`.

---

## Verification Plan

### Automated / Build Verification
- Compile hardware test application using nRF Connect SDK toolchain (`west build -b thingy91x/nrf9151/ns -- -DCONFIG_APP_PROFILE_HW_TEST=y`) with `CONFIG_BOOTLOADER_MCUBOOT=y`.

### Hardware Flashing & Live Testing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Press **BUTTON1** or **BUTTON2** on the physical board to trigger connection to `s4.sytemonitor.co.uk:1200`.
- Monitor live serial console logs to verify TCP socket creation, JSON command parsing, and response serialization.
