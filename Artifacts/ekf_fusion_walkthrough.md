# Walkthrough: Extended Kalman Filter (EKF) 9-DOF Sensor Fusion Library

The **Extended Kalman Filter (EKF) AHRS 9-DOF Sensor Fusion Library** has been implemented in `src/services/fusion/` to fuse 6-Axis IMU (Accelerometer + Gyroscope) and 3-Axis Magnetometer (BMM150) data into a stabilized unit orientation quaternion $\mathbf{q} = [q_0, q_1, q_2, q_3]^T$ and output 3D Euler angles (Roll $\phi$, Pitch $\theta$, Yaw/Heading $\psi$), compiled, and programmed to the connected **Nordic Thingy:91 X** hardware.

---

## 1. Implemented Features & Library HAL

- **Service Header**: [`ekf_fusion.h`](file:///d:/Projects/thingy91x/src/services/fusion/ekf_fusion.h)
  - Exposes `struct ekf_state`, `ekf_fusion_init()`, `ekf_fusion_predict()`, `ekf_fusion_update()`, `ekf_fusion_get_orientation()`.
- **Service Logic**: [`ekf_fusion.c`](file:///d:/Projects/thingy91x/src/services/fusion/ekf_fusion.c)
  - Implements state vector kinematics $\dot{\mathbf{q}} = \frac{1}{2} \mathbf{q} \otimes \boldsymbol{\omega}$, covariance propagation, unit quaternion normalization $\|\mathbf{q}\| = 1$, gyro bias estimation $\mathbf{b}_\omega$, and 9-DOF measurement correction update (Accelerometer gravity + 3-Axis Magnetometer flux vector).
- **Application Integration**: [`app.c`](file:///d:/Projects/thingy91x/src/app/asset_tracker/app.c#L100-L112)
  - `app_init()` initializes EKF filter state; executes `ekf_fusion_predict()` and `ekf_fusion_update()` periodically inside `telemetry_work_handler()`, logging fused 3D orientation metrics.

---

## 2. Hardware Deployment & Test Results

- **Target Board**: Nordic Thingy:91 X (`THINGY91X_F40679066AD`, Board PCA20065)
- **Firmware Flashed**: [`build/dfu_application.zip`](file:///d:/Projects/thingy91x/build/dfu_application.zip) via MCUboot DFU
- **Test Scenarios**: Saved in [`Artifacts/ekf_fusion_test_results.md`](file:///d:/Projects/thingy91x/Artifacts/ekf_fusion_test_results.md)
