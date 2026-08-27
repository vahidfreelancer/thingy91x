# Python Serial Monitor Terminal Tool Test Results

## 1. Tool Features & Verification Matrix

| Feature Module | Verification Test Scenario | Expected Behavior | Result |
| :--- | :--- | :--- | :--- |
| **Auto-Port Detection** | `python Tools/Terminal/monitor.py` | Auto-detects `COM15` / `COM14` connected Nordic devices | **PASS** |
| **Flow Control Assertion** | `ser.rts = True`, `ser.dtr = True` | Asserts RTS & DTR signals to unblock USB CDC ACM stream | **PASS** |
| **ANSI Color Highlighting** | `format_colored_line()` | Highlights Green (Heartbeats), Yellow (Buttons), Cyan (TCP), Magenta (JSON), Red (Errors) | **PASS** |
| **Interactive CLI Input** | Typing `GET_ENV_DATA` / `PING` | Formats and transmits `{"cmd":"..."}` JSON payload | **PASS** |
| **Log File Export** | `--log Artifacts/serial_monitor.log` | Appends timestamped log lines to `Artifacts/serial_monitor.log` | **PASS** |

---

## 2. Command Usage Specification

```bash
# Basic auto-detection on default port (COM15, 115200 baud)
python Tools/Terminal/monitor.py

# Specify COM port explicitly
python Tools/Terminal/monitor.py --port COM15 --baud 115200

# Specify custom log output file
python Tools/Terminal/monitor.py --port COM15 --log Artifacts/my_session.log
```
