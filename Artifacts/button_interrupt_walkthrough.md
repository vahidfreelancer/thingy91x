# Walkthrough: Hardware GPIO Interrupt & Pull-Up Fix for Dual User Buttons

Native Zephyr GPIO hardware interrupt callbacks and explicit pull-up resistor configurations have been implemented in `src/platform/drivers/buttons/buttons.c` for **BUTTON1** (`P0.18`) and **BUTTON2** (`P0.19`) on the **Nordic Thingy:91 X (PCA20065)**.

---

## 1. Resolved Issues

1. **Pull-Up & Polarity Configuration**:
   - Updated `gpio_pin_configure_dt(&button_spec, GPIO_INPUT | button_spec.dt_flags)` to activate internal `GPIO_PULL_UP` resistors and `GPIO_ACTIVE_LOW` inversion.
2. **Hardware Interrupt Integration**:
   - Added native Zephyr GPIO callbacks (`button0_cb_data`, `button1_cb_data`) and ISR functions (`button0_isr`, `button1_isr`) configured for active-edge triggers (`GPIO_INT_EDGE_TO_ACTIVE`).
3. **Auto-Timer Removal**:
   - Removed periodic auto-simulation loop from `buttons_update()`. The system stays in **Green Breathing** state until physical button interaction occurs.

---

## 2. Driver Code Highlights

- **[`buttons.c`](file:///d:/Projects/thingy91x/src/platform/drivers/buttons/buttons.c)**:
  - Registers hardware ISR callbacks `button0_isr` (BUTTON1) and `button1_isr` (BUTTON2).
  - Triggers instant `registered_cb()` execution on physical button press.

---

## 3. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Flashed**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/button_interrupt_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/button_interrupt_test_results.md)
