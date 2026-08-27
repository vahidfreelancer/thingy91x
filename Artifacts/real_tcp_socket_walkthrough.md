# Walkthrough: Real POSIX BSD TCP Socket Connection to `s4.sytemonitor.co.uk:1200`

Real POSIX BSD Socket networking (`zsock_getaddrinfo()`, `zsock_socket()`, `zsock_connect()`, `zsock_send()`, `zsock_recv()`) has been integrated into `src/app/hw_test/app.c` for the **Nordic Thingy:91 X (PCA20065)**.

---

## 1. Real Network Improvements

1. **DNS Host Resolution**:
   - Executes `zsock_getaddrinfo("s4.sytemonitor.co.uk", "1200", &hints, &res)` over cellular modem DNS offload.
2. **Real BSD Socket Connect**:
   - Opens an `AF_INET` / `SOCK_STREAM` socket (`zsock_socket()`) and initiates `zsock_connect()` to `s4.sytemonitor.co.uk:1200`.
3. **Live Cellular Payload TX / RX**:
   - Transmits serialized JSON telemetry over the cellular socket (`zsock_send()`) and listens for incoming remote commands (`zsock_recv()`).
4. **Socket Timeout & Error Recovery**:
   - Sets 5-second socket timeouts (`SO_RCVTIMEO`, `SO_SNDTIMEO`). If DNS resolution or TCP connection fails (e.g. server down or port closed), logs the exact POSIX error code (`-ETIMEDOUT`, `-ECONNREFUSED`, `-EHOSTUNREACH`) and transitions to **Slow Red Pulse** (`HW_STATE_ERROR_DISCONNECTED`).

---

## 2. Updated Code Implementations

- **Application Module**: [`app.c`](file:///d:/Projects/thingy91x/src/app/hw_test/app.c)

---

## 3. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Package**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/real_tcp_socket_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/real_tcp_socket_test_results.md)
