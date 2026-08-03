#ifndef IMU_H
#define IMU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the IMU hardware and device driver.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int imu_init(void);

/**
 * @brief Read accelerometer and gyroscope measurements from the IMU.
 * 
 * @param accel Array of size 3 to store accel X, Y, Z (in m/s^2).
 * @param gyro Array of size 3 to store gyro X, Y, Z (in degrees/second).
 * @return 0 on success, negative POSIX error code on failure.
 */
int imu_read(double accel[3], double gyro[3]);

#ifdef __cplusplus
}
#endif

#endif /* IMU_H */
