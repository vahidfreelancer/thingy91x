#!/usr/bin/env python3
"""
Nordic Thingy:91 X Serial Monitor Terminal Tool
------------------------------------------------
Cross-platform serial monitoring terminal with auto-port detection,
RTS/DTR flow control assertion, live ANSI color highlighting,
and interactive JSON diagnostic command transmission.

Usage:
    python Tools/Terminal/monitor.py [--port COM15] [--baud 115200] [--log Artifacts/serial_monitor.log]
"""

import sys
import os
import time
import datetime
import threading
import argparse

# Color definitions for terminal logging
COLOR_RESET   = "\033[0m"
COLOR_GREEN   = "\033[92m" # Heartbeats / Idle
COLOR_BLUE    = "\033[94m" # Connecting / Network
COLOR_CYAN    = "\033[96m" # Connected / Sockets
COLOR_MAGENTA = "\033[95m" # JSON Data / Processing
COLOR_YELLOW  = "\033[93m" # Button Events / Actions
COLOR_RED     = "\033[91m" # Errors / Alerts
COLOR_BOLD    = "\033[1m"

try:
    import serial
    import serial.tools.list_ports
except ImportError:
    print(f"{COLOR_RED}[ERROR] 'pyserial' package is not installed.{COLOR_RESET}")
    print("Installing 'pyserial' automatically via pip...")
    os.system(f"{sys.executable} -m pip install pyserial")
    import serial
    import serial.tools.list_ports


def find_nordic_port():
    """Auto-detect connected Nordic Thingy:91 X serial ports."""
    ports = list(serial.tools.list_ports.comports())
    nordic_ports = []
    
    for p in ports:
        desc = f"{p.device} - {p.description} [{p.hwid}]"
        if "Nordic" in p.description or "Thingy" in p.description or "VCOM" in p.description:
            nordic_ports.append(p.device)
        elif "COM15" in p.device or "COM14" in p.device:
            nordic_ports.append(p.device)
            
    if nordic_ports:
        # Prefer COM15 if available, otherwise first detected
        if "COM15" in nordic_ports:
            return "COM15"
        return nordic_ports[0]
    
    if ports:
        return ports[0].device
        
    return "COM15"


def format_colored_line(line):
    """Apply ANSI color highlights based on log keywords."""
    if "[HEARTBEAT" in line:
        return f"{COLOR_GREEN}{COLOR_BOLD}{line}{COLOR_RESET}"
    elif "[USER ACTION]" in line or "Pressed" in line:
        return f"{COLOR_YELLOW}{COLOR_BOLD}{line}{COLOR_RESET}"
    elif "[LED STATE]" in line:
        if "Green" in line:
            return f"{COLOR_GREEN}{line}{COLOR_RESET}"
        elif "Blue" in line:
            return f"{COLOR_BLUE}{line}{COLOR_RESET}"
        elif "Cyan" in line:
            return f"{COLOR_CYAN}{line}{COLOR_RESET}"
        elif "Magenta" in line:
            return f"{COLOR_MAGENTA}{line}{COLOR_RESET}"
        elif "Red" in line:
            return f"{COLOR_RED}{line}{COLOR_RESET}"
    elif "[TCP SUCCESS]" in line or "[TCP SOCKET" in line or "Connected" in line:
        return f"{COLOR_CYAN}{COLOR_BOLD}{line}{COLOR_RESET}"
    elif "[TCP RECV" in line or "[TCP SEND" in line or "Response" in line:
        return f"{COLOR_MAGENTA}{line}{COLOR_RESET}"
    elif "[ERROR]" in line or "[TCP ERROR]" in line or "failed" in line:
        return f"{COLOR_RED}{COLOR_BOLD}{line}{COLOR_RESET}"
    elif "[CELLULAR" in line:
        return f"{COLOR_BLUE}{line}{COLOR_RESET}"
    
    return line


