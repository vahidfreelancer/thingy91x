# Implementation Plan: Step 5 - Cellular Modem Controller Driver (nRF9151)

Implement a high-level Cellular Modem Controller Driver (`src/platform/drivers/cellular/`) for the integrated **nRF9151 LTE-M / NB-IoT / DECT NR+ SiP modem** on the Nordic Thingy:91 X platform to manage cellular network registration, signal quality reporting (RSRP, RSRQ, SNR, Cell ID), Power Saving Mode (PSM / eDRX) timer configurations, and low-power flight mode sleep yielding.

## Proposed Changes

### Hardware Abstraction Layer (HAL) - Drivers

#### [NEW] [cellular.h](file:///d:/Projects/thingy91x/src/platform/drivers/cellular/cellular.h)
- Declare `enum cellular_modem_mode`: `CELLULAR_MODE_OFFLINE`, `CELLULAR_MODE_LTE_M`, `CELLULAR_MODE_NB_IOT`, `CELLULAR_MODE_DECT_NR_PLUS`.
- Declare `struct cellular_signal_info`:
  - `rsrp_dbm`: Reference Signal Received Power (-140 to -44 dBm)
  - `rsrq_db`: Reference Signal Received Quality (-20 to -3 dB)
  - `snr_db`: Signal-to-Noise Ratio (0 to 30 dB)
  - `cell_id`: 28-bit Cell ID
  - `mcc`: Mobile Country Code
  - `mnc`: Mobile Network Code
  - `registered`: Network attachment status boolean
  - `valid`: Signal reading status boolean
- Expose interface functions:
  - `int cellular_modem_init(void)`: Initialize nRF Modem library subsystem and AT interface.
  - `int cellular_modem_connect(enum cellular_modem_mode mode)`: Connect to LTE-M / NB-IoT network.
  - `int cellular_modem_get_signal_info(struct cellular_signal_info *info)`: Retrieve network registration state and RSRP/RSRQ/SNR metrics.
  - `int cellular_modem_set_psm_edrx(bool enable_psm, uint32_t psm_sec, bool enable_edrx)`: Configure PSM / eDRX low-power timers.
  - `int cellular_modem_sleep(void)`: Put modem into flight mode / low-power sleep state.

#### [NEW] [cellular.c](file:///d:/Projects/thingy91x/src/platform/drivers/cellular/cellular.c)
- Implement driver logic using Zephyr / nRF Modem APIs (`nrf_modem_at`, `nrf_modem_lib`), Devicetree macros, AT notification handling, signal quality parsing, POSIX error return codes, and software simulation fallback for off-target validation.

---

### Application Integration & Build System

#### [MODIFY] [CMakeLists.txt](file:///d:/Projects/thingy91x/src/app/asset_tracker/CMakeLists.txt)
- Include `../../platform/drivers/cellular/cellular.c` and `../../platform/drivers/cellular` in `asset_tracker/CMakeLists.txt`.

#### [MODIFY] [app.c](file:///d:/Projects/thingy91x/src/app/asset_tracker/app.c)
- Call `cellular_modem_init()` during boot, connect to LTE-M, sample RSRP/RSRQ/SNR signal metrics, and demonstrate PSM power saving inside `telemetry_work_handler()`.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/cellular_modem_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/cellular_modem_implementation_plan.md)
#### [NEW] [Artifacts/cellular_modem_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/cellular_modem_walkthrough.md)
#### [NEW] [Artifacts/cellular_modem_test_results.md](file:///d:/Projects/thingy91x/Artifacts/cellular_modem_test_results.md)
- Save design plans, execution walkthrough, and hardware test scenario results in `Artifacts/`.

---

## Verification Plan

### Automated / Build Verification
- Compile firmware using nRF Connect SDK toolchain (`west build -b thingy91x/nrf9151/ns -- -DCONFIG_APP_PROFILE_ASSET_TRACKER=y`) with `CONFIG_BOOTLOADER_MCUBOOT=y`.

### Hardware Flashing & Live Testing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Monitor live serial console logs to verify cellular modem initialization, LTE-M connection status, RSRP/RSRQ/SNR signal metrics, and PSM low-power configuration.
