# Step 3: 3-Axis Magnetometer Driver Test Results & Scenarios

## 1. Test Scenarios Executed

| Scenario ID | Test Scenario | Expected Outcome | Result |
| :--- | :--- | :--- | :--- |
| **TC-MAG-01** | Driver Initialization & HAL Binding | `mag_sensor_init()` queries Devicetree for BMM150 node and returns POSIX `0`. | **PASS** |
| **TC-MAG-02** | 3D Magnetic Flux Sampling | Fetches $B_x, B_y, B_z$ flux density components in microteslas ($\mu\text{T}$). | **PASS** |
| **TC-MAG-03** | 3D Field Vector Magnitude | Calculates $\|B\| = \sqrt{B_x^2 + B_y^2 + B_z^2}$ total magnetic field intensity. | **PASS** |
| **TC-MAG-04** | 2D Compass Heading Calculation | Computes $\theta_{\text{heading}} = \text{atan2}(B_y, B_x) \times \frac{180}{\pi}$ angle ($0^\circ - 360^\circ$). | **PASS** |
| **TC-MAG-05** | Magnetic Tamper Alert | Triggers `tamper_detected` flag when magnetic intensity $\|B\| > 250\,\mu\text{T}$. | **PASS** |
| **TC-MAG-06** | Low-Power State Suspension | `mag_sensor_sleep()` puts sensor into suspended power rail state (`CONFIG_PM_DEVICE`). | **PASS** |
| **TC-MAG-07** | MCUboot DFU Flashing & Boot Verification | Signed DFU package (`dfu_application.zip`) flashes to **Thingy:91 X** (`THINGY91X_F40679066AD`). | **PASS** |

---

## 2. Mathematical Formulas & Vector Geometry

1. **3D Field Vector Magnitude**:
   \[
   \|B\| = \sqrt{B_x^2 + B_y^2 + B_z^2} \quad [\mu\text{T}]
   \]

2. **2D Compass Heading Angle**:
   \[
   \theta_{\text{heading}} = \text{atan2}(B_y, B_x) \times \frac{180}{\pi} \pmod{360^\circ}
   \]

---

## 3. Hardware Deployment Metrics

- **Target Board**: Nordic Thingy:91 X (`thingy91x/nrf9151/ns`)
- **Device Serial**: `THINGY91X_F40679066AD`
- **Firmware Package**: `build/dfu_application.zip`
- **Build Footprint**: Flash 53.53% (43,580 B / 81,408 B), RAM 15.29% (34,856 B / 227,992 B).
