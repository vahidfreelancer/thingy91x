# Implementation Plan: Dynamic RGB LED Visual Status State Machine

Update the **Hardware Diagnostic & Remote Test Suite Profile** (`src/app/hw_test/app.c`) to dynamically reflect the network connection state and active diagnostic functions on the onboard **RGB LED** using distinct visual color palettes and animation patterns.

---

## 1. Visual LED State Mapping Specification

| Connection / Execution State | RGB Color ($R, G, B$) | Visual Pattern | Visual Description |
| :--- | :--- | :--- | :--- |
| **IDLE_DISCONNECTED** | Green (`0, 255, 0`) | `LED_PATTERN_BREATHE` | Smooth green breathing pulse (ready for button press) |
| **CONNECTING_TCP** | Blue (`0, 128, 255`) | `LED_PATTERN_BLINK_FAST` | Rapid blue blinking (TCP socket handshake to `s4.sytemonitor.co.uk:1200`) |
| **SOCKET_CONNECTED** | Cyan (`0, 255, 255`) | `LED_PATTERN_SOLID` | Solid cyan glow (active TCP socket connection established) |
| **PROCESSING_COMMAND** | Magenta (`255, 0, 255`) | `LED_PATTERN_BLINK_FAST` | Rapid magenta pulse (active JSON command reception & response tx) |
| **ERROR_DISCONNECTED** | Red (`255, 0, 0`) | `LED_PATTERN_BLINK_SLOW` | Slow red pulse (connection loss or error warning) |

---

## Proposed Changes

### Application Layer (`src/app/hw_test/`)

#### [MODIFY] [app.c](file:///d:/Projects/thingy91x/src/app/hw_test/app.c)
- Define `enum hw_test_led_state`: `HW_STATE_IDLE_DISCONNECTED`, `HW_STATE_CONNECTING_TCP`, `HW_STATE_SOCKET_CONNECTED`, `HW_STATE_PROCESSING_COMMAND`, `HW_STATE_ERROR`.
- Implement `set_hw_test_led_state(enum hw_test_led_state state)` to update RGB LED color and pattern dynamically.
- Update `on_hw_test_button()`, `process_json_command()`, and `hw_test_work_handler()` to transition states cleanly.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/led_status_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/led_status_implementation_plan.md)
#### [NEW] [Artifacts/led_status_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/led_status_walkthrough.md)
#### [NEW] [Artifacts/led_status_test_results.md](file:///d:/Projects/thingy91x/Artifacts/led_status_test_results.md)
- Save design plans, execution walkthrough, and hardware test scenario results in `Artifacts/`.

---

## Verification Plan

### Automated / Build Verification
- Compile hardware test application using nRF Connect SDK toolchain (`west build -b thingy91x/nrf9151/ns -- -DCONFIG_APP_PROFILE_HW_TEST=y`) with `CONFIG_BOOTLOADER_MCUBOOT=y`.

### Hardware Flashing & Live Testing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Press **BUTTON1** or **BUTTON2** on physical hardware to trigger connection transitions and observe RGB LED color state transitions (Green -> Blue -> Cyan -> Magenta -> Green).
- Monitor live serial console logs to verify LED state transition events.
