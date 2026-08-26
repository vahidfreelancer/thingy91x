#ifndef EKF_FUSION_H
#define EKF_FUSION_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Extended Kalman Filter State Container (7-state vector).
 * State: x = [q0, q1, q2, q3, gyro_bias_x, gyro_bias_y, gyro_bias_z]^T
 */
struct ekf_state {
    float q[4];             /**< Unit Orientation Quaternion [q0, q1, q2, q3] */
    float gyro_bias[3];     /**< Gyroscope zero-rate bias vector [bx, by, bz] (rad/s) */
    float roll_deg;         /**< Fused Roll angle in degrees (-180.0° to 180.0°) */
    float pitch_deg;        /**< Fused Pitch angle in degrees (-90.0° to 90.0°) */
    float yaw_deg;          /**< Fused Yaw / Compass Heading angle in degrees (0.0° to 360.0°) */
    float cov_P[7][7];      /**< 7x7 State Covariance Matrix P */
    bool initialized;       /**< True if filter has completed initialization */
};

/**
 * @brief Initialize the Extended Kalman Filter state vector and covariance matrices.
 * 
 * Sets unit quaternion q = [1, 0, 0, 0], zeroes gyro bias, and sets initial error covariance P0.
 * 
 * @param state Pointer to ekf_state structure to initialize.
 * @return 0 on success, negative POSIX error code on failure.
 */
int ekf_fusion_init(struct ekf_state *state);

/**
 * @brief EKF Prediction Step: Integrates 3-axis Gyroscope angular velocity.
 * 
 * Kinematic integration: q_dot = 0.5 * q x omega.
 * 
 * @param state Pointer to ekf_state structure.
 * @param gyro_x Angular velocity X-axis (rad/s or deg/s).
 * @param gyro_y Angular velocity Y-axis (rad/s or deg/s).
 * @param gyro_z Angular velocity Z-axis (rad/s or deg/s).
 * @param dt Sampling time interval in seconds.
 * @return 0 on success, negative POSIX error code on failure.
 */
int ekf_fusion_predict(struct ekf_state *state, float gyro_x, float gyro_y, float gyro_z, float dt);

/**
 * @brief EKF Measurement Update Step: Fuses 3-Axis Accelerometer and 3-Axis Magnetometer.
 * 
 * Fuses gravity direction vector [ax, ay, az] and earth magnetic field vector [mx, my, mz]
 * to compute Kalman gain K and correct roll, pitch, yaw, and gyro bias drift.
 * 
 * @param state Pointer to ekf_state structure.
 * @param accel_x Acceleration force X-axis (m/s² or g).
 * @param accel_y Acceleration force Y-axis (m/s² or g).
 * @param accel_z Acceleration force Z-axis (m/s² or g).
 * @param mag_x Magnetic flux density X-axis (µT).
 * @param mag_y Magnetic flux density Y-axis (µT).
 * @param mag_z Magnetic flux density Z-axis (µT).
 * @return 0 on success, negative POSIX error code on failure.
 */
int ekf_fusion_update(struct ekf_state *state, float accel_x, float accel_y, float accel_z,
                      float mag_x, float mag_y, float mag_z);

/**
 * @brief Retrieve current 3D Euler Orientation Angles (Roll, Pitch, Yaw).
 * 
 * @param state Pointer to ekf_state structure.
 * @param roll Output pointer for Roll angle in degrees.
 * @param pitch Output pointer for Pitch angle in degrees.
 * @param yaw Output pointer for Yaw / Compass Heading angle in degrees.
 * @return 0 on success, negative POSIX error code on failure.
 */
int ekf_fusion_get_orientation(const struct ekf_state *state, float *roll, float *pitch, float *yaw);

#ifdef __cplusplus
}
#endif

#endif /* EKF_FUSION_H */
