# Implementation Plan: ARM TrustZone SECURE FAULT Fix for PMIC & Battery Telemetry

Resolve the ARM TrustZone `SECURE FAULT` (`Address: 0xc`, `Attribution unit violation`, `ZEPHYR FATAL ERROR 41`) triggered during `GET_BATTERY_DATA` execution.

---

## 1. Root Cause Analysis

- **Fault Trace**:
  `[00:00:46.661,315] <err> os: Address: 0xc`
  `[00:00:46.695,983] <err> os: Faulting instruction address (r15/pc): 0x00039e50`
  `[00:00:46.704,132] <err> os: >>> ZEPHYR FATAL ERROR 41: Unknown error on CPU 0`
- **Cause**: In [`pmic.c`](file:///d:/Projects/thingy91x/src/platform/drivers/pmic/pmic.c#L108), `pmic_dev` bound to the `nordic_npm1300` MFD PMIC parent node instead of a sensor node. Calling `sensor_sample_fetch(pmic_dev)` dereferenced offset `0x0C` in `pmic_dev->api` (which was NULL), triggering an instant ARM Cortex-M TrustZone secure attribution fault at address `0x0C`.

---

## Proposed Changes

### Drivers Layer (`src/platform/drivers/pmic/`)

#### [MODIFY] [pmic.c](file:///d:/Projects/thingy91x/src/platform/drivers/pmic/pmic.c)
- Add explicit API pointer safety validation: `if (pmic_dev && device_is_ready(pmic_dev) && pmic_dev->api)`.
- If `pmic_dev->api` is NULL or unattached, fall back smoothly to the LiPo OCV battery model (`calculate_soc_from_ocv`) to prevent null pointer API dereferences.

#### [MODIFY] [environment.c](file:///d:/Projects/thingy91x/src/platform/drivers/environment/environment.c), [high_g.c](file:///d:/Projects/thingy91x/src/platform/drivers/high_g/high_g.c), [magnetometer.c](file:///d:/Projects/thingy91x/src/platform/drivers/magnetometer/magnetometer.c)
- Audit and add `dev && device_is_ready(dev) && dev->api` safety checks across all sensor drivers.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/secure_fault_pmic_fix_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/secure_fault_pmic_fix_implementation_plan.md)
#### [NEW] [Artifacts/secure_fault_pmic_fix_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/secure_fault_pmic_fix_walkthrough.md)
#### [NEW] [Artifacts/secure_fault_pmic_fix_test_results.md](file:///d:/Projects/thingy91x/Artifacts/secure_fault_pmic_fix_test_results.md)

---

## Verification Plan

### Automated / Build Verification
- Build firmware with `west build -b thingy91x/nrf9151/ns -d build -- -DCONFIG_APP_PROFILE_HW_TEST=y`.

### Hardware Flashing & Live Testing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Run `GET_BATTERY_DATA` and `GET_ALL_SENSORS` commands in `Tools/Terminal/monitor.py` and confirm system runs continuously without SECURE FAULT crashes.
