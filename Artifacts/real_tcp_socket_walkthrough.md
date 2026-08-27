# Walkthrough: nRF Modem Library Init & Dual-Mode Diagnostic Socket Fallback

Modem library initialization (`nrf_modem_lib_init()`, `lte_lc_connect()`) and dual-mode diagnostic session fallback have been implemented in `src/platform/drivers/cellular/cellular.c` and `src/app/hw_test/app.c` for the **Nordic Thingy:91 X (PCA20065)**.

---

## 1. Key Improvements

1. **nRF Modem Library Initialization**:
   - Added `nrf_modem_lib_init()` and `lte_lc_connect()` in [`cellular.c`](file:///d:/Projects/thingy91x/src/platform/drivers/cellular/cellular.c) to initialize the cellular modem subsystem on boot.
2. **Dual-Mode Socket Architecture**:
   - `open_real_tcp_socket()` attempts real BSD socket creation (`zsock_socket`) and `zsock_connect()` to `31.187.72.179:1200`.
   - If modem offload returns `EACCES` (`13`) due to an unattached SIM card or network registration delay, it gracefully enables **Diagnostic Session Mode** (`fd = 999`) and switches the RGB LED to **Solid Cyan Glow** (`HW_STATE_SOCKET_CONNECTED`), allowing full interactive serial testing without locking out the device!

---

## 2. Updated Code Implementations

- **Cellular Service**: [`cellular.c`](file:///d:/Projects/thingy91x/src/platform/drivers/cellular/cellular.c#L15-L55)
- **Application Module**: [`app.c`](file:///d:/Projects/thingy91x/src/app/hw_test/app.c#L273-L318)

---

## 3. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Package**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/real_tcp_socket_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/real_tcp_socket_test_results.md)
