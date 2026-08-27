# Implementation Plan: Python Serial Monitor Terminal Tool (`Tools/Terminal/monitor.py`)

Create a standalone, cross-platform Python Serial Terminal Monitoring tool in `Tools/Terminal/monitor.py` for the **Nordic Thingy:91 X**.

---

## 1. Key Tool Features

- **Auto-Port Detection**: Auto-detects connected Nordic Thingy:91 X serial ports (`COM15`, `COM14`, etc.).
- **Hardware Flow Control (RTS/DTR)**: Enables `rtscts=False/True`, `rts=1`, `dtr=1` automatically using `pyserial`.
- **Live ANSI Color Highlighting**: Color-codes heartbeat messages, button events, LED state transitions, cellular signal metrics, and JSON command responses.
- **Interactive Terminal Input**: Allows sending diagnostic test commands (`PING`, `GET_ENV_DATA`, `GET_MOTION_DATA`, `GET_EKF_FUSION`, `GET_BATTERY_DATA`, `GET_CELLULAR_INFO`) directly to the device.
- **Log Exporting**: Automatically logs received telemetry to timestamped session files in `Artifacts/serial_monitor.log`.

---

## Proposed Changes

### Tools Layer (`Tools/Terminal/`)

#### [NEW] [monitor.py](file:///d:/Projects/thingy91x/Tools/Terminal/monitor.py)
- Interactive CLI serial terminal built with `pyserial` and `argparse`.
- Auto-scans system COM ports if no port argument is specified.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/serial_terminal_tool_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/serial_terminal_tool_implementation_plan.md)
#### [NEW] [Artifacts/serial_terminal_tool_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/serial_terminal_tool_walkthrough.md)
#### [NEW] [Artifacts/serial_terminal_tool_test_results.md](file:///d:/Projects/thingy91x/Artifacts/serial_terminal_tool_test_results.md)

---

## Verification Plan

### Tool Execution Test
- Run `python Tools/Terminal/monitor.py --port COM15 --baud 115200`.
- Verify live heartbeat logs and button events render with color formatting.
