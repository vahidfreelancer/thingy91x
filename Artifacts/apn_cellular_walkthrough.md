# Walkthrough: Cellular Modem & APN Network Configuration

The nRF Connect SDK cellular modem network stack (`nrf_modem_lib`, `lte_lc`, `net_sockets_offload`, `pdn`) has been enabled and configured in [`prj.conf`](file:///d:/Projects/thingy91x/prj.conf) for the **Nordic Thingy:91 X (PCA20065)** running the **Hardware Diagnostic & Remote Test Suite Profile** (`APP_PROFILE_HW_TEST`).

---

## 1. APN Discovery & Configuration Protocol

1. **Automatic SIM APN Query**:
   - When `CONFIG_PDN_DEFAULT_APN=""` is set, the nRF9151 modem automatically retrieves the SIM card operator's APN credentials (e.g. `soracom.io`, `hologram`, `em`, `1nce.net`, `iot.vodafone.com`).
2. **Custom APN Override**:
   - To force a specific APN for private M2M or roaming SIM cards, update [`prj.conf`](file:///d:/Projects/thingy91x/prj.conf#L39):
     ```ini
     CONFIG_PDN_DEFAULT_APN="<your_apn>"
     ```

---

## 2. Hardware Deployment & Build Metrics

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Package**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/apn_cellular_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/apn_cellular_test_results.md)
