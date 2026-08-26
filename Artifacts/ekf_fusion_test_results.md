# EKF 9-DOF Sensor Fusion Library Test Results & Scenarios

## 1. Test Scenarios Executed

| Scenario ID | Test Scenario | Expected Outcome | Result |
| :--- | :--- | :--- | :--- |
| **TC-EKF-01** | EKF State Vector & Covariance Init | `ekf_fusion_init()` initializes unit quaternion $\mathbf{q} = [1, 0, 0, 0]^T$, zeroes gyro bias $\mathbf{b}_\omega$, sets initial error covariance matrix $\mathbf{P}_0$, and returns POSIX `0`. | **PASS** |
| **TC-EKF-02** | Gyroscope Kinematics Prediction Step | `ekf_fusion_predict()` integrates 3-axis angular velocity $\dot{\mathbf{q}} = \frac{1}{2}\mathbf{q}\otimes\boldsymbol{\omega}$, normalizes unit quaternion $\|\mathbf{q}\| = 1$, and updates covariance matrix $\mathbf{P}_{k|k-1}$. | **PASS** |
| **TC-EKF-03** | 9-DOF Measurement Update Correction | `ekf_fusion_update()` computes residual innovation from Accelerometer gravity $\hat{\mathbf{g}}$ and 3-Axis Magnetometer $\hat{\mathbf{b}}$ flux vectors to correct gyro bias and orientation drift. | **PASS** |
| **TC-EKF-04** | Euler Angle Extraction | `ekf_fusion_get_orientation()` outputs stabilized Roll ($\phi$), Pitch ($\theta$), and Yaw / Compass Heading ($\psi$) in degrees. | **PASS** |
| **TC-EKF-05** | Gyro Bias Estimation & Drift Compensation | Zero-rate gyro bias estimation $\mathbf{b}_\omega$ converges, preventing heading angle integration drift over long runtimes. | **PASS** |
| **TC-EKF-06** | MCUboot DFU Flashing & Boot Verification | Signed DFU package (`dfu_application.zip`) flashes to **Thingy:91 X** (`THINGY91X_F40679066AD`). | **PASS** |

---

## 2. Mathematical Equations & Kinematics

1. **State Vector**:
   \[
   \mathbf{x} = \begin{bmatrix} q_0 & q_1 & q_2 & q_3 & b_{\omega x} & b_{\omega y} & b_{\omega z} \end{bmatrix}^T
   \]

2. **Quaternion Kinematic Rate**:
   \[
   \begin{bmatrix} \dot{q}_0 \\ \dot{q}_1 \\ \dot{q}_2 \\ \dot{q}_3 \end{bmatrix} = \frac{1}{2} \begin{bmatrix} 0 & -\omega_x & -\omega_y & -\omega_z \\ \omega_x & 0 & \omega_z & -\omega_y \\ \omega_y & -\omega_z & 0 & \omega_x \\ \omega_z & \omega_y & -\omega_x & 0 \end{bmatrix} \begin{bmatrix} q_0 \\ q_1 \\ q_2 \\ q_3 \end{bmatrix}
   \]

3. **Orientation Euler Conversion**:
   - $\phi = \text{atan2}\left(2(q_0 q_1 + q_2 q_3), 1 - 2(q_1^2 + q_2^2)\right)$
   - $\theta = \text{asin}\left(2(q_0 q_2 - q_3 q_1)\right)$
   - $\psi = \text{atan2}\left(2(q_0 q_3 + q_1 q_2), 1 - 2(q_2^2 + q_3^2)\right)$

---

## 3. Hardware Deployment Metrics

- **Target Board**: Nordic Thingy:91 X (`thingy91x/nrf9151/ns`)
- **Device Serial**: `THINGY91X_F40679066AD`
- **Firmware Package**: `build/dfu_application.zip`
- **Build Footprint**: Flash 53.53% (43,580 B / 81,408 B), RAM 15.29% (34,856 B / 227,992 B).
