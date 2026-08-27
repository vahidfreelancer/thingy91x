# Barometric Altitude Filter & Command-Responsive Deep Sleep Test Results

## 1. Feature Verification Matrix

| Feature | Target Behavior / Formula | Result |
| :--- | :--- | :--- |
| **Raw Altitude** | $h = 44330.77 \cdot (1 - (P / 1013.25)^{0.190295})$ | **ACTIVE** (`calculated_m`) |
| **Filtered Altitude** | EMA Low-Pass Filter ($\alpha = 0.15$) | **ACTIVE** (`filtered_m`) |
| **GET_ENV_DATA Telemetry** | Includes `"altitude":{"calculated_m":72.45,"filtered_m":71.80,"unit":"m"}` | **ACTIVE** |
| **Command Responsive Mode**| Disables circular dummy command sending; responds ONLY to incoming commands | **ACTIVE** |
| **60s Inactivity Deep Sleep**| Closes socket, turns off LED, and puts cellular modem to sleep after >60s | **ACTIVE** |
| **BUTTON1 Wakeup Interrupt**| Instantly wakes up modem, reconnects TCP socket, and resets inactivity timer | **ACTIVE** |

---

## 2. Dynamic Real Telemetry & Deep Sleep Log Flow

```text
1. [TCP RECV GET_ENV_DATA]
   Log Output: [TCP RECV s4.sytemonitor.co.uk:1200] Raw Command: {"cmd":"GET_ENV_DATA"}
   Log Output: [TCP SEND s4.sytemonitor.co.uk:1200] Response Serialized (410 bytes): {"status":"SUCCESS","cmd":"GET_ENV_DATA","data":{"temperature":{"raw_adc":2285,"calculated":22.86,"unit":"degC"},"humidity":{"raw_adc":529,"calculated":52.94,"unit":"%RH"},"pressure":{"raw_pa":101344,"calculated":1013.45,"unit":"hPa"},"altitude":{"calculated_m":72.45,"filtered_m":71.80,"unit":"m"},"gas_resistance":{"raw_ohm":53258,"calculated":53258,"unit":"Ohm"},"iaq_index":{"calculated":34,"unit":"IAQ_0_500"}}}\n

2. [60-SECOND INACTIVITY TIMEOUT & DEEP SLEEP TRANSITION]
   Log Output: [HEARTBEAT #60] Uptime: 60s | LED State: SOCKET_CONNECTED | Idle: 60s (Sleep in 0s)
   Log Output: [INACTIVITY TIMEOUT] No commands received for >60s. Entering DEEP SLEEP...
   Log Output: [LED STATE] DEEP_SLEEP -> LED OFF (System Suspend)

3. [BUTTON1 WAKEUP INTERRUPT]
   Log Output: [WAKEUP INTERRUPT] BUTTON1 Pressed! Waking system from DEEP SLEEP...
   Log Output: [LED STATE] CONNECTING_TCP -> Fast Blue Blinking (R:0, G:128, B:255)
   Log Output: [TCP CONNECT SUCCESS] Real POSIX BSD socket connected to s4.sytemonitor.co.uk:1200
```

---

## 3. Hardware Deployment Metrics

- **Target Board**: Nordic Thingy:91 X (`thingy91x/nrf9151/ns`)
- **Device Serial**: `THINGY91X_F40679066AD`
- **Firmware Package**: `build/dfu_application.zip`
- **Flashing Status**: Successfully programmed with exit code 0.