def read_serial_loop(ser, log_file, stop_event):
    """Background thread to read serial stream continuously."""
    while not stop_event.is_set():
        try:
            if ser.in_waiting > 0:
                raw_data = ser.readline()
                try:
                    line = raw_data.decode("utf-8", errors="replace").rstrip()
                except Exception:
                    line = str(raw_data)
                
                if line:
                    timestamp = datetime.datetime.now().strftime("%H:%M:%S.%f")[:-3]
                    formatted = f"[{timestamp}] {line}"
                    colored = format_colored_line(formatted)
                    
                    print(colored)
                    
                    if log_file:
                        try:
                            with open(log_file, "a", encoding="utf-8") as f:
                                f.write(f"{formatted}\n")
                        except Exception:
                            pass
            else:
                time.sleep(0.05)
        except Exception as e:
            if not stop_event.is_set():
                print(f"{COLOR_RED}[SERIAL READ ERROR] {e}{COLOR_RESET}")
                time.sleep(1.0)


def main():
    parser = argparse.ArgumentParser(description="Nordic Thingy:91 X Serial Monitor Terminal Tool")
    parser.add_argument("--port", type=str, default=None, help="Serial COM port (e.g. COM15)")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate (default: 115200)")
    parser.add_argument("--log", type=str, default="Artifacts/serial_monitor.log", help="Log output file path")
    args = parser.parse_args()

    port_name = args.port if args.port else find_nordic_port()
    baud_rate = args.baud

    print(f"\n{COLOR_CYAN}{COLOR_BOLD}============================================================={COLOR_RESET}")
    print(f"{COLOR_CYAN}{COLOR_BOLD}   Nordic Thingy:91 X Serial Monitor Terminal Tool v1.0      {COLOR_RESET}")
    print(f"{COLOR_CYAN}{COLOR_BOLD}============================================================={COLOR_RESET}")
    print(f"Target Port : {COLOR_BOLD}{port_name}{COLOR_RESET}")
    print(f"Baud Rate   : {COLOR_BOLD}{baud_rate} 8N1{COLOR_RESET}")
    print(f"Flow Control: {COLOR_BOLD}RTS=True, DTR=True (Hardware Asserted){COLOR_RESET}")
    print(f"Log File    : {COLOR_BOLD}{args.log}{COLOR_RESET}")
    print(f"{COLOR_CYAN}-------------------------------------------------------------{COLOR_RESET}")
    print("Available Interactive Commands:")
    print("  PING, GET_ENV_DATA, GET_MOTION_DATA, GET_MAG_DATA, GET_EKF_FUSION,")
    print("  GET_BATTERY_DATA, GET_WIFI_SCAN, GET_CELLULAR_INFO, GET_ALL_SENSORS, exit")
    print(f"{COLOR_CYAN}-------------------------------------------------------------{COLOR_RESET}\n")

    try:
        ser = serial.Serial(
            port=port_name,
            baudrate=baud_rate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1.0
        )
        # Assert hardware flow control signals to unblock CDC UART bridge
        ser.rts = True
        ser.dtr = True
        
        print(f"{COLOR_GREEN}{COLOR_BOLD}[CONNECTED] Serial port {port_name} opened successfully! Listening live...{COLOR_RESET}\n")
    except Exception as e:
        print(f"{COLOR_RED}{COLOR_BOLD}[ERROR] Failed to open serial port {port_name}: {e}{COLOR_RESET}")
        print(f"Please check that no other serial terminal app is currently holding {port_name} open.")
        sys.exit(1)

    stop_event = threading.Event()
    reader_thread = threading.Thread(target=read_serial_loop, args=(ser, args.log, stop_event), daemon=True)
    reader_thread.start()

    try:
        while True:
            cmd = input().strip()
            if cmd.lower() in ["exit", "quit", "q"]:
                print(f"{COLOR_YELLOW}[TERMINAL] Disconnecting and exiting...{COLOR_RESET}")
                break
            elif cmd:
                json_cmd = f'{{"cmd":"{cmd}"}}\r\n'
                ser.write(json_cmd.encode("utf-8"))
                ser.flush()
                print(f"{COLOR_YELLOW}[COMMAND TRANSMITTED] -> {json_cmd.strip()}{COLOR_RESET}")
    except KeyboardInterrupt:
        print(f"\n{COLOR_YELLOW}[TERMINAL] Interrupted by user. Exiting...{COLOR_RESET}")
    finally:
        stop_event.set()
        time.sleep(0.2)
        if ser.is_open:
            ser.close()
        print(f"{COLOR_GREEN}[CLOSED] Serial port {port_name} closed cleanly.{COLOR_RESET}\n")


if __name__ == "__main__":
    main()
