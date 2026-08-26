# Implementation Plan: Hardware GPIO Interrupt & Pull-Up Fix for Dual User Buttons

Fix button responsiveness on the **Nordic Thingy:91 X (PCA20065)** by:
1. Configuring physical GPIO input flags (`GPIO_INPUT | button0_spec.dt_flags`) so internal `GPIO_PULL_UP` resistors and `GPIO_ACTIVE_LOW` inversion are active.
2. Registering native Zephyr GPIO hardware interrupt callbacks (`gpio_init_callback`, `gpio_add_callback`, `gpio_pin_interrupt_configure_dt`) for instant button press detection.
3. Removing the automatic fallback simulation timer (`step % 10 == 0`) so the RGB LED remains in **Green Breathing** state until the user physically presses **BUTTON1** or **BUTTON2**.

---

## Proposed Changes

### Drivers Layer (`src/platform/drivers/buttons/`)

#### [MODIFY] [buttons.c](file:///d:/Projects/thingy91x/src/platform/drivers/buttons/buttons.c)
- Update `buttons_driver_init()` to apply `gpio_pin_configure_dt(&button_spec, GPIO_INPUT | button_spec.dt_flags)`.
- Configure interrupts with `gpio_pin_interrupt_configure_dt(&button_spec, GPIO_INT_EDGE_TO_ACTIVE)`.
- Attach Zephyr GPIO callbacks (`button0_cb`, `button1_cb`) to fire `registered_cb()` instantly on physical button press.
- Remove automatic simulation block from `buttons_update()`.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/button_interrupt_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/button_interrupt_implementation_plan.md)
#### [NEW] [Artifacts/button_interrupt_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/button_interrupt_walkthrough.md)
#### [NEW] [Artifacts/button_interrupt_test_results.md](file:///d:/Projects/thingy91x/Artifacts/button_interrupt_test_results.md)
- Save design plans, execution walkthrough, and hardware test scenario results in `Artifacts/`.

---

## Verification Plan

### Automated / Build Verification
- Compile firmware using nRF Connect SDK toolchain (`west build -b thingy91x/nrf9151/ns -- -DCONFIG_APP_PROFILE_HW_TEST=y`) with `CONFIG_BOOTLOADER_MCUBOOT=y`.

### Hardware Flashing & Live Testing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Observe physical RGB LED remaining in **Green Breathing** state indefinitely after power-on.
- Press physical **BUTTON1** or **BUTTON2** on the board and verify the RGB LED instantly transitions to **Blue Blinking** -> **Solid Cyan Glow** -> **Magenta Flashes**.
