# Implementation Plan: TCP JSON Message Line-Framing (`\n`) for Remote Server Compatibility

Append line-framing newline terminators (`\n`) to all outbound TCP JSON telemetry responses transmitted to `s4.sytemonitor.co.uk:1200`.

---

## 1. Problem Analysis

- **Symptom**:
  `zsock_send()` returns `352 bytes sent`, but the remote TCP server at `s4.sytemonitor.co.uk:1200` does not trigger its data reception handler.
- **Root Cause**:
  TCP socket stream protocols accumulate received bytes in an internal stream buffer until a message boundary delimiter (`\n` or `\r\n`) is encountered. The serialized JSON strings currently lack trailing newline terminators, leaving payloads queued in the server's TCP socket buffer.

---

## Proposed Changes

### Application Layer (`src/app/hw_test/`)

#### [MODIFY] [app.c](file:///d:/Projects/thingy91x/src/app/hw_test/app.c)
- Update all `snprintf(resp_buf, ...)` JSON format strings in `process_json_command()` to include trailing `\n` message boundaries.
- Example: `"{\"status\":\"SUCCESS\",\"cmd\":\"PING\",...}\n"`.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/tcp_line_framing_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/tcp_line_framing_implementation_plan.md)
#### [NEW] [Artifacts/tcp_line_framing_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/tcp_line_framing_walkthrough.md)
#### [NEW] [Artifacts/tcp_line_framing_test_results.md](file:///d:/Projects/thingy91x/Artifacts/tcp_line_framing_test_results.md)

---

## Verification Plan

### Automated Build Verification
- Build firmware with `west build -b thingy91x/nrf9151/ns -d build -- -DCONFIG_APP_PROFILE_HW_TEST=y`.

### Hardware Flashing & Server Verification
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`).
- Run `Tools/Terminal/monitor.py`, press BUTTON1, and verify `s4.sytemonitor.co.uk:1200` receives and processes the line-terminated JSON telemetry payloads.
