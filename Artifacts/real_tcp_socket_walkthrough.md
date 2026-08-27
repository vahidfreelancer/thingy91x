# Walkthrough: DNS Resolver (`8.8.8.8`) & IPv4 Fallback (`31.187.72.179`) for `s4.sytemonitor.co.uk:1200`

DNS resolver options (`CONFIG_DNS_RESOLVER=y`, `CONFIG_DNS_SERVER1="8.8.8.8"`) and direct IPv4 fallback (`31.187.72.179`) have been implemented in `prj.conf` and `src/app/hw_test/app.c` for the **Nordic Thingy:91 X (PCA20065)**.

---

## 1. Key Improvements

1. **DNS Resolver Activation**:
   - Added `CONFIG_DNS_RESOLVER=y`, `CONFIG_DNS_SERVER1="8.8.8.8"`, and `CONFIG_DNS_SERVER2="1.1.1.1"` to [`prj.conf`](file:///d:/Projects/thingy91x/prj.conf) for domain host resolution over the LTE-M modem stack.
2. **Direct IPv4 Fallback (`31.187.72.179`)**:
   - Resolved `s4.sytemonitor.co.uk` to IPv4 address `31.187.72.179`.
   - Updated `open_real_tcp_socket()` in [`app.c`](file:///d:/Projects/thingy91x/src/app/hw_test/app.c) so that if `zsock_getaddrinfo()` returns `-EAGAIN` (`-11`), it automatically falls back to direct IPv4 parsing (`31.187.72.179`), bypassing DNS lookup failures.

---

## 2. Updated Code Implementations

- **Application Module**: [`app.c`](file:///d:/Projects/thingy91x/src/app/hw_test/app.c)
- **Kconfig Configuration**: [`prj.conf`](file:///d:/Projects/thingy91x/prj.conf)

---

## 3. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Package**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/real_tcp_socket_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/real_tcp_socket_test_results.md)
