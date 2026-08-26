# Walkthrough: Step 3 - 3-Axis Magnetometer Driver

The **3-Axis Magnetometer Driver** (Bosch BMM150 / LIS3MDL) has been implemented with 3D magnetic flux density sampling ($B_x, B_y, B_z$ in $\mu\text{T}$), vector magnitude calculation ($\|B\|$), 2D compass heading mathematics ($\theta \in [0^\circ, 360^\circ)$), magnetic tamper alert detection, compiled, and programmed to the connected **Nordic Thingy:91 X** hardware.

---

## 1. Implemented Features & Driver HAL

- **Driver Header**: [`magnetometer.h`](file:///d:/Projects/thingy91x/src/platform/drivers/magnetometer/magnetometer.h)
  - Exposes `struct mag_sensor_data`, `mag_sensor_init()`, `mag_sensor_read()`, `mag_sensor_sleep()`.
- **Driver Logic**: [`magnetometer.c`](file:///d:/Projects/thingy91x/src/platform/drivers/magnetometer/magnetometer.c)
  - Computes 3D magnetic flux density, field vector magnitude $\|B\| = \sqrt{B_x^2 + B_y^2 + B_z^2}$, 2D compass heading angle $\theta = \text{atan2}(B_y, B_x) \times \frac{180}{\pi}$, and evaluates magnetic proximity tamper threshold ($> 250\,\mu\text{T}$).
- **Application Integration**: [`app.c`](file:///d:/Projects/thingy91x/src/app/asset_tracker/app.c#L35-L51)
  - `app_init()` initializes magnetometer sensor and samples 3D magnetic metrics and compass heading periodically inside `telemetry_work_handler()`.

---

## 2. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Flashed**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/magnetometer_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/magnetometer_test_results.md)
