# Dynamic RGB LED Visual Status State Machine Test Results

## 1. State Machine Test Scenarios Executed

| Scenario ID | Connection & Function State | RGB Color Target ($R, G, B$) | Visual LED Pattern | Result |
| :--- | :--- | :--- | :--- | :--- |
| **TC-LED-01** | `HW_STATE_IDLE_DISCONNECTED` | Green (`0, 255, 0`) | `LED_PATTERN_BREATHE` (Smooth green breathing pulse) | **PASS** |
| **TC-LED-02** | `HW_STATE_CONNECTING_TCP` | Blue (`0, 128, 255`) | `LED_PATTERN_BLINK_FAST` (Rapid blue blinking during handshake) | **PASS** |
| **TC-LED-03** | `HW_STATE_SOCKET_CONNECTED` | Cyan (`0, 255, 255`) | `LED_PATTERN_SOLID` (Solid cyan glow for established socket) | **PASS** |
| **TC-LED-04** | `HW_STATE_PROCESSING_COMMAND` | Magenta (`255, 0, 255`) | `LED_PATTERN_BLINK_FAST` (Rapid magenta flashes during JSON rx/tx) | **PASS** |
| **TC-LED-05** | `HW_STATE_ERROR_DISCONNECTED` | Red (`255, 0, 0`) | `LED_PATTERN_BLINK_SLOW` (Slow red pulse warning) | **PASS** |
| **TC-LED-06** | MCUboot DFU Deployment | `dfu_application.zip` | Flashed cleanly to target serial `THINGY91X_F40679066AD`. | **PASS** |

---

## 2. Dynamic LED State Machine Transition Table

```
  [ POWER ON / BOOT ]
           |
           v
 +--------------------+
 | IDLE_DISCONNECTED  |  ---> Green Breathing (0, 255, 0)
 +--------------------+
           |
           | [ User Presses BUTTON1 / BUTTON2 ]
           v
 +--------------------+
 |   CONNECTING_TCP   |  ---> Rapid Blue Blinking (0, 128, 255)
 +--------------------+
           |
           | [ Socket Handshake Complete ]
           v
 +--------------------+
 |  SOCKET_CONNECTED  |  ---> Solid Cyan Glow (0, 255, 255)
 +--------------------+
        |      ^
        |      | [ Command Completed ]
  [ Receive JSON Cmd ]
        v      |
 +--------------------+
 | PROCESSING_COMMAND |  ---> Rapid Magenta Flashes (255, 0, 255)
 +--------------------+
```

---

## 3. Hardware Deployment Metrics

- **Target Board**: Nordic Thingy:91 X (`thingy91x/nrf9151/ns`)
- **Device Serial**: `THINGY91X_F40679066AD`
- **Firmware Package**: `build/dfu_application.zip`
- **Remote Host**: `s4.sytemonitor.co.uk:1200`
