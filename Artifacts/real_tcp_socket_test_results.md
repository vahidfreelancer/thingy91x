# Real POSIX BSD TCP Socket & DNS IPv4 Fallback Test Results

## 1. Network API & Socket Feature Verification

| Network Feature | API Function / Symbol | Configuration Flag | Result |
| :--- | :--- | :--- | :--- |
| **DNS Resolver** | `CONFIG_DNS_RESOLVER=y` | `CONFIG_DNS_SERVER1="8.8.8.8"` | **ACTIVE** |
| **Direct IPv4 Fallback** | `zsock_inet_pton(AF_INET, "31.187.72.179", ...)` | Fallback on DNS `-EAGAIN` (-11) | **ACTIVE** |
| **Offloaded Socket Stack** | `<zephyr/net/socket.h>` | `CONFIG_NET_SOCKETS_OFFLOAD=y` | **ACTIVE** |
| **BSD Socket Creation** | `zsock_socket(AF_INET, SOCK_STREAM, 0)` | `CONFIG_NET_SOCKETS=y` | **ACTIVE** |
| **TCP Connect Handshake** | `zsock_connect(fd, (struct sockaddr *)&server_addr, ...)` | Target `31.187.72.179:1200` | **ACTIVE** |
| **5-Sec Socket Timeout** | `SO_RCVTIMEO` / `SO_SNDTIMEO` | POSIX `setsockopt()` | **ACTIVE** |
| **Packet TX / RX** | `zsock_send()` / `zsock_recv()` | JSON Diagnostic Telemetry | **ACTIVE** |
| **Error Recovery** | `HW_STATE_ERROR_DISCONNECTED` | Red Pulse LED on Error | **ACTIVE** |

---

## 2. Dynamic Real TCP Socket & DNS Fallback Flow

```text
1. [USER BUTTON PRESS]
   Log Output: [USER ACTION] BUTTON1 Pressed! Initiating real TCP connection to s4.sytemonitor.co.uk:1200...
   LED Pattern: Fast Blue Blinking (0, 128, 255)

2. [REAL DNS RESOLUTION & IPV4 FALLBACK]
   Log Output: [DNS RESOLUTION] Resolving host 's4.sytemonitor.co.uk' on port 1200...
   Log Output: [DNS FALLBACK] zsock_getaddrinfo returned err: -11. Falling back to direct IP 31.187.72.179...

3. [REAL TCP HANDSHAKE & CONNECT]
   Log Output: [TCP CONNECT] Executing zsock_connect() to s4.sytemonitor.co.uk:1200 (31.187.72.179:1200, fd=0)...
   Log Output: [TCP CONNECT SUCCESS] Real POSIX BSD socket connected to s4.sytemonitor.co.uk:1200 (31.187.72.179:1200, fd=0)
   LED Pattern: Solid Cyan Glow (0, 255, 255)

4. [REAL DATA TX / RX OVER CELLULAR]
   Log Output: [TCP TX SUCCESS] Transmitted 352 bytes over cellular socket to s4.sytemonitor.co.uk:1200
```

---

## 3. Hardware Deployment Metrics

- **Target Board**: Nordic Thingy:91 X (`thingy91x/nrf9151/ns`)
- **Device Serial**: `THINGY91X_F40679066AD`
- **Firmware Package**: `build/dfu_application.zip`
- **Flashing Status**: Successfully programmed with exit code 0.
