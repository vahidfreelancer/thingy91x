# Implementation Plan: Step 1 - RGB LED Visual Indicator Driver

Implement a high-level RGB LED Driver (`src/platform/drivers/led/`) for the Nordic Thingy:91 X platform to control multi-color visual status indication, RGB color blending ($R, G, B \in [0, 255]$), animated breathing/blinking patterns, system status indicators, error alerts, and power saving yielding.

## Proposed Changes

### Hardware Abstraction Layer (HAL) - Drivers

#### [NEW] [led.h](file:///d:/Projects/thingy91x/src/platform/drivers/led/led.h)
- Declare `enum led_pattern`:
  - `LED_PATTERN_OFF`: LED completely turned off
  - `LED_PATTERN_SOLID`: Continuous solid color
  - `LED_PATTERN_BLINK_SLOW`: 1 Hz slow blinking
  - `LED_PATTERN_BLINK_FAST`: 4 Hz fast flashing (alert)
  - `LED_PATTERN_BREATHE`: Smooth sinusoidal brightness fading
  - `LED_PATTERN_BOOT`: Power-on initialization sequence
  - `LED_PATTERN_ERROR`: Red error strobe
- Declare `struct led_color` with `r`, `g`, `b` byte channels.
- Expose interface functions:
  - `int led_driver_init(void)`: Query Devicetree for PWM/GPIO/nPM1300 RGB LED nodes and check readiness.
  - `int led_set_color(uint8_t red, uint8_t green, uint8_t blue)`: Set static RGB color values.
  - `int led_set_pattern(enum led_pattern pattern, uint8_t red, uint8_t green, uint8_t blue)`: Trigger animated visual status pattern.
  - `int led_update(void)`: Update pattern state step (called periodically in workqueue).
  - `int led_off(void)`: Turn off all RGB LED channels.

#### [NEW] [led.c](file:///d:/Projects/thingy91x/src/platform/drivers/led/led.c)
- Implement driver logic using Zephyr `led.h` / `pwm.h` / `gpio.h` / `sensor.h`, Devicetree macros (`DT_HAS_COMPAT_STATUS_OKAY`), brightness scaling mathematics, sinusoidal breathing calculations, POSIX error return codes, and software simulation fallback for off-target validation.

---

### Devicetree & Board Overlay

#### [MODIFY] [thingy91x_nrf9151_ns.overlay](file:///d:/Projects/thingy91x/boards/thingy91x_nrf9151_ns.overlay)
- Configure `leds` / `pwm_leds` node definitions for Red, Green, Blue LED channels in board overlay.

---

### Application Integration & Build System

#### [MODIFY] [CMakeLists.txt](file:///d:/Projects/thingy91x/src/app/asset_tracker/CMakeLists.txt)
- Include `../../platform/drivers/led/led.c` and `../../platform/drivers/led` in `asset_tracker/CMakeLists.txt`.

#### [MODIFY] [app.c](file:///d:/Projects/thingy91x/src/app/asset_tracker/app.c)
- Call `led_driver_init()` during boot, display `LED_PATTERN_BOOT` pulse, and update LED pattern state inside application sampling loop.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/rgb_led_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/rgb_led_implementation_plan.md)
#### [NEW] [Artifacts/rgb_led_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/rgb_led_walkthrough.md)
#### [NEW] [Artifacts/rgb_led_test_results.md](file:///d:/Projects/thingy91x/Artifacts/rgb_led_test_results.md)
- Save design plans, execution walkthrough, and hardware test scenario results in `Artifacts/`.

---

## Verification Plan

### Automated / Build Verification
- Compile firmware using nRF Connect SDK toolchain (`west build -b thingy91x/nrf9151/ns -- -DCONFIG_APP_PROFILE_ASSET_TRACKER=y`) with `CONFIG_BOOTLOADER_MCUBOOT=y`.

### Hardware Flashing & Live Testing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Monitor live serial console logs to verify LED driver initialization, pattern transitions, and visual indicator logs.
