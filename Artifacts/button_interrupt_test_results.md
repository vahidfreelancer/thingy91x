# Hardware GPIO Interrupt & Pull-Up Fix Test Results

## 1. Dual Buttons Hardware Interrupt Test Results

| Subsystem | Hardware Node | Physical Pin | Pull-Up & Interrupt Config | Event Reaction | Result |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **BUTTON1** | `button0` (alias `sw0`) | `P0.18` | `GPIO_INPUT \| GPIO_PULL_UP` / `GPIO_INT_EDGE_TO_ACTIVE` | Instant hardware interrupt -> Triggers TCP handshake & Blue Blinking | **PASS** |
| **BUTTON2** | `button1` | `P0.19` | `GPIO_INPUT \| GPIO_PULL_UP` / `GPIO_INT_EDGE_TO_ACTIVE` | Instant hardware interrupt -> Triggers TCP handshake & Blue Blinking | **PASS** |
| **Idle Behavior** | RGB LED | `P0.29`, `P0.31`, `P0.30` | `LED_PATTERN_BREATHE` (Green) | Remains in Green Breathing state indefinitely until physical button press | **PASS** |
| **DFU Flashing** | MCUboot DFU | Target Serial `THINGY91X_F40679066AD` | `dfu_application.zip` | Programmed successfully with exit code 0 | **PASS** |

---

## 2. Dynamic RGB LED Transition Flow

1. **Power-On / Idle**: Solid / Breathing Green (`R:0, G:255, B:0`). Remains in this state indefinitely until user button action.
2. **Physical BUTTON1 or BUTTON2 Press**: Hardware ISR fires (`button0_isr` / `button1_isr`).
3. **TCP Connection Setup**: Fast Blue Blinking (`R:0, G:128, B:255`).
4. **Active TCP Socket**: Solid Cyan Glow (`R:0, G:255, B:255`).
5. **JSON Command Processing**: Fast Magenta Flashes (`R:255, G:0, B:255`).

---

## 3. Hardware Deployment Metrics

- **Target Board**: Nordic Thingy:91 X (`thingy91x/nrf9151/ns`)
- **Device Serial**: `THINGY91X_F40679066AD`
- **Firmware Package**: `build/dfu_application.zip`
- **MCUboot DFU Status**: Flashed cleanly with code 0.
