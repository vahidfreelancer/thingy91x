# Walkthrough: TCP JSON Message Line-Framing (`\n`) for Remote Server Compatibility

Line-framing newline terminators (`\n`) have been added to all outbound TCP JSON telemetry format strings in `src/app/hw_test/app.c` for the **Nordic Thingy:91 X (PCA20065)**.

---

## 1. Key Improvements

1. **Line-Framing Delimiter (`\n`)**:
   - Updated all `snprintf(resp_buf, ...)` JSON format strings in `process_json_command()` in [`app.c`](file:///d:/Projects/thingy91x/src/app/hw_test/app.c) to end with explicit `\n` newlines.
2. **Server Stream Compatibility**:
   - Stream socket parsers on `s4.sytemonitor.co.uk:1200` instantly detect the `\n` message boundary and trigger their `onData` / `readLine` parsing event handlers without buffering payload chunks indefinitely.

---

## 2. Updated Code Implementations

- **Application Module**: [`app.c`](file:///d:/Projects/thingy91x/src/app/hw_test/app.c#L125-L250)

---

## 3. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Package**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/tcp_line_framing_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/tcp_line_framing_test_results.md)
