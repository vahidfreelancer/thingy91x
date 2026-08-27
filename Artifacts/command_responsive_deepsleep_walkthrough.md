# Walkthrough: Barometric Altitude Filter & Command-Responsive Deep Sleep

Barometric altitude calculation, low-pass EMA filtering, command-only responsive mode, and 60-second inactivity deep sleep mode with **BUTTON1** wakeup have been implemented in `src/app/hw_test/app.c` for the **Nordic Thingy:91 X (PCA20065)**.

---

## 1. Key Improvements

1. **Filtered Altitude Telemetry**:
   - Calculates raw altitude ($h = 44330.77 \cdot (1 - (P / 1013.25)^{0.190295})$) and applies an Exponential Moving Average (EMA) low-pass filter ($\alpha = 0.15$).
   - Returns `"altitude":{"calculated_m":72.45,"filtered_m":71.80,"unit":"m"}` inside `GET_ENV_DATA`.
2. **Command-Only Responsive Mode**:
   - Removed circular dummy command sending. The device transmits responses **only when server commands are received**.
3. **60-Second Inactivity Deep Sleep & BUTTON1 Wakeup**:
   - Automatically closes the TCP socket, turns OFF the RGB LED, and puts the modem into flight mode if no command is received for **>60 seconds**.
   - Pressing **BUTTON1** instantly wakes up the system, reconnects the TCP socket to `s4.sytemonitor.co.uk:1200`, and resets the inactivity timer.

---

## 2. Updated Code Implementations

- **Application Module**: [`app.c`](file:///d:/Projects/thingy91x/src/app/hw_test/app.c#L110-L465)

---

## 3. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Package**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/command_responsive_deepsleep_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/command_responsive_deepsleep_test_results.md)
