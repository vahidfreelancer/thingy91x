# Implementation Plan: Real POSIX BSD TCP Socket Connection to `s4.sytemonitor.co.uk:1200`

Replace the offline test socket simulation in `src/app/hw_test/app.c` with **Real Zephyr POSIX BSD Sockets** (`zsock_getaddrinfo()`, `zsock_socket()`, `zsock_connect()`, `zsock_send()`, `zsock_recv()`) connected over cellular LTE-M to `s4.sytemonitor.co.uk:1200`.

---

## 1. Architectural Changes

- **DNS Host Name Resolution**:
  Use `zsock_getaddrinfo("s4.sytemonitor.co.uk", "1200", &hints, &res)` over the cellular offloaded modem stack (`CONFIG_NET_SOCKETS_OFFLOAD=y`).
- **Non-blocking / Timed Socket Connection**:
  Configure socket timeout options `SO_RCVTIMEO` and `SO_SNDTIMEO` (5-second timeout).
- **Live Bidirectional Data Exchange**:
  - Transmit serialized sensor telemetry JSON payloads over cellular socket to `s4.sytemonitor.co.uk:1200` via `zsock_send()`.
  - Receive remote server JSON diagnostic commands via `zsock_recv()`.

---

## Proposed Changes

### Application Layer (`src/app/hw_test/`)

#### [MODIFY] [app.c](file:///d:/Projects/thingy91x/src/app/hw_test/app.c)
- Include `<zephyr/net/socket.h>`.
- Replace simulated socket logic with real BSD socket creation, DNS resolution, connection establishing, and packet TX/RX.
- If DNS resolution or TCP connection fails (e.g. server down or port closed), transition to **Slow Red Pulse** (`HW_STATE_ERROR_DISCONNECTED`) and log the exact POSIX error code (`-ETIMEDOUT`, `-ECONNREFUSED`, `-EHOSTUNREACH`).

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/real_tcp_socket_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/real_tcp_socket_implementation_plan.md)
#### [NEW] [Artifacts/real_tcp_socket_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/real_tcp_socket_walkthrough.md)
#### [NEW] [Artifacts/real_tcp_socket_test_results.md](file:///d:/Projects/thingy91x/Artifacts/real_tcp_socket_test_results.md)

---

## Verification Plan

### Automated Build Verification
- Build firmware with `west build -b thingy91x/nrf9151/ns -d build -- -DCONFIG_APP_PROFILE_HW_TEST=y`.

### Hardware Deployment & Live Connection Test
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`).
- Run `Tools/Terminal/monitor.py` and press BUTTON1 to trigger real DNS resolution and TCP socket handshake to `s4.sytemonitor.co.uk:1200`.
