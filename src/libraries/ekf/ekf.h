#ifndef EKF_H
#define EKF_H

#include "matrix.h"
#include "quaternion.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ekf_config {
    float q_gyro;        /**< Gyroscope rate noise covariance (rad/s)^2 */
    float q_gyro_bias;   /**< Gyroscope bias random walk covariance (rad/s^2)^2 */
    float r_accel;       /**< Accelerometer measurement noise covariance (g^2) */
    float r_yaw;         /**< Yaw heading measurement noise covariance (rad^2) */
};

struct ekf_filter {
    struct quaternion q;      /**< State: Attitude quaternion */
    float gyro_bias[3];       /**< State: Gyroscope bias estimates (rad/s) */

    struct matrix P;          /**< State estimation error covariance (7x7) */
    struct matrix Q;          /**< Process noise covariance (7x7) */
    
    float r_accel;            /**< Accel measurement noise */
    float r_yaw;              /**< Yaw measurement noise */
};

/**
 * @brief Initialize the Extended Kalman Filter state and covariances.
 * 
 * @param filter Pointer to the EKF filter structure.
 * @param config Pointer to the configuration parameters.
 * @param initial_roll Initial estimate of roll (degrees).
 * @param initial_pitch Initial estimate of pitch (degrees).
 * @param initial_yaw Initial estimate of yaw (degrees).
 */
void ekf_init(struct ekf_filter *filter, const struct ekf_config *config,
              float initial_roll, float initial_pitch, float initial_yaw);

/**
 * @brief EKF prediction step (time-update) using gyroscope measurements.
 * 
 * @param filter Pointer to the EKF filter structure.
 * @param gyro_raw Raw gyroscope measurements [x, y, z] in rad/s.
 * @param dt Time delta since last update in seconds.
 */
void ekf_predict(struct ekf_filter *filter, const float gyro_raw[3], float dt);

/**
 * @brief EKF measurement update (correction) using accelerometer readings.
 * 
 * @param filter Pointer to the EKF filter structure.
 * @param accel_raw Raw accelerometer measurements [x, y, z] in g (m/s^2 is also fine if normalized).
 * @return 0 on success, negative error code on failure.
 */
int ekf_update_accel(struct ekf_filter *filter, const float accel_raw[3]);

/**
 * @brief EKF measurement update (correction) using heading/yaw reading.
 * 
 * @param filter Pointer to the EKF filter structure.
 * @param yaw_measured_deg Measured yaw angle in degrees.
 * @return 0 on success, negative error code on failure.
 */
int ekf_update_yaw(struct ekf_filter *filter, float yaw_measured_deg);

/**
 * @brief Get the current attitude estimate from EKF in Euler angles.
 * 
 * @param filter Pointer to the EKF filter structure.
 * @param roll Output parameter for estimated roll (degrees).
 * @param pitch Output parameter for estimated pitch (degrees).
 * @param yaw Output parameter for estimated yaw (degrees).
 */
void ekf_get_attitude(const struct ekf_filter *filter, float *roll, float *pitch, float *yaw);

#ifdef __cplusplus
}
#endif

#endif /* EKF_H */
