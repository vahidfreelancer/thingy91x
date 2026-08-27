# Walkthrough: ARM TrustZone SECURE FAULT Fix for PMIC & Battery Telemetry

Explicit `pmic_dev->api` safety guards and LiPo OCV battery calculations have been implemented in `src/platform/drivers/pmic/pmic.c` to prevent ARM Cortex-M TrustZone `SECURE FAULT` crashes on the **Nordic Thingy:91 X (PCA20065)**.

---

## 1. Resolved Crash Analysis

- **Fault Trace**:
  `[00:00:46.661,315] <err> os: Address: 0xc`
  `[00:00:46.695,983] <err> os: Faulting instruction address (r15/pc): 0x00039e50`
  `[00:00:46.704,132] <err> os: >>> ZEPHYR FATAL ERROR 41: Unknown error on CPU 0`
- **Root Cause**:
  Calling `sensor_sample_fetch(pmic_dev)` when `pmic_dev` was bound to a non-sensor MFD device node dereferenced offset `0x0C` in `pmic_dev->api` (NULL), causing an instant ARM TrustZone secure attribution fault at address `0x0C`.
- **Fix Implemented**:
  Updated `pmic_init()` and `pmic_read()` to validate `if (pmic_dev && device_is_ready(pmic_dev) && pmic_dev->api)`. If `pmic_dev->api` is NULL, the driver falls back to safe LiPo OCV battery calculations (`calculate_soc_from_ocv`), eliminating SECURE FAULT crashes.

---

## 2. Updated Code Implementations

- **PMIC Driver**: [`pmic.c`](file:///d:/Projects/thingy91x/src/platform/drivers/pmic/pmic.c#L53-L105)

---

## 3. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Package**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/secure_fault_pmic_fix_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/secure_fault_pmic_fix_test_results.md)
