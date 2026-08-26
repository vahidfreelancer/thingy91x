# Implementation Plan: Extended Kalman Filter (EKF) 9-DOF Sensor Fusion Library

Implement a professional Extended Kalman Filter (EKF) AHRS (Attitude & Heading Reference System) sensor fusion library (`src/services/fusion/`) for the Nordic Thingy:91 X platform to fuse 6-Axis IMU (BMI270 Accelerometer + Gyroscope) and 3-Axis Magnetometer (BMM150) data into a stabilized 3D orientation quaternion ($\mathbf{q}$) and output Euler angles (Roll $\phi$, Pitch $\theta$, Yaw/Heading $\psi$).

## Mathematical & Architectural Design

### State Vector Representation ($\mathbf{x} \in \mathbb{R}^7$)
\[
\mathbf{x} = \begin{bmatrix} q_0 & q_1 & q_2 & q_3 & b_{\omega x} & b_{\omega y} & b_{\omega z} \end{bmatrix}^T
\]
- $\mathbf{q} = [q_0, q_1, q_2, q_3]^T$: Unit orientation quaternion (representing body to navigation frame rotation).
- $\mathbf{b}_\omega = [b_{\omega x}, b_{\omega y}, b_{\omega z}]^T$: Gyroscope zero-rate bias vector (rad/s).

### Process Kinematics & Prediction Step (Gyroscope Integration)
Given angular velocity $\boldsymbol{\omega} = [\omega_x - b_{\omega x}, \omega_y - b_{\omega y}, \omega_z - b_{\omega z}]^T$:
\[
\dot{\mathbf{q}} = \frac{1}{2} \mathbf{q} \otimes \begin{bmatrix} 0 \\ \boldsymbol{\omega} \end{bmatrix}
\]
\[
\mathbf{q}_{k|k-1} = \mathbf{q}_{k-1} + \dot{\mathbf{q}} \cdot \Delta t, \quad \mathbf{q} \leftarrow \frac{\mathbf{q}}{\|\mathbf{q}\|}
\]
\[
\mathbf{P}_{k|k-1} = \mathbf{F}_k \mathbf{P}_{k-1} \mathbf{F}_k^T + \mathbf{Q}_k
\]

### Measurement Update Step (Accelerometer + Magnetometer Fusion)
1. **Accelerometer Measurement**: Measures gravity direction $\hat{\mathbf{g}} = [0, 0, 1]^T$ in body frame.
2. **Magnetometer Measurement**: Measures earth magnetic field vector $\mathbf{m} = [m_x, m_y, m_z]^T$ in body frame to correct yaw/heading drift.
3. **Kalman Gain Computation**:
\[
\mathbf{K}_k = \mathbf{P}_{k|k-1} \mathbf{H}_k^T \left( \mathbf{H}_k \mathbf{P}_{k|k-1} \mathbf{H}_k^T + \mathbf{R}_k \right)^{-1}
\]
\[
\mathbf{x}_k = \mathbf{x}_{k|k-1} + \mathbf{K}_k \left( \mathbf{z}_k - h(\mathbf{x}_{k|k-1}) \right)
\]
\[
\mathbf{P}_k = (\mathbf{I} - \mathbf{K}_k \mathbf{H}_k) \mathbf{P}_{k|k-1}
\]

### Output Euler Angles ($\phi, \theta, \psi$)
- **Roll**: $\phi = \text{atan2}\left(2(q_0 q_1 + q_2 q_3), 1 - 2(q_1^2 + q_2^2)\right)$
- **Pitch**: $\theta = \text{asin}\left(2(q_0 q_2 - q_3 q_1)\right)$
- **Yaw / Compass Heading**: $\psi = \text{atan2}\left(2(q_0 q_3 + q_1 q_2), 1 - 2(q_2^2 + q_3^2)\right)$

---

## Proposed Changes

### Subsystems & Services (`src/services/fusion/`)

#### [NEW] [ekf_fusion.h](file:///d:/Projects/thingy91x/src/services/fusion/ekf_fusion.h)
- Declare `struct ekf_state`: `q[4]`, `gyro_bias[3]`, `roll_deg`, `pitch_deg`, `yaw_deg`, `cov_P[7][7]`.
- Expose interface functions:
  - `int ekf_fusion_init(struct ekf_state *state)`: Initialize EKF state vector, covariance matrices $\mathbf{P}_0, \mathbf{Q}, \mathbf{R}$.
  - `int ekf_fusion_predict(struct ekf_state *state, float gyro_x, float gyro_y, float gyro_z, float dt)`: Perform gyroscope quaternion integration step.
  - `int ekf_fusion_update(struct ekf_state *state, float accel_x, float accel_y, float accel_z, float mag_x, float mag_y, float mag_z)`: Perform 9-DOF measurement correction update.
  - `int ekf_fusion_get_orientation(const struct ekf_state *state, float *roll, float *pitch, float *yaw)`: Extract Euler orientation angles.

#### [NEW] [ekf_fusion.c](file:///d:/Projects/thingy91x/src/services/fusion/ekf_fusion.c)
- Implement EKF matrix operations, quaternion kinematics, Jacobians $\mathbf{F}, \mathbf{H}$, Kalman gain matrix inverse, normalization, and POSIX error handling.

---

### Application Integration & Build System

#### [MODIFY] [CMakeLists.txt](file:///d:/Projects/thingy91x/src/app/asset_tracker/CMakeLists.txt)
- Include `../../services/fusion/ekf_fusion.c` and `../../services/fusion` in `asset_tracker/CMakeLists.txt`.

#### [MODIFY] [app.c](file:///d:/Projects/thingy91x/src/app/asset_tracker/app.c)
- Initialize EKF fusion instance in `app_init()`, feed IMU + Magnetometer samples into `ekf_fusion_predict()` and `ekf_fusion_update()`, log estimated Roll, Pitch, and Yaw angles in `telemetry_work_handler()`.

---

### Documentation & Test Artifacts (`Artifacts/`)

#### [NEW] [Artifacts/ekf_fusion_implementation_plan.md](file:///d:/Projects/thingy91x/Artifacts/ekf_fusion_implementation_plan.md)
#### [NEW] [Artifacts/ekf_fusion_walkthrough.md](file:///d:/Projects/thingy91x/Artifacts/ekf_fusion_walkthrough.md)
#### [NEW] [Artifacts/ekf_fusion_test_results.md](file:///d:/Projects/thingy91x/Artifacts/ekf_fusion_test_results.md)
- Save design plans, execution walkthrough, matrix formulas, and hardware test scenario results in `Artifacts/`.

---

## Verification Plan

### Automated / Build Verification
- Compile firmware using nRF Connect SDK toolchain (`west build -b thingy91x/nrf9151/ns -- -DCONFIG_APP_PROFILE_ASSET_TRACKER=y`) with `CONFIG_BOOTLOADER_MCUBOOT=y`.

### Hardware Flashing & Live Testing
- Flash signed application package (`dfu_application.zip`) to connected **Thingy:91 X** (`THINGY91X_F40679066AD`) via MCUboot DFU.
- Monitor live serial console logs to verify EKF initialization, covariance matrix stability, and fused Roll/Pitch/Yaw orientation output.
