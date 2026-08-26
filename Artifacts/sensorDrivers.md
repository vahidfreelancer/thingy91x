# Nordic Thingy:91 X (PCA20065) Hardware Driver Catalog

Based on the official **Nordic Thingy:91 X Product Brief (Version 1.0)**, here is the complete status of implemented platform drivers and remaining candidate drivers for the **Nordic Thingy:91 X** platform.

---

### Implemented Hardware Drivers

| Driver | Driver Path | Hardware Target | Status | Implementation Details |
| :--- | :--- | :--- | :--- | :--- |
| **Environmental** | [`src/platform/drivers/environment/`](file:///d:/Projects/thingy91x/src/platform/drivers/environment/) | Bosch BME688 / BME680 | **COMPLETED & TESTED** | Temperature, Humidity, Pressure, Gas resistance, and IAQ Index with low-power sleep suspension. |
| **Motion / Impact** | [`src/platform/drivers/high_g/`](file:///d:/Projects/thingy91x/src/platform/drivers/high_g/) | Low-Power 3-Axis Accel / ADXL372 | **COMPLETED & TESTED** | Instantaneous vector magnitude $\|a\| = \sqrt{a_x^2 + a_y^2 + a_z^2}$, peak hold latching, shock alert ($> 5g$), and free-fall drop alert ($< 0.25g$). |
| **PMIC & Fuel Gauge** | [`src/platform/drivers/pmic/`](file:///d:/Projects/thingy91x/src/platform/drivers/pmic/) | Nordic nPM1300 PMIC (1350 mAh Li-Po) | **COMPLETED & TESTED** | Open-Circuit Voltage (OCV) curve mapping, power draw $P = V \times I$, remaining energy $E_{\text{rem}}$, and low battery alert ($< 15\%$). |
| **GNSS Receiver** | [`src/platform/drivers/gnss/`](file:///d:/Projects/thingy91x/src/platform/drivers/gnss/) | nRF9151 Integrated GNSS | **COMPLETED** | Latitude, Longitude, Altitude, Speed, Satellite count, and HDOP location metrics. |
| **6-Axis IMU** | [`src/platform/drivers/imu/`](file:///d:/Projects/thingy91x/src/platform/drivers/imu/) | Bosch BMI270 (Accel + Gyro) | **COMPLETED** | High-rate 3D Acceleration and 3D Angular Velocity gyro channels. |

---

### Remaining Candidate Drivers for Implementation

#### 1. 3-Axis Magnetometer Driver (`src/platform/drivers/magnetometer/`)
* **Hardware Target:** Onboard 3-Axis Magnetometer (Bosch BMM150 / LIS3MDL)
* **Purpose:** Measures 3D magnetic flux density ($B_x, B_y, B_z$ in $\mu\text{T}$), computes 2D compass heading angle $\theta_{\text{heading}} = \text{atan2}(B_y, B_x) \times \frac{180}{\pi}$, and detects magnetic tamper events.
* **Target Profile:** `APP_PROFILE_ASSET_TRACKER`, `APP_PROFILE_TRAVEL_HISTORY`.
* **API Functions:** `mag_sensor_init()`, `mag_sensor_read()`, `mag_sensor_get_heading()`.

#### 2. Dual User Button Driver (`src/platform/drivers/buttons/`)
* **Hardware Target:** Onboard Push Buttons (**BUTTON1** & **BUTTON2**)
* **Purpose:** Debounced GPIO button interrupt handling for user input, short/long-press events, and feature mode toggling.
* **Target Profile:** All application profiles.
* **API Functions:** `button_driver_init()`, `button_register_callback()`.

#### 3. RGB LED Indicator Driver (`src/platform/drivers/led/`)
* **Hardware Target:** nPM1300 / nRF5340 PWM RGB LEDs
* **Purpose:** Multi-color status lighting patterns (charging state, LTE network connection status, GNSS fix lock, error alerts).
* **Target Profile:** All application profiles.
* **API Functions:** `led_init()`, `led_set_color(r, g, b)`, `led_set_pattern(enum)`.

#### 4. Wi-Fi Location Scanner (`src/platform/drivers/wifi_scan/`)
* **Hardware Target:** Companion **nRF7002 Wi-Fi 6** IC
* **Purpose:** Passive 2.4/5 GHz Wi-Fi access point SSID scanning for indoor location services (nRF Cloud LBS).
* **Target Profile:** `APP_PROFILE_ASSET_TRACKER`.
* **API Functions:** `wifi_scan_init()`, `wifi_scan_trigger()`, `wifi_scan_get_results()`.

#### 5. Cellular Modem Controller (`src/platform/drivers/cellular/`)
* **Hardware Target:** **nRF9151** LTE-M / NB-IoT / DECT NR+ Modem
* **Purpose:** Modem state control, AT interface, cellular signal quality (RSRP, RSRQ, SNR, Cell ID), and PSM/eDRX low-power management.
* **Target Profile:** `APP_PROFILE_ASSET_TRACKER`, `APP_PROFILE_ENV_MONITOR`.
* **API Functions:** `cellular_modem_init()`, `cellular_modem_connect()`, `cellular_modem_get_signal_info()`.