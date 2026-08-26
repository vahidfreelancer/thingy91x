# PMIC & Battery Fuel Gauge Driver Test Results & Scenarios

## 1. Test Scenarios Executed

| Scenario ID | Test Scenario | Expected Outcome | Result |
| :--- | :--- | :--- | :--- |
| **TC-PMIC-01** | Device Initialization & Driver HAL Binding | Driver locates PMIC DTS node (`nordic,npm1300` / `maxim,max17048`) or registers simulation fallback. Returns POSIX `0`. | **PASS** |
| **TC-PMIC-02** | OCV Voltage-to-SoC % Interpolation | Linear piecewise curve mapping: 4200mV = 100%, 4000mV = 80%, 3800mV = 40%, 3600mV = 10%, 3000mV = 0%. | **PASS** |
| **TC-PMIC-03** | Instantaneous Power Calculation ($P = V \times I$) | Correctly calculates power draw $P = V_{\text{mV}} \times |I_{\text{mA}}| / 1000.0$ in mW. | **PASS** |
| **TC-PMIC-04** | Remaining Capacity Estimation ($E_{\text{rem}}$) | Evaluates $E_{\text{rem}} = C_{\text{nominal}} \times (\text{SoC} / 100)$ based on nominal 1400 mAh LiPo capacity. | **PASS** |
| **TC-PMIC-05** | Low Battery Alert Trigger | Activates `low_battery_alert = true` when State-of-Charge falls below 15.0%. | **PASS** |
| **TC-PMIC-06** | MCUboot DFU Deployment & Flashing | Programs signed DFU package (`dfu_application.zip`) to **Thingy:91 X** (`THINGY91X_F40679066AD`). | **PASS** |

---

## 2. Mathematical Formulas Verified

1. **Piecewise Linear OCV to SoC Interpolation**:
   \[
   \text{SoC}(V) =
   \begin{cases}
   100.0\% & V \ge 4200\text{ mV} \\
   80.0 + \frac{V - 4000}{200} \times 20.0 & 4000\text{ mV} \le V < 4200\text{ mV} \\
   40.0 + \frac{V - 3800}{200} \times 40.0 & 3800\text{ mV} \le V < 4000\text{ mV} \\
   10.0 + \frac{V - 3600}{200} \times 30.0 & 3600\text{ mV} \le V < 3800\text{ mV} \\
   \frac{V - 3000}{600} \times 10.0 & 3000\text{ mV} \le V < 3600\text{ mV} \\
   0.0\% & V < 3000\text{ mV}
   \end{cases}
   \]

2. **Instantaneous Power Calculation**:
   \[
   P_{\text{mW}} = \frac{V_{\text{mV}} \times |I_{\text{mA}}|}{1000.0}
   \]

3. **Remaining Battery Energy Capacity**:
   \[
   E_{\text{rem\_mAh}} = C_{\text{nominal\_mAh}} \times \left(\frac{\text{SoC}}{100}\right)
   \]

---

## 3. Deployment Summary

- **Target Board**: Nordic Thingy:91 X (`thingy91x/nrf9151/ns`)
- **Device Serial**: `THINGY91X_F40679066AD`
- **Firmware Package**: `build/dfu_application.zip`
- **MCUboot Compatibility**: Enabled (`CONFIG_BOOTLOADER_MCUBOOT=y`)
- **Build Footprint**: Flash 53.53% (43,580 B / 81,408 B), RAM 15.29% (34,856 B / 227,992 B).
