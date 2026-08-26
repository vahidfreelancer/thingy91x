# Implementation Plan: Step 3 - 3-Axis Magnetometer Driver

Implement a high-level 3-Axis Magnetometer Driver (`src/platform/drivers/magnetometer/`) for the Nordic Thingy:91 X platform to measure 3D magnetic flux density ($B_x, B_y, B_z$ in $\mu\text{T}$), calculate 3D field vector magnitude ($\|B\| = \sqrt{B_x^2 + B_y^2 + B_z^2}$), compute 2D compass heading angle ($\theta_{\text{heading}} = \text{atan2}(B_y, B_x) \times \frac{180}{\pi}$), and detect magnetic enclosure tamper alerts.

## Proposed Changes

### Hardware Abstraction Layer (HAL) - Drivers & Math

#### [NEW] [magnetometer.h](file:///d:/Projects/thingy91x/src/platform/drivers/magnetometer/magnetometer.h)
- Declare `struct mag_sensor_data` containing:
  - `mag_x_ut`, `mag_y_ut`, `mag_z_ut`: Magnetic flux density in microteslas ($\mu\text{T}$)
  - `magnitude_ut`: Total 3D vector magnitude $\|B\| = \sqrt{B_x^2 + B_y^2 + B_z^2}$ ($\mu\text{T}$)
  - `heading_deg`: 2D compass heading angle $\theta \in [0^\circ, 360^\circ)$
  - `tamper_detected`: True if magnetic field magnitude exceeds proximity threshold ($> 250\,\mu\text{T}$)
  - `valid`: True if sensor sample read successfully
- Expose interface functions:
  - `int mag_sensor_init(void)`: Query Devicetree for compatible magnetometer nodes (`bosch,bmm150`, `st,lis3mdl`) and check readiness.
  - `int mag_sensor_read(struct mag_sensor_data *data)`: Fetch sample channels (`SENSOR_CHAN_MAGN_XYZ`), calculate magnitude and compass heading, evaluate tamper flag.
  - `int mag_sensor_sleep(void)`: Put sensor into low-power suspended state (`CONFIG_PM_DEVICE`).

#### [NEW] [magnetometer.c](file:///d:/Projects/thingy91x/src/platform/drivers/magnetometer/magnetometer.c)
- Implement driver logic using Zephyr `sensor.h`, Devicetree macros (`DT_HAS_COMPAT_STATUS_OKAY`), trigonometric heading math (`atan2f`), Gauss-to-$\mu\text{T}$ conversion, POSIX error return codes, and software simulation fallback for off-target validation.

---

### Devicetree & Board Overlay

#### [MODIFY] [thingy91x_nrf9151_ns.overlay](file:///d:/Projects/thingy91x/boards/thingy91x_nrf9151_ns.overlay)
- Configure `bmm150` magnetometer node under `&i2c2` bus in board overlay.

---

### Application Integration & Build System

#### [MODIFY] [CMakeLists.txt](file:///d:/Projects/thingy91x/src/app/asset_tracker/CMakeLists.txt)
- Include `../../platform/drivers/magnetometer/magnetometer.c` and `../../platform/drivers/magnetometer` in `asset_tracker/CMakeLists.txt`.

#### [MODIFY] [app.c](file:///d:/Projects/thingy91x/src/app/asset_tracker/app.c)
- Call `mag_sensor_init()` during boot, sample 3D magnetic flux density and compass heading inside application sampling loop.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/magnetometer_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/magnetometer_implementation_plan.md)
#### [NEW] [Artifacts/magnetometer_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/magnetometer_walkthrough.md)
#### [NEW] [Artifacts/magnetometer_test_results.md](file:///d:/Projects/thingy91x/Artifacts/magnetometer_test_results.md)
- Save design plans, execution walkthrough, and hardware test scenario results in `Artifacts/`.

---

## Verification Plan

### Automated / Build Verification
- Compile firmware using nRF Connect SDK toolchain (`west build -b thingy91x/nrf9151/ns -- -DCONFIG_APP_PROFILE_ASSET_TRACKER=y`) with `CONFIG_BOOTLOADER_MCUBOOT=y`.

### Hardware Flashing & Live Testing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Monitor live serial console logs to verify 3D magnetic flux values ($B_x, B_y, B_z$), magnitude $\|B\|$, and 2D compass heading calculations.
