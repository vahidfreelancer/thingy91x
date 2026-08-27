# Walkthrough: Serial UART Heartbeat & Asynchronous TCP Connection State Diagnostics

Periodic 1-second serial UART heartbeat logging and TCP connection diagnostic error handling have been implemented in `src/app/hw_test/app.c` and `prj.conf` for the **Nordic Thingy:91 X (PCA20065)**.

---

## 1. Key Improvements

1. **Immediate Serial UART Log Flushing**:
   - Added `CONFIG_LOG_MODE_IMMEDIATE=y` and `CONFIG_LOG_BACKEND_UART=y` to [`prj.conf`](file:///d:/Projects/thingy91x/prj.conf) to guarantee real-time log stream printing over USB `COM15` at 115200 baud.
2. **Periodic 1-Second Heartbeat**:
   - Added `[HEARTBEAT #N]` log output every second detailing system uptime, current LED state name, cellular signal RSRP (dBm), network operator name, and assigned IP address.
3. **TCP Connection Diagnostics & Failure Recovery**:
   - Logs `[TCP CONNECTING attempt #N] Resolving host 's4.sytemonitor.co.uk'...` during TCP handshake (Blue Blinking LED).
   - On connection success, transitions to **Solid Cyan Glow** (`HW_STATE_SOCKET_CONNECTED`).
   - On connection failure or timeout, logs `[TCP ERROR] Connection to s4.sytemonitor.co.uk:1200 failed (-ETIMEDOUT)` and transitions to **Slow Red Pulse** (`HW_STATE_ERROR_DISCONNECTED`).

---

## 2. Updated Code Implementations

- **Application Module**: [`app.c`](file:///d:/Projects/thingy91x/src/app/hw_test/app.c)
- **Kconfig Configuration**: [`prj.conf`](file:///d:/Projects/thingy91x/prj.conf)

---

## 3. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Package**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/serial_heartbeat_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/serial_heartbeat_test_results.md)
