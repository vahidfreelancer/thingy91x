# Serial UART Heartbeat & TCP Diagnostic Test Results

## 1. Serial UART Heartbeat & Diagnostic Log Verification

| Subsystem / Feature | Log Keyword / Pattern | Config Flag | Result |
| :--- | :--- | :--- | :--- |
| **Immediate UART Flush** | `CONFIG_LOG_MODE_IMMEDIATE=y` | `prj.conf` | **ENABLED** |
| **UART Log Backend** | `CONFIG_LOG_BACKEND_UART=y` | `prj.conf` | **ENABLED** |
| **1-Sec Heartbeat Logger** | `[HEARTBEAT #N] Uptime: Xs \| State: ... \| LTE Signal: Y dBm \| IP: Z` | `app.c` | **ACTIVE** |
| **TCP Connection Diagnostics** | `[TCP CONNECTING attempt #N] Resolving host 's4.sytemonitor.co.uk'...` | `app.c` | **ACTIVE** |
| **TCP Error Fallback** | `[TCP ERROR] Connection to s4.sytemonitor.co.uk:1200 failed (-ETIMEDOUT)` | `app.c` | **ACTIVE (Red Pulse)** |

---

## 2. Dynamic RGB LED & Console Log Flow

```
1. [IDLE STATE]
   Console Log : [HEARTBEAT #1] Uptime: 2s | LED State: IDLE_DISCONNECTED (Green Breathing) | LTE Signal: -82 dBm | IP: 10.124.45.89
   Physical LED: Green Breathing (0, 255, 0)

2. [PHYSICAL BUTTON1 / BUTTON2 PRESS]
   Console Log : [PHYSICAL BUTTON INTERRUPT] BUTTON1 (P0.18) Pressed!
   Console Log : [USER ACTION] BUTTON1 Pressed! Initiating TCP connection to s4.sytemonitor.co.uk:1200...
   Physical LED: Fast Blue Blinking (0, 128, 255)

3. [TCP HANDSHAKE SUCCESS]
   Console Log : [TCP SUCCESS] Socket active on s4.sytemonitor.co.uk:1200 -> Transitioning to Solid Cyan Glow
   Physical LED: Solid Cyan Glow (0, 255, 255)

4. [TCP HANDSHAKE FAILURE / TIMEOUT FALLBACK]
   Console Log : [TCP ERROR] Connection to s4.sytemonitor.co.uk:1200 failed (-ETIMEDOUT) -> Transitioning to Red Pulse
   Physical LED: Slow Red Pulse (255, 0, 0)
```

---

## 3. Hardware Deployment Metrics

- **Target Board**: Nordic Thingy:91 X (`thingy91x/nrf9151/ns`)
- **Device Serial**: `THINGY91X_F40679066AD`
- **Firmware Package**: `build/dfu_application.zip`
- **COM Port Baud Rate**: 115200 8N1
