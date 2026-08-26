# Walkthrough: Step 2 - Dual User Buttons Driver

The **Dual User Buttons Driver** (BUTTON1 & BUTTON2) has been implemented with active-low GPIO edge interrupt support, 50ms software debouncing noise filtering, multi-click (single/double), long-press hold detection, compiled, and programmed to the connected **Nordic Thingy:91 X** hardware.

---

## 1. Implemented Features & Driver HAL

- **Driver Header**: [`buttons.h`](file:///d:/Projects/thingy91x/src/platform/drivers/buttons/buttons.h)
  - Exposes `enum button_id`, `enum button_event`, `button_event_cb_t`, `buttons_driver_init()`, `buttons_register_callback()`, `buttons_read_state()`, `buttons_update()`.
- **Driver Logic**: [`buttons.c`](file:///d:/Projects/thingy91x/src/platform/drivers/buttons/buttons.c)
  - Supports `BUTTON_EVENT_SINGLE_CLICK`, `BUTTON_EVENT_DOUBLE_CLICK` (< 400ms), `BUTTON_EVENT_LONG_PRESS` (> 1500ms), and `BUTTON_EVENT_RELEASED`.
  - Implements 50ms software debouncing filter for mechanical switch chatter.
- **Application Integration**: [`app.c`](file:///d:/Projects/thingy91x/src/app/asset_tracker/app.c#L14-L26)
  - `app_init()` registers `on_button_event` handler and logs button interaction events.

---

## 2. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Flashed**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/buttons_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/buttons_test_results.md)
