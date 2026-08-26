# Implementation Plan: Step 2 - Dual User Buttons Driver

Implement a high-level Dual User Buttons Driver (`src/platform/drivers/buttons/`) for the Nordic Thingy:91 X platform to manage GPIO interrupts, software debouncing algorithms, single-click, double-click, and long-press hold event callbacks for **BUTTON1** and **BUTTON2**.

## Proposed Changes

### Hardware Abstraction Layer (HAL) - Drivers

#### [NEW] [buttons.h](file:///d:/Projects/thingy91x/src/platform/drivers/buttons/buttons.h)
- Declare `enum button_id`: `BUTTON_ID_1` (Button 1), `BUTTON_ID_2` (Button 2).
- Declare `enum button_event`:
  - `BUTTON_EVENT_SINGLE_CLICK`: Short press and release (< 500ms)
  - `BUTTON_EVENT_DOUBLE_CLICK`: Two rapid presses within 400ms
  - `BUTTON_EVENT_LONG_PRESS`: Sustained press held > 1500ms
  - `BUTTON_EVENT_RELEASED`: Button release event
- Declare callback type: `typedef void (*button_event_cb_t)(enum button_id id, enum button_event event);`
- Expose interface functions:
  - `int buttons_driver_init(void)`: Query Devicetree for `buttons` / `gpio_keys` / GPIO input nodes for BUTTON1 and BUTTON2, configure active-low GPIO edge interrupts.
  - `int buttons_register_callback(button_event_cb_t callback)`: Register event handler callback.
  - `int buttons_read_state(enum button_id id, bool *is_pressed)`: Read instantaneous logic state.
  - `int buttons_update(void)`: Service debouncing and press-duration timing logic.

#### [NEW] [buttons.c](file:///d:/Projects/thingy91x/src/platform/drivers/buttons/buttons.c)
- Implement driver logic using Zephyr `gpio.h` / `k_work`, Devicetree macros (`DT_HAS_COMPAT_STATUS_OKAY`), active-low GPIO edge interrupt callbacks, software debouncing algorithms (50ms noise filter), press hold duration timers, POSIX error return codes, and software simulation fallback for off-target testing.

---

### Devicetree & Board Overlay

#### [MODIFY] [thingy91x_nrf9151_ns.overlay](file:///d:/Projects/thingy91x/boards/thingy91x_nrf9151_ns.overlay)
- Configure `buttons` node definitions for BUTTON1 and BUTTON2 inputs in board overlay.

---

### Application Integration & Build System

#### [MODIFY] [CMakeLists.txt](file:///d:/Projects/thingy91x/src/app/asset_tracker/CMakeLists.txt)
- Include `../../platform/drivers/buttons/buttons.c` and `../../platform/drivers/buttons` in `asset_tracker/CMakeLists.txt`.

#### [MODIFY] [app.c](file:///d:/Projects/thingy91x/src/app/asset_tracker/app.c)
- Call `buttons_driver_init()` and register button callback function during boot to handle button press events.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/buttons_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/buttons_implementation_plan.md)
#### [NEW] [Artifacts/buttons_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/buttons_walkthrough.md)
#### [NEW] [Artifacts/buttons_test_results.md](file:///d:/Projects/thingy91x/Artifacts/buttons_test_results.md)
- Save design plans, execution walkthrough, and hardware test scenario results in `Artifacts/`.

---

## Verification Plan

### Automated / Build Verification
- Compile firmware using nRF Connect SDK toolchain (`west build -b thingy91x/nrf9151/ns -- -DCONFIG_APP_PROFILE_ASSET_TRACKER=y`) with `CONFIG_BOOTLOADER_MCUBOOT=y`.

### Hardware Flashing & Live Testing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Monitor live serial console logs to verify button press callbacks, debouncing algorithms, and multi-click event handling.
