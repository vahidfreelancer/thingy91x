# Walkthrough: Python Serial Monitor Terminal Tool (`Tools/Terminal/monitor.py`)

A dedicated, cross-platform Python Serial Terminal Monitoring tool has been created under [`Tools/Terminal/monitor.py`](file:///d:/Projects/thingy91x/Tools/Terminal/monitor.py) for the **Nordic Thingy:91 X**.

---

## 1. Directory Structure

```text
d:\Projects\thingy91x\
└── Tools/
    └── Terminal/
        └── monitor.py   <-- Standalone Python Serial Terminal Tool
```

---

## 2. Key Features

- **Auto-Port Detection**: Auto-detects connected Nordic Thingy:91 X ports (`COM15`, `COM14`).
- **RTS/DTR Flow Control**: Sets `ser.rts = True` and `ser.dtr = True` automatically using `pyserial`.
- **Live ANSI Color Highlighting**: Color-codes heartbeat messages (Green), button events (Yellow), connection state (Cyan), JSON telemetry (Magenta), and errors (Red).
- **Interactive Command Console**: Send test commands (`PING`, `GET_ENV_DATA`, `GET_MOTION_DATA`, `GET_EKF_FUSION`, `GET_BATTERY_DATA`, `GET_CELLULAR_INFO`, `GET_ALL_SENSORS`) directly from the terminal prompt.
- **Log Exporting**: Automatically records all captured serial logs to [`Artifacts/serial_monitor.log`](file:///d:/Projects/thingy91x/Artifacts/serial_monitor.log).

---

## 3. Usage Instructions

```bash
# Launch serial terminal (auto-detects port COM15)
python Tools/Terminal/monitor.py

# Specify explicit port and baud rate
python Tools/Terminal/monitor.py --port COM15 --baud 115200
```
