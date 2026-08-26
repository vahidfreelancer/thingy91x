# Implementation Plan: Thingy:91 X Hardware Driver Corrections & Magnetometer Implementation

Correct all hardware specifications to strictly reflect the **Nordic Thingy:91 X (PCA20065)** hardware architecture, implement the missing **3-Axis Magnetometer Driver**, update PMIC battery capacity to **1350 mAh**, build, flash to hardware, and save all test artifacts under `Artifacts/`.

## Proposed Changes

### 1. PMIC Battery Capacity Correction (`src/platform/drivers/pmic/`)

#### [MODIFY] [pmic.c](file:///d:/Projects/thingy91x/src/platform/drivers/pmic/pmic.c)
- Update `NOMINAL_BATTERY_CAPACITY_MAH` from `1400` to `1350` mAh matching the official Thingy:91 X Li-Po battery.
- Update energy calculation formula $E_{\text{rem\_mAh}} = 1350 \times \left(\frac{\text{SoC}}{100}\right)$.

---

### 2. 3-Axis Magnetometer Driver Implementation (`src/platform/drivers/magnetometer/`)

#### [NEW] [magnetometer.h](file:///d:/Projects/thingy91x/src/platform/drivers/magnetometer/magnetometer.h)
- Declare `struct mag_sensor_data` containing:
  - `mag_x_ut`, `mag_y_ut`, `mag_z_ut`: Magnetic flux density components in microteslas ($\mu\text{T}$).
  - `magnitude_ut`: Total 3D magnetic field vector magnitude $\|B\| = \sqrt{B_x^2 + B_y^2 + B_z^2}$.
  - `heading_deg`: 2D compass heading angle $\theta = \text{atan2}(B_y, B_x) \times \frac{180}{\pi}$ (0° - 360°).
  - `valid`: True if sensor sample read successfully.
- Expose driver interface functions: `mag_sensor_init()`, `mag_sensor_read()`, `mag_sensor_sleep()`.

#### [NEW] [magnetometer.c](file:///d:/Projects/thingy91x/src/platform/drivers/magnetometer/magnetometer.c)
- Implement driver logic using Zephyr `sensor.h` channels (`SENSOR_CHAN_MAGN_XYZ`), Devicetree compatible checks (`bosch,bmm150`, `st,lis3mdl`), 2D/3D magnetic vector math, POSIX error return codes, and software simulation fallback.

---

### 3. Devicetree & Board Overlay

#### [MODIFY] [thingy91x_nrf9151_ns.overlay](file:///d:/Projects/thingy91x/boards/thingy91x_nrf9151_ns.overlay)
- Add magnetometer node (`bmm150`) under `&i2c2` bus.

---

### 4. Application Integration & Build System

#### [MODIFY] [CMakeLists.txt](file:///d:/Projects/thingy91x/src/app/asset_tracker/CMakeLists.txt)
- Include `../../platform/drivers/magnetometer/magnetometer.c` and include directory.

#### [MODIFY] [app.c](file:///d:/Projects/thingy91x/src/app/asset_tracker/app.c)
- Call `mag_sensor_init()` in `app_init()` and sample 3D magnetic metrics ($B_x, B_y, B_z$) and compass heading ($\theta$) inside `telemetry_work_handler()`.

---

### 5. Corrected Driver Catalog & Documentation (`Artifacts/`)

#### [MODIFY] [Artifacts/sensorDrivers.md](file:///d:/Projects/thingy91x/Artifacts/sensorDrivers.md)
- Update driver specification catalog to accurately document all 9 drivers on the **Nordic Thingy:91 X (PCA20065)**.

#### [NEW] [Artifacts/magnetometer_test_results.md](file:///d:/Projects/thingy91x/Artifacts/magnetometer_test_results.md)
- Store test scenarios, mathematical formulas, and test execution results for the Magnetometer driver.

#### [MODIFY] [Artifacts/walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/walkthrough.md)
- Update complete walkthrough of all hardware corrections and driver test results.

---

## Verification Plan

### Automated / Build Verification
- Compile firmware using nRF Connect SDK v3.2.1 (`west build -b thingy91x/nrf9151/ns -- -DCONFIG_APP_PROFILE_ASSET_TRACKER=y`) with `CONFIG_BOOTLOADER_MCUBOOT=y`.

### Hardware Flashing & Live Testing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Verify live serial output logs for 1350 mAh battery metrics and 3D compass heading calculations.
