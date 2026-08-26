# Cellular Modem & APN Network Configuration Test Results

## 1. Cellular Stack Kconfig & Build Verification

| Feature | Kconfig Flag | Status | Description |
| :--- | :--- | :--- | :--- |
| **Modem Driver Library** | `CONFIG_NRF_MODEM_LIB=y` | **ENABLED** | nRF9151 cellular modem library support |
| **LTE Link Control** | `CONFIG_LTE_LINK_CONTROL=y` | **ENABLED** | Auto network registration & LTE-M attach |
| **LTE Mode** | `CONFIG_LTE_NETWORK_MODE_LTE_M=y` | **ENABLED** | LTE-M mode selection for IoT roaming |
| **POSIX Sockets** | `CONFIG_NET_SOCKETS_OFFLOAD=y` | **ENABLED** | BSD socket offload to nRF9151 modem core |
| **PDN & APN** | `CONFIG_PDN=y` / `CONFIG_PDN_DEFAULT_APN=""` | **CONFIGURED** | Dynamic auto APN query & custom APN override |

---

## 2. MCUboot DFU Deployment

- **Target Device**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`)
- **Firmware Package**: `build/dfu_application.zip`
- **Flashing Status**: Successfully programmed with exit code 0.
