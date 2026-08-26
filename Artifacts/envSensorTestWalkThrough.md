# Walkthrough & Hardware Test Report: Environmental Sensor Driver

The **Environmental Sensor Driver** (BME688 / SHTC3 / BME680) has been implemented, compiled, backed up, and successfully programmed to the physical **Nordic Thingy:91 X** hardware.

---

## 1. MCUboot DFU & Factory Firmware Backup

1. **MCUboot DFU Configuration**:
   - Enabled `CONFIG_BOOTLOADER_MCUBOOT=y` in [prj.conf](file:///d:/Projects/thingy91x/prj.conf#L10-L12).
   - Generates MCUboot partition header boundaries and produces signed DFU packages ([`dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip)) for internal USB/Serial bootloader updates.
2. **Factory Firmware Backup**:
   - Backed up all original stock/factory precompiled application packages to [`backup/factory_firmware_backup/`](file:///d:/Projects/thingy91x/backup/factory_firmware_backup/).

---

## 2. Driver Code & Subsystem Implementation

- **Driver Interface**: [`environment.h`](file:///d:/Projects/thingy91x/src/platform/drivers/environment/environment.h)
- **Driver Logic**: [`environment.c`](file:///d:/Projects/thingy91x/src/platform/drivers/environment/environment.c)
  - Interacts with Zephyr `sensor.h` channels (`SENSOR_CHAN_AMBIENT_TEMP`, `SENSOR_CHAN_HUMIDITY`, `SENSOR_CHAN_PRESS`, `SENSOR_CHAN_GAS_RES`).
  - Supports `bosch,bme680`, `bosch,bme688`, `sensirion,shtc3`, `bosch,bmp388` Devicetree targets.
  - Implements low-power device suspension (`PM_DEVICE_ACTION_SUSPEND`).
  - Implements realistic software simulation fallback for headless / off-target validation.
- **Device Tree Overlay**: [`thingy91x_nrf9151_ns.overlay`](file:///d:/Projects/thingy91x/boards/thingy91x_nrf9151_ns.overlay#L20-L31)
  - Configured `bme680` devicetree node under `&i2c2` bus.
- **Application Profile Integration**: [`app.c`](file:///d:/Projects/thingy91x/src/app/env_monitor/app.c#L35-L42)
  - Configured periodic sampling loop in `env_monitor` application.

---

## 3. Firmware Build & Hardware Flashing Verification

1. **Compilation**:
   - Built using nRF Connect SDK toolchain **v3.2.1** for target `thingy91x/nrf9151/ns`.
   - **Flash Utilization**: 53.53% (43,580 B / 81,408 B).
   - **RAM Utilization**: 15.29% (34,856 B / 227,992 B).
2. **Device Discovery & MCUboot Programming**:
   - **Connected Device**: `THINGY91X_F40679066AD` (Thingy:91 X UART, Board PCA20065).
   - **Programmed DFU Package**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via `nrfutil device program`.
