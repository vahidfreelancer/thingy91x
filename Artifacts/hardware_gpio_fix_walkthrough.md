# Walkthrough: Physical GPIO Devicetree Hardware Fix for RGB LED & Buttons

The physical Devicetree GPIO pin bindings for the **RGB LED** and **Dual User Buttons** have been updated in `src/platform/drivers/led/led.c` and `src/platform/drivers/buttons/buttons.c` targeting the **Nordic Thingy:91 X (PCA20065)** hardware.

---

## 1. Hardware Root Cause Identified & Resolved

In the official **Thingy:91 X (PCA20065)** Devicetree specification (`thingy91x_nrf9151_common.dtsi`), the RGB channels and Buttons are mapped to specific `gpio0` pins:
- **Red LED (`red_led`)**: Pin `P0.29`
- **Green LED (`green_led`)**: Pin `P0.31`
- **Blue LED (`blue_led`)**: Pin `P0.30`
- **BUTTON1 (`button0` / `sw0`)**: Pin `P0.18` (Active Low)
- **BUTTON2 (`button1`)**: Pin `P0.19` (Active Low)

The previous driver implementation queried generic `DEVICE_DT_GET_ANY(gpio_leds)` and `DEVICE_DT_GET_ANY(gpio_keys)` container nodes which did not bind to individual GPIO pins. Both drivers have been updated to use direct `GPIO_DT_SPEC_GET()` bindings.

---

## 2. Updated Code Implementations

- **RGB LED Driver**: [`led.c`](file:///d:/Projects/thingy91x/src/platform/drivers/led/led.c)
  - Configures physical GPIO specs (`red_led_spec`, `green_led_spec`, `blue_led_spec`) directly with `gpio_pin_configure_dt()` and `gpio_pin_set_dt()`.
- **Dual Buttons Driver**: [`buttons.c`](file:///d:/Projects/thingy91x/src/platform/drivers/buttons/buttons.c)
  - Configures physical GPIO input specs (`button0_spec`, `button1_spec`) directly with `gpio_pin_configure_dt()` and `gpio_pin_get_dt()`.

---

## 3. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Package**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/hardware_gpio_fix_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/hardware_gpio_fix_test_results.md)
