# Walkthrough & Test Report: High-G Impact & Motion Driver

The **High-G Impact & Motion Driver** (ADXL372 / BMA400) has been implemented with 3D acceleration vector mathematics, peak hold tracking, shock event alerts, free-fall drop detection, and compiled/flashed to the connected **Nordic Thingy:91 X** hardware.

---

## 1. Vector Mathematics & Algorithm Implementation

- **3D Acceleration Vector Magnitude**:
  Calculates instantaneous total 3D acceleration force magnitude in g:
  \[
  \|a\| = \sqrt{a_x^2 + a_y^2 + a_z^2}
  \]
- **Peak Hold Latching**:
  Latches maximum acceleration force $\|a\|_{\text{peak}}$ recorded across samples until explicitly reset via `high_g_reset_peak()`.
- **Threshold Alerting**:
  - **Shock Impact Alert**: Triggered when $\|a\| \ge \text{threshold}_g$ (default 5.0g).
  - **Free-Fall Drop Alert**: Triggered when $\|a\| \le 0.25g$ (near zero-gravity condition).

---

## 2. File & Driver Structure

- **Driver Header**: [`high_g.h`](file:///d:/Projects/thingy91x/src/platform/drivers/high_g/high_g.h)
  - Exposes `struct high_g_data`, `high_g_init()`, `high_g_read()`, `high_g_set_threshold()`, `high_g_reset_peak()`, `high_g_sleep()`.
- **Driver Logic**: [`high_g.c`](file:///d:/Projects/thingy91x/src/platform/drivers/high_g/high_g.c)
  - Devicetree compatible targets (`adi,adxl372`, `bosch,bma400`, `analog,adxl362`), Zephyr `sensor.h` channel conversion from $\text{m/s}^2$ to $g$, vector math computation, peak hold latching, low-power suspension (`PM_DEVICE_ACTION_SUSPEND`), and software simulation fallback.
- **Board Overlay**: [`thingy91x_nrf9151_ns.overlay`](file:///d:/Projects/thingy91x/boards/thingy91x_nrf9151_ns.overlay#L29-L34)
  - Configured `adxl372` devicetree node under `&i2c2`.
- **Application Profile Integration**: [`app.c`](file:///d:/Projects/thingy91x/src/app/asset_tracker/app.c#L11-L30)
  - Integrated High-G sampling into `asset_tracker` profile.

---

## 3. Firmware Build & Hardware Flashing Verification

1. **Compilation**:
   - Built for `thingy91x/nrf9151/ns` with `APP_PROFILE_ASSET_TRACKER` and `CONFIG_BOOTLOADER_MCUBOOT=y`.
   - **Flash Utilization**: 53.53% (43,580 B / 81,408 B).
   - **RAM Utilization**: 15.29% (34,856 B / 227,992 B).
2. **Device Discovery & MCUboot Programming**:
   - **Target Board**: `THINGY91X_F40679066AD` (Thingy:91 X UART, Board PCA20065).
   - **Programmed DFU Package**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via `nrfutil device program`.
