# Implementation Plan: Python GUI Serial Monitor Dashboard (`Tools/Terminal/gui_monitor.py`)

Create a standalone desktop GUI application in `Tools/Terminal/gui_monitor.py` for the **Nordic Thingy:91 X**.

---

## 1. Key GUI Features

- **Modern Dark-Mode Dashboard**: Clean UI with real-time log viewer, color highlights, and status cards.
- **Visual Log Filtering**: Live search bar to instantly filter logs by keyword (`HEARTBEAT`, `TCP`, `CELLULAR`, `ERROR`, `LED`).
- **Telemetry Meters & Indicators**: Real-time visual cards displaying Uptime, Battery %, RSRP Signal (dBm), and Active LED State.
- **One-Click Diagnostic Buttons**: Quick action buttons to send JSON test commands (`PING`, `GET_ENV_DATA`, `GET_MOTION_DATA`, `GET_BATTERY_DATA`, `GET_CELLULAR_INFO`, `GET_ALL_SENSORS`).
- **COM Port Auto-Connect**: Auto-detects connected Nordic COM ports (`COM15`, `COM14`) and asserts RTS/DTR signals.

---

## Proposed Changes

### Tools Layer (`Tools/Terminal/`)

#### [NEW] [gui_monitor.py](file:///d:/Projects/thingy91x/Tools/Terminal/gui_monitor.py)
- Standalone Python Tkinter desktop GUI app.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/gui_monitor_tool_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/gui_monitor_tool_implementation_plan.md)
#### [NEW] [Artifacts/gui_monitor_tool_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/gui_monitor_tool_walkthrough.md)
#### [NEW] [Artifacts/gui_monitor_tool_test_results.md](file:///d:/Projects/thingy91x/Artifacts/gui_monitor_tool_test_results.md)

---

## Verification Plan

### Automated Verification
- Run `python Tools/Terminal/gui_monitor.py --help`.

### Manual GUI Verification
- Launch `python Tools/Terminal/gui_monitor.py`, verify port auto-connection, test live filter search, and click diagnostic buttons.
