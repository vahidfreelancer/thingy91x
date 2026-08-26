# Walkthrough: Dynamic RGB LED Visual Status State Machine

The **Dynamic RGB LED Visual Status State Machine** has been implemented in `src/app/hw_test/app.c` for the **Hardware Diagnostic & Remote Test Suite Profile** (`APP_PROFILE_HW_TEST`). The onboard RGB LED dynamically changes color palettes and visual animation patterns to reflect network socket connection states, button triggers, and active JSON command processing.

---

## 1. Visual LED State Palette & Animation Map

| Connection / Execution State | RGB Color ($R, G, B$) | Visual Pattern | Description |
| :--- | :--- | :--- | :--- |
| **IDLE_DISCONNECTED** | Green (`0, 255, 0`) | `LED_PATTERN_BREATHE` | Smooth green breathing pulse (idle, ready for button press) |
| **CONNECTING_TCP** | Blue (`0, 128, 255`) | `LED_PATTERN_BLINK_FAST` | Rapid blue blinking (TCP socket handshake to `s4.sytemonitor.co.uk:1200`) |
| **SOCKET_CONNECTED** | Cyan (`0, 255, 255`) | `LED_PATTERN_SOLID` | Solid cyan glow (active TCP socket connection established) |
| **PROCESSING_COMMAND** | Magenta (`255, 0, 255`) | `LED_PATTERN_BLINK_FAST` | Rapid magenta flashes (active JSON command reception & response tx) |
| **ERROR_DISCONNECTED** | Red (`255, 0, 0`) | `LED_PATTERN_BLINK_SLOW` | Slow red pulse (connection loss or error warning) |

---

## 2. Code Implementation Highlights

- **State Enum & Helper**: [`app.c`](file:///d:/Projects/thingy91x/src/app/hw_test/app.c#L35-L65)
  - `enum hw_test_led_state` defines all 5 connection and function states.
  - `set_hw_test_led_state()` updates RGB colors and sinusoidal animation algorithms in real-time.
- **State Transition Triggers**:
  - `app_init()` -> Sets `HW_STATE_IDLE_DISCONNECTED` (Green Breathing).
  - `on_hw_test_button()` -> Sets `HW_STATE_CONNECTING_TCP` (Blue Blinking) then `HW_STATE_SOCKET_CONNECTED` (Solid Cyan).
  - `process_json_command()` -> Sets `HW_STATE_PROCESSING_COMMAND` (Magenta Flashes) during serialization, then returns to `HW_STATE_SOCKET_CONNECTED` (Solid Cyan).

---

## 3. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Flashed**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/led_status_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/led_status_test_results.md)
