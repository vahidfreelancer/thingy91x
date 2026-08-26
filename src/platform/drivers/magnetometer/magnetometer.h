#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 3-Axis Magnetometer measurement and compass data structure.
 */
struct mag_sensor_data {
    float mag_x_ut;         /**< Magnetic flux density X-axis in microteslas (µT) */
    float mag_y_ut;         /**< Magnetic flux density Y-axis in microteslas (µT) */
    float mag_z_ut;         /**< Magnetic flux density Z-axis in microteslas (µT) */
    float magnitude_ut;     /**< Total 3D vector magnetic magnitude |B| in µT */
    float heading_deg;      /**< 2D compass heading angle in degrees (0.0° to 360.0°) */
    bool tamper_detected;   /**< True if magnetic field magnitude exceeds 250 µT threshold */
    bool valid;             /**< True if sensor sample read successfully */
};

/**
 * @brief Initialize the 3-Axis Magnetometer hardware device.
 * 
 * Queries Devicetree for compatible magnetometer nodes (Bosch BMM150, ST LIS3MDL) and verifies readiness.
 * Registers simulation fallback if physical magnetometer is unattached.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int mag_sensor_init(void);

/**
 * @brief Read 3D magnetic flux density channels and compute compass heading angle.
 * 
 * Computes |B| = sqrt(Bx^2 + By^2 + Bz^2) and theta = atan2(By, Bx) * (180 / PI).
 * 
 * @param data Pointer to mag_sensor_data structure to populate.
 * @return 0 on success, negative POSIX error code on failure.
 */
int mag_sensor_read(struct mag_sensor_data *data);

/**
 * @brief Put magnetometer sensor into low-power suspended state.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int mag_sensor_sleep(void);

#ifdef __cplusplus
}
#endif

#endif /* MAGNETOMETER_H */
