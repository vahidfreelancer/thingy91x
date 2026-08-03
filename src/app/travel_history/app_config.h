#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* Log interval in milliseconds (1 second) */
#define LOG_INTERVAL_MS                  1000

/* Complementary filter coefficient for attitude math */
#define ATTITUDE_FILTER_ALPHA            0.98f

/* Kalman Filter Adjustable Parameters */
#define KALMAN_Q_ANGLE                   0.001f  /* Process noise covariance for angle */
#define KALMAN_Q_BIAS                    0.003f  /* Process noise covariance for gyro bias */
#define KALMAN_R_MEASURE                 0.030f  /* Measurement noise covariance (accel) */

#endif /* APP_CONFIG_H */
