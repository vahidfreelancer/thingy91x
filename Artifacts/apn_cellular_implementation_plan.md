# Implementation Plan: Cellular Modem & APN Network Configuration

Enable real cellular LTE-M / NB-IoT network registration and dynamic APN configuration on the **nRF9151 SiP** by configuring nRF Connect SDK network stack Kconfigs and adding custom APN options.

---

## 1. How Cellular APN (Access Point Name) Discovery Works

### Automatic Network APN Discovery
- The nRF9151 cellular modem automatically queries the network operator and SIM card for default APN credentials (e.g. `soracom.io`, `hologram`, `em`, `1nce.net`, `iot.vodafone.com`).

### Setting Custom APN via Kconfig / Runtime
- When using specialized M2M, private, or roaming IoT SIM cards, the APN can be explicitly set in `prj.conf`:
  ```ini
  CONFIG_PDN=y
  CONFIG_PDN_DEFAULTS=y
  CONFIG_PDN_DEFAULT_APN="<your_apn>"
  ```
- Common global IoT APNs:
  - **Soracom**: `soracom.io`
  - **Hologram**: `hologram`
  - **EMnify**: `em`
  - **1NCE**: `iot.1nce.net`
  - **Twilio**: `global`
  - **iBASIS**: `mnet.nu`
  - **Telenor IoT**: `telenor.iot`
  - **Vodafone IoT**: `iot.vodafone.com`

---

## Proposed Changes

### Configuration Layer (`prj.conf`)

#### [MODIFY] [prj.conf](file:///d:/Projects/thingy91x/prj.conf)
- Add nRF Modem Library (`CONFIG_NRF_MODEM_LIB=y`).
- Add LTE Link Control (`CONFIG_LTE_LINK_CONTROL=y`, `CONFIG_LTE_AUTO_INIT_AND_CONNECT=n`).
- Add BSD / POSIX Sockets API (`CONFIG_NETWORKING=y`, `CONFIG_NET_SOCKETS=y`, `CONFIG_NET_SOCKETS_OFFLOAD=y`).
- Add Packet Data Network (PDN) management and default APN settings (`CONFIG_PDN=y`, `CONFIG_PDN_DEFAULT_APN=""`).

---

### Application Profile & Driver Layer

#### [MODIFY] [cellular.c](file:///d:/Projects/thingy91x/src/platform/drivers/cellular/cellular.c)
- Initialize nRF modem library `nrf_modem_lib_init()`.
- Register LTE network callbacks via `lte_lc_init_and_connect_async()`.
- Implement dynamic AT command execution (`AT+CGDCONT=1,"IP","<apn>"`) for dynamic APN configuration.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/apn_cellular_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/apn_cellular_implementation_plan.md)
#### [NEW] [Artifacts/apn_cellular_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/apn_cellular_walkthrough.md)
#### [NEW] [Artifacts/apn_cellular_test_results.md](file:///d:/Projects/thingy91x/Artifacts/apn_cellular_test_results.md)

---

## Verification Plan

### Automated / Build Verification
- Build target firmware with `west build -b thingy91x/nrf9151/ns -d build -- -DCONFIG_APP_PROFILE_HW_TEST=y`.

### Hardware Deployment & Flashing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Monitor live network registration logs to confirm LTE-M / NB-IoT attach status and APN assignment.
