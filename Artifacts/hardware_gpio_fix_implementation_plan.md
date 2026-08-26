# Implementation Plan: Physical GPIO Devicetree Hardware Fix for RGB LED & Buttons

Fix hardware devicetree bindings for the **RGB LED** and **Dual User Buttons** drivers on the **Nordic Thingy:91 X (PCA20065)** by binding directly to the physical `gpio0` pins declared in `thingy91x_nrf9151_common.dtsi`:
- **Red LED (`red_led`)**: Pin `P0.29`
- **Green LED (`green_led`)**: Pin `P0.31`
- **Blue LED (`blue_led`)**: Pin `P0.30`
- **BUTTON1 (`button0` / `sw0`)**: Pin `P0.18` (Active Low, Pull-Up)
- **BUTTON2 (`button1`)**: Pin `P0.19` (Active Low, Pull-Up)

---

## Proposed Changes

### Drivers Layer (`src/platform/drivers/`)

#### [MODIFY] [led.c](file:///d:/Projects/thingy91x/src/platform/drivers/led/led.c)
- Retrieve `GPIO_DT_SPEC_GET(DT_NODELABEL(red_led), gpios)`, `green_led`, and `blue_led`.
- Configure pins with `gpio_pin_configure_dt(&led_spec, GPIO_OUTPUT_INACTIVE)` during `led_driver_init()`.
- Update `led_set_color()` and `led_update()` to drive hardware GPIO pins with `gpio_pin_set_dt()`.

#### [MODIFY] [buttons.c](file:///d:/Projects/thingy91x/src/platform/drivers/buttons/buttons.c)
- Retrieve `GPIO_DT_SPEC_GET(DT_NODELABEL(button0), gpios)` (**BUTTON1**) and `button1` (**BUTTON2**).
- Configure pins with `gpio_pin_configure_dt(&btn_spec, GPIO_INPUT)` and `gpio_pin_interrupt_configure_dt()`.
- Read live physical GPIO pin states with `gpio_pin_get_dt()` inside `buttons_update()`.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/hardware_gpio_fix_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/hardware_gpio_fix_implementation_plan.md)
#### [NEW] [Artifacts/hardware_gpio_fix_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/hardware_gpio_fix_walkthrough.md)
#### [NEW] [Artifacts/hardware_gpio_fix_test_results.md](file:///d:/Projects/thingy91x/Artifacts/hardware_gpio_fix_test_results.md)
- Save design plans, execution walkthrough, and hardware test scenario results in `Artifacts/`.

---

## Verification Plan

### Automated / Build Verification
- Compile firmware using nRF Connect SDK toolchain (`west build -b thingy91x/nrf9151/ns -- -DCONFIG_APP_PROFILE_HW_TEST=y`) with `CONFIG_BOOTLOADER_MCUBOOT=y`.

### Hardware Flashing & Live Testing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Verify onboard RGB LED immediately lights up with green breathing pattern upon power-on.
- Press **BUTTON1** or **BUTTON2** on the physical board to trigger connection transitions and observe active RGB LED state changes (Green -> Blue -> Cyan -> Magenta).
