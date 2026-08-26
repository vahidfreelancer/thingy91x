# Walkthrough & Test Report: PMIC & Battery Fuel Gauge Driver

The **PMIC & Battery Fuel Gauge Driver** (nPM1300 / MAX17048 / ADP5360) has been implemented with Open-Circuit Voltage (OCV) State-of-Charge (SoC %) interpolation mathematics, power calculation ($P = V \times I$), low-battery threshold detection, compiled, and programmed to the connected **Nordic Thingy:91 X** hardware.

---

## 1. Mathematical Formulas & Driver Logic

- **State of Charge (SoC %) OCV Curve Mapping**:
  Piecewise linear interpolation converting cell voltage $V_{\text{mV}}$ to SoC percentage:
  - $V \ge 4200\text{ mV} \Rightarrow 100\%$
  - $4000\text{ mV} \le V < 4200\text{ mV} \Rightarrow 80\% + \frac{V - 4000}{200} \times 20\%$
  - $3800\text{ mV} \le V < 4000\text{ mV} \Rightarrow 40\% + \frac{V - 3800}{200} \times 40\%$
  - $3600\text{ mV} \le V < 3800\text{ mV} \Rightarrow 10\% + \frac{V - 3600}{200} \times 30\%$
  - $3000\text{ mV} \le V < 3600\text{ mV} \Rightarrow \frac{V - 3000}{600} \times 10\%$
  - $V < 3000\text{ mV} \Rightarrow 0\%$
- **Power Calculation**:
  \[
  P_{\text{mW}} = \frac{V_{\text{mV}} \times |I_{\text{mA}}|}{1000.0}
  \]
- **Remaining Battery Capacity**:
  \[
  E_{\text{rem\_mAh}} = C_{\text{nominal\_mAh}} \times \left(\frac{\text{SoC}}{100}\right) \quad (C_{\text{nominal}} = 1400\text{ mAh})
  \]

---

## 2. File & Driver Structure

- **Driver Header**: [`pmic.h`](file:///d:/Projects/thingy91x/src/platform/drivers/pmic/pmic.h)
  - Exposes `struct pmic_battery_data`, `enum pmic_charge_status`, `pmic_init()`, `pmic_read()`, `pmic_set_rail_state()`, `pmic_sleep()`.
- **Driver Implementation**: [`pmic.c`](file:///d:/Projects/thingy91x/src/platform/drivers/pmic/pmic.c)
  - Devicetree compatible targets (`nordic,npm1300`, `maxim,max17048`, `adi,adp5360`), Zephyr `sensor.h` voltage/current conversion, power math, POSIX error return codes, and software simulation fallback.
- **Board Overlay**: [`thingy91x_nrf9151_ns.overlay`](file:///d:/Projects/thingy91x/boards/thingy91x_nrf9151_ns.overlay#L35-L40)
  - Configured `npm1300` devicetree node under `&i2c2`.
- **Application Profile Integration**: [`app.c`](file:///d:/Projects/thingy91x/src/app/asset_tracker/app.c#L30-L45)
  - Integrated PMIC battery sampling into `asset_tracker` profile.

---

## 3. Deployment & Test Results

- **Target Hardware**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **DFU Package Flashed**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/pmic_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/pmic_test_results.md)
