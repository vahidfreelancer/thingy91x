#ifndef KALMAN_H
#define KALMAN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 1D Kalman Filter state structure for estimating angle and gyro bias.
 */
struct kalman_filter {
    /* State variables */
    float angle;         /**< Estimated angle (degrees) */
    float bias;          /**< Estimated gyroscope bias drift (degrees/second) */

    /* Covariances */
    float P[2][2];       /**< Error covariance matrix */
    
    /* Configurable/Adjustable parameters */
    float Q_angle;       /**< Process noise covariance for angle */
    float Q_bias;        /**< Process noise covariance for gyro bias */
    float R_measure;     /**< Measurement noise covariance for accelerometer */
};

/**
 * @brief Initialize the Kalman filter state and parameters.
 * 
 * @param filter Pointer to the filter structure.
 * @param Q_angle Process noise covariance for the angle.
 * @param Q_bias Process noise covariance for the gyroscope bias.
 * @param R_measure Measurement noise covariance (accelerometer).
 */
void kalman_init(struct kalman_filter *filter, float Q_angle, float Q_bias, float R_measure);

/**
 * @brief Update the Kalman filter state with new readings.
 * 
 * @param filter Pointer to the filter structure.
 * @param new_angle Measurement angle from accelerometer (degrees).
 * @param new_rate Angular velocity reading from gyroscope (degrees/second).
 * @param dt Time delta since last update (seconds).
 * @return The updated estimated angle.
 */
float kalman_update(struct kalman_filter *filter, float new_angle, float new_rate, float dt);

#ifdef __cplusplus
}
#endif

#endif /* KALMAN_H */
