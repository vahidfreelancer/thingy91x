# Implementation Plan: BME688 / SHTC3 Environmental Sensor Driver

Implement a modular, production-ready Environmental Sensor Driver (`src/platform/drivers/environment/`) for the Nordic Thingy:91 X platform, adhering to nRF Connect SDK / Zephyr RTOS coding standards, hardware abstraction policies, and power-management rules defined in [GEMINI.md](file:///d:/Projects/thingy91x/GEMINI.md).

## Proposed Changes

### Hardware Abstraction Layer (HAL) - Drivers

#### [NEW] [environment.h](file:///d:/Projects/thingy91x/src/platform/drivers/environment/environment.h)
- Declare `struct env_sensor_data` with fields for `temperature` (°C), `humidity` (% RH), `pressure` (hPa), `gas_resistance` (Ohms / VOC), `iaq_index` (Air Quality Index), and `valid` flag.
- Expose driver interface functions:
  - `int env_sensor_init(void)`: Initialize hardware sensor instance or register fallback simulation.
  - `int env_sensor_read(struct env_sensor_data *data)`: Fetch sample and extract channels (`SENSOR_CHAN_AMBIENT_TEMP`, `SENSOR_CHAN_HUMIDITY`, `SENSOR_CHAN_PRESS`, `SENSOR_CHAN_GAS_RES`).
  - `int env_sensor_sleep(void)`: Put sensor / bus interface into low-power suspended state via Zephyr Power Management (`CONFIG_PM_DEVICE`).

#### [NEW] [environment.c](file:///d:/Projects/thingy91x/src/platform/drivers/environment/environment.c)
- Implement `env_sensor_init()`, `env_sensor_read()`, and `env_sensor_sleep()`.
- Use Zephyr `sensor.h` APIs and Devicetree macros (`DT_HAS_COMPAT_STATUS_OKAY`, `DEVICE_DT_GET_ANY`) for `bosch,bme680`, `bosch,bme688`, and `sensirion,shtc3`.
- Include safe fallbacks to software simulation when physical hardware is not detected (for QEMU/host testing).
- Follow POSIX error code conventions (`0` on success, negative POSIX errors like `-ENODEV`, `-EINVAL`, `-EIO` on failure) and logging via `LOG_MODULE_REGISTER(env_driver)`.

---

### Board & Device Tree Configuration

#### [MODIFY] [thingy91x_nrf9151_ns.overlay](file:///d:/Projects/thingy91x/boards/thingy91x_nrf9151_ns.overlay)
- Configure `&i2c2` node with fast-mode bitrate and add BME688 / SHTC3 sensor definitions on the I2C bus.

#### [MODIFY] [prj.conf](file:///d:/Projects/thingy91x/prj.conf)
- Ensure `CONFIG_I2C=y`, `CONFIG_SENSOR=y`, and `CONFIG_BME680=y` are enabled in project configuration.

---

### Build System & Application Profile

#### [MODIFY] [CMakeLists.txt](file:///d:/Projects/thingy91x/src/app/env_monitor/CMakeLists.txt)
- Add `../../platform/drivers/environment/environment.c` to `target_sources` and add `../../platform/drivers/environment` to `target_include_directories`.

#### [MODIFY] [app.c](file:///d:/Projects/thingy91x/src/app/env_monitor/app.c)
- Integrate `env_sensor_init()` into `app_init()` and call `env_sensor_read()` inside `sensor_sample_handler()` to log temperature, humidity, pressure, and gas measurements.

---

## Verification Plan

### Automated / Build Verification
- Compile check / static syntax validation of new header and source files against Zephyr C11 standards.

### Manual / System Verification
- Verify that `env_monitor` profile initializes the driver cleanly.
- Verify fallback simulation provides realistic sensor readings when target hardware is unattached.
