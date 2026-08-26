Listed directory thingy91x
Listed directory src
Listed directory platform
Listed directory drivers
Listed directory boards
Viewed thingy91x_nrf9151_ns.overlay:1-32
Viewed prj.conf:1-26
Viewed Kconfig:1-63
Searched for ".*"
Listed directory app
Listed directory gnss
Listed directory imu
Listed directory services
Listed directory Doc
Viewed ProjectArchitecture.md:1-52
Viewed GEMINI.md:1-112
Viewed gnss.h:1-44
Viewed imu.h:1-29
Listed directory storage

Based on a review of the repository architecture, [GEMINI.md](file:///d:/Projects/thingy91x/GEMINI.md), and existing drivers in [`src/platform/drivers/`](file:///d:/Projects/thingy91x/src/platform/drivers/), here is the current state of the platform drivers and a comprehensive breakdown of missing drivers that can be implemented for the **Nordic Thingy:91 X** platform.

---

### Current Driver Status

Currently, the project only contains initial stub implementations for two hardware drivers under [`src/platform/drivers/`](file:///d:/Projects/thingy91x/src/platform/drivers/):
1. **GNSS Driver** ([`gnss.h`](file:///d:/Projects/thingy91x/src/platform/drivers/gnss/gnss.h), [`gnss.c`](file:///d:/Projects/thingy91x/src/platform/drivers/gnss/gnss.c)): Interfaces with the nRF9151 integrated GNSS receiver for location fixes (lat/lon, speed, altitude, HDOP).
2. **IMU Driver** ([`imu.h`](file:///d:/Projects/thingy91x/src/platform/drivers/imu/imu.h), [`imu.c`](file:///d:/Projects/thingy91x/src/platform/drivers/imu/imu.c)): Abstraction for 6-axis motion sensing (accelerometer + gyroscope).

---

### Recommended Drivers for Implementation

To fully leverage the **Thingy:91 X multi-SoC hardware** (nRF9151 + nRF5340 + nRF7002) and support all application profiles defined in [`Kconfig`](file:///d:/Projects/thingy91x/Kconfig), the following drivers can be added to `src/platform/drivers/`:

#### 1. Environmental Sensors (`src/platform/drivers/environment/`)
* **Hardware Target:** Bosch **BME688** / **BMP390** / **SHTC3** (Environmental & Gas Sensors)
* **Purpose:** Reads ambient temperature, humidity, atmospheric pressure, air quality index (IAQ), and volatile organic compounds (VOCs).
* **Target Profile:** `APP_PROFILE_ENV_MONITOR`, `APP_PROFILE_ASSET_TRACKER`.
* **API Functions:** `env_sensor_init()`, `env_sensor_read(struct env_data *data)`.

#### 2. High-G Shock / Impact Sensor (`src/platform/drivers/high_g/`)
* **Hardware Target:** Analog Devices **ADXL372** or Bosch **BMA400** (High-G / Low-Power Accel)
* **Purpose:** Detects severe drops, crash impacts, or high acceleration shock events without keeping the high-power IMU running continuously. Supports wake-on-motion threshold interrupts.
* **Target Profile:** `APP_PROFILE_ASSET_TRACKER`, `APP_PROFILE_TRAVEL_HISTORY`.
* **API Functions:** `high_g_init()`, `high_g_read_peak()`, `high_g_set_trigger_threshold()`.

#### 3. Power Management & Fuel Gauge (`src/platform/drivers/pmic/` or `battery/`)
* **Hardware Target:** Nordic **nPM1300** PMIC / ADI **MAX17048** / ADP536X
* **Purpose:** Monitors battery State of Charge (SoC %), cell voltage, charge current, power source status (USB vs. Battery), and manages soft power rail shutdown (`CONFIG_PM_DEVICE`).
* **Target Profile:** All application profiles (critical for power budget management).
* **API Functions:** `battery_init()`, `battery_get_soc()`, `battery_get_voltage()`, `pmic_set_power_state()`.

#### 4. Ambient Light & Color Sensor (`src/platform/drivers/color/` or `ambient_light/`)
* **Hardware Target:** AMS **BH1749NVI** or OSRAM RGB/Light Sensor
* **Purpose:** Measures ambient illuminance (lux) and RGB color channels. Useful for optical box-open / tamper detection in asset tracking or ambient light sensing.
* **Target Profile:** `APP_PROFILE_ASSET_TRACKER`, `APP_PROFILE_ENV_MONITOR`.
* **API Functions:** `light_sensor_init()`, `light_sensor_read_lux()`, `light_sensor_read_rgb()`.

#### 5. Wi-Fi Location Scanner (`src/platform/drivers/wifi_scan/`)
* **Hardware Target:** Companion **nRF7002** Wi-Fi IC
* **Purpose:** Performs passive 2.4 GHz / 5 GHz Wi-Fi SSID access point scanning to feed raw BSSID/RSSI data into cloud-based location services (e.g. nRF Cloud Wi-Fi LBS) when indoor GNSS signal is lost.
* **Target Profile:** `APP_PROFILE_ASSET_TRACKER`.
* **API Functions:** `wifi_scan_init()`, `wifi_scan_trigger()`, `wifi_scan_get_results()`.

#### 6. Cellular Modem Controller (`src/platform/drivers/cellular/`)
* **Hardware Target:** **nRF9151** LTE-M / NB-IoT / DECT NR+ Modem
* **Purpose:** Encapsulates nRF Modem library calls for cellular connection management, network registration, signal quality reporting (RSRP, RSRQ, SNR, Cell ID), and flight mode control.
* **Target Profile:** `APP_PROFILE_ASSET_TRACKER`, `APP_PROFILE_ENV_MONITOR`.
* **API Functions:** `cellular_modem_init()`, `cellular_modem_connect()`, `cellular_modem_get_signal_info()`.

#### 7. RGB LED & User Indicator Driver (`src/platform/drivers/led/`)
* **Hardware Target:** Onboard PWM / PMIC RGB LED controller
* **Purpose:** Controls multi-color visual feedback patterns (system state, charging status, network connection status, error alerts).
* **Target Profile:** All application profiles (User experience & diagnostics).
* **API Functions:** `led_init()`, `led_set_color(r, g, b)`, `led_set_pattern(pattern_enum)`.

#### 8. User Button & Tamper Switch Driver (`src/platform/drivers/button/`)
* **Hardware Target:** Push buttons / mechanical enclosure switches
* **Purpose:** Handles debounced button presses, long-press events, and enclosure tamper detection triggers via Zephyr GPIO interrupts.
* **Target Profile:** All application profiles.
* **API Functions:** `button_init()`, `button_register_callback()`.

---

### Summary Table

| Proposed Driver | Driver Path | Hardware Chip | Key Use Case |
| :--- | :--- | :--- | :--- |
| **Environmental** | `src/platform/drivers/environment/` | BME688 / SHTC3 | Temp, Humidity, Pressure, Gas/IAQ |
| **High-G Impact** | `src/platform/drivers/high_g/` | ADXL372 / BMA400 | Shock/Drop detection & Wake-on-motion |
| **PMIC / Battery** | `src/platform/drivers/pmic/` | nPM1300 / MAX17048 | Battery SoC %, Voltage & Power rail management |
| **Ambient Light** | `src/platform/drivers/ambient_light/` | BH1749NVI | Lux, RGB color, Enclosure tamper detection |
| **Wi-Fi Scanner** | `src/platform/drivers/wifi_scan/` | nRF7002 | Wi-Fi BSSID scan for indoor location |
| **Cellular Modem**| `src/platform/drivers/cellular/` | nRF9151 LTE modem | Signal stats, LTE-M/NB-IoT state management |
| **RGB Indicator** | `src/platform/drivers/led/` | PMIC PWM / GPIO RGB | Status & alert lighting patterns |
| **Button / Tamper**| `src/platform/drivers/button/` | GPIO Push Buttons | User inputs & enclosure security switch |

---

### Next Steps

No code changes have been made yet. When you are ready to proceed with implementing any of these drivers:
1. We can select which specific driver(s) you would like to implement first.
2. We will create matching Device Tree overlays in [`boards/thingy91x_nrf9151_ns.overlay`](file:///d:/Projects/thingy91x/boards/thingy91x_nrf9151_ns.overlay) and corresponding options in [`Kconfig`](file:///d:/Projects/thingy91x/Kconfig).