# Implementation Plan: Serial UART Heartbeat & Asynchronous TCP Connection State Diagnostics

Enhance the **Hardware Diagnostic & Remote Test Suite Profile** (`src/app/hw_test/app.c`) with:
1. **Periodic 1-Second Serial UART Heartbeat**: Print live system uptime, current LED state name, cellular signal metrics (RSRP/SNR), and network IP address directly to the COM port console (115200 baud).
2. **Asynchronous TCP Connection & Timeout Diagnostics**: Handle TCP socket connection handshakes with a 10-second timeout. If connection fails, log explicit POSIX error trace over UART and transition LED to `HW_STATE_ERROR_DISCONNECTED` (Red Pulse); if connection succeeds, transition LED to `HW_STATE_SOCKET_CONNECTED` (Solid Cyan Glow).

---

## 1. Serial UART Heartbeat Log Format

```
[HEARTBEAT] Uptime: 14 sec | State: IDLE_DISCONNECTED (Green Breathing) | Cellular: LTE-M (-82 dBm) | IP: 10.124.45.89
[BUTTON EVENT] BUTTON1 Pressed -> Transitioning to CONNECTING_TCP (Blue Blinking)...
[TCP CONNECTING] Attempting TCP handshake to s4.sytemonitor.co.uk:1200...
[TCP SUCCESS] Connection established -> Transitioning to SOCKET_CONNECTED (Solid Cyan Glow)
```

---

## Proposed Changes

### Application Layer (`src/app/hw_test/`)

#### [MODIFY] [app.c](file:///d:/Projects/thingy91x/src/app/hw_test/app.c)
- Add 1-second periodic heartbeat logger `LOG_INF("[HEARTBEAT] ...")` and `printk()`.
- Add non-blocking socket connection attempt logic with 10-second timeout.
- Transition LED to `HW_STATE_ERROR_DISCONNECTED` (Red Pulse) if TCP socket fails, or `HW_STATE_SOCKET_CONNECTED` (Cyan Glow) on success.

#### [MODIFY] [prj.conf](file:///d:/Projects/thingy91x/prj.conf)
- Ensure `CONFIG_LOG_BACKEND_UART=y` and `CONFIG_LOG_MODE_IMMEDIATE=y` so log lines flush immediately to serial COM port.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/serial_heartbeat_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/serial_heartbeat_implementation_plan.md)
#### [NEW] [Artifacts/serial_heartbeat_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/serial_heartbeat_walkthrough.md)
#### [NEW] [Artifacts/serial_heartbeat_test_results.md](file:///d:/Projects/thingy91x/Artifacts/serial_heartbeat_test_results.md)

---

## Verification Plan

### Automated / Build Verification
- Build firmware with `west build -b thingy91x/nrf9151/ns -d build -- -DCONFIG_APP_PROFILE_HW_TEST=y`.

### Hardware Deployment & Flashing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Connect serial monitor to `COM15` (115200 baud) and observe continuous 1-second heartbeat logs and live button event state transitions.
