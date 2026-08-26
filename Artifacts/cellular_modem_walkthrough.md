# Walkthrough: Step 5 - Cellular Modem Controller Driver (nRF9151)

The **Cellular Modem Controller Driver** (nRF9151 SiP) has been implemented supporting LTE-M / NB-IoT / DECT NR+ network attachment, live signal quality metrics (RSRP, RSRQ, SNR, Cell ID), PLMN network operator metadata (Operator Name, Band, TAC, IP Address), neighboring cellular station scanning for triangulation, PSM / eDRX low-power timers, flight mode sleep suspension, compiled, and programmed to the connected **Nordic Thingy:91 X** hardware.

---

## 1. Implemented Features & Driver HAL

- **Driver Header**: [`cellular.h`](file:///d:/Projects/thingy91x/src/platform/drivers/cellular/cellular.h)
  - Exposes `enum cellular_modem_mode`, `struct cellular_signal_info`, `struct cellular_network_metadata`, `struct cellular_neighbor_scan`, `cellular_modem_init()`, `cellular_modem_connect()`, `cellular_modem_get_signal_info()`, `cellular_modem_get_network_metadata()`, `cellular_modem_scan_neighbor_cells()`, `cellular_modem_set_psm_edrx()`, `cellular_modem_sleep()`.
- **Driver Logic**: [`cellular.c`](file:///d:/Projects/thingy91x/src/platform/drivers/cellular/cellular.c)
  - Handles cellular network attachment, signal metrics retrieval, operator metadata parsing, neighboring cell station scanning, PSM / eDRX timers, and low-power flight mode sleep yielding.
- **Application Integration**: [`app.c`](file:///d:/Projects/thingy91x/src/app/asset_tracker/app.c#L35-L51)
  - `app_init()` initializes nRF9151 modem driver and connects to LTE-M; samples signal metrics, metadata, and neighbor cell stations periodically inside `telemetry_work_handler()`.

---

## 2. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Flashed**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/cellular_modem_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/cellular_modem_test_results.md)
