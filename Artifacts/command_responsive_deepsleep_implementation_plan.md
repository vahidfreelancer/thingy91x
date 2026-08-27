# Implementation Plan: Barometric Altitude Filter & Command-Responsive Deep Sleep

Implement filtered altitude calculation in `GET_ENV_DATA` and convert TCP socket operation to command-only responsive mode with a **60-Second Inactivity Deep Sleep** mode waking up on **BUTTON1**.

---

## 1. Feature Specifications

### Feature 1: Filtered Altitude Calculation in `GET_ENV_DATA`
- **Barometric Formula**:
  $$h = 44330.77 \cdot \left(1.0 - \left(\frac{P_{\text{hPa}}}{1013.25}\right)^{0.190295}\right)$$
- **Low-Pass Filter (EMA)**:
  Apply exponential moving average filtering ($\alpha = 0.15$) to smooth atmospheric noise:
  $$h_{\text{filtered}}[k] = \alpha \cdot h_{\text{raw}}[k] + (1 - \alpha) \cdot h_{\text{filtered}}[k-1]$$
- **JSON Telemetry Field**:
  Include `"altitude": {"calculated_m": 72.45, "filtered_m": 71.80, "unit": "m"}` in `GET_ENV_DATA`.

### Feature 2: Command-Only Responsive Mode & 60s Inactivity Deep Sleep
- **Remove Circular Telemetry Dispatch**:
  Eliminate the periodic automatic transmission of dummy test commands (`cmd_step % 8`). Transmit data **strictly in response to incoming server commands**.
- **60-Second Inactivity Timeout**:
  Track `last_command_time = k_uptime_get()`. Reset timer whenever a command is received or BUTTON1 is pressed.
- **Deep Sleep Transition**:
  If `(k_uptime_get() - last_command_time) >= 60,000 ms`:
  - Close socket (`zsock_close(client_fd)`).
  - Place cellular modem into flight/sleep mode (`cellular_modem_sleep()`).
  - Turn OFF RGB LED to conserve battery.
  - Suspend workqueue task until woken.
- **Wake Up on BUTTON1**:
  Pressing BUTTON1 immediately wakes up the device, restores cellular modem attach, reconnects socket to `s4.sytemonitor.co.uk:1200`, and resets the 60s inactivity timer.

---

## Proposed Changes

### Application Layer (`src/app/hw_test/`)

#### [MODIFY] [app.c](file:///d:/Projects/thingy91x/src/app/hw_test/app.c)
- Add altitude calculation and low-pass EMA filter variables (`altitude_raw`, `altitude_filtered`).
- Update `process_json_command()` for `GET_ENV_DATA` to include altitude telemetry fields.
- Remove automatic circular command loop (`test_cmds[cmd_step % 8]`).
- Implement `last_command_timestamp`, 60-second inactivity check, socket teardown, and low-power deep sleep mode.
- Update `on_hw_test_button()` to wake up system from deep sleep and re-establish socket connection.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/command_responsive_deepsleep_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/command_responsive_deepsleep_implementation_plan.md)
#### [NEW] [Artifacts/command_responsive_deepsleep_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/command_responsive_deepsleep_walkthrough.md)
#### [NEW] [Artifacts/command_responsive_deepsleep_test_results.md](file:///d:/Projects/thingy91x/Artifacts/command_responsive_deepsleep_test_results.md)

---

## Verification Plan

### Automated Build Verification
- Compile with `west build -b thingy91x/nrf9151/ns -d build -- -DCONFIG_APP_PROFILE_HW_TEST=y`.

### Hardware Flashing & Live Testing
- Flash `build/dfu_application.zip` via MCUboot DFU.
- Send `GET_ENV_DATA` and verify `"altitude"` fields in JSON response.
- Confirm device only responds when commands are sent.
- Leave idle for 60 seconds and verify socket closure and deep sleep state.
- Press BUTTON1 and confirm immediate system wakeup and re-connection.
