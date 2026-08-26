#ifndef HIGH_G_H
#define HIGH_G_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief High-G Acceleration & Motion measurement data structure.
 */
struct high_g_data {
    float accel_x;          /**< Acceleration X-axis (in g force) */
    float accel_y;          /**< Acceleration Y-axis (in g force) */
    float accel_z;          /**< Acceleration Z-axis (in g force) */
    float magnitude;        /**< Total 3D vector acceleration magnitude |a| in g */
    float peak_g;           /**< Peak acceleration magnitude recorded since initialization/reset */
    bool impact_detected;   /**< True if magnitude exceeded shock impact threshold */
    bool freefall_detected; /**< True if magnitude dropped below free-fall threshold (< 0.2g) */
    bool valid;             /**< True if sensor sample read successfully */
};

/**
 * @brief Initialize the High-G accelerometer hardware device.
 * 
 * Searches Devicetree for compatible High-G / Motion sensor nodes (ADI ADXL372, Bosch BMA400, ADXL362)
 * and verifies device readiness. Enables threshold latching and software simulation fallback if hardware is absent.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int high_g_init(void);

/**
 * @brief Read acceleration measurements and calculate vector metrics.
 * 
 * Computes vector magnitude |a| = sqrt(Ax^2 + Ay^2 + Az^2), evaluates shock and freefall flags,
 * and updates peak acceleration latch.
 * 
 * @param data Pointer to high_g_data structure to populate.
 * @return 0 on success, negative POSIX error code on failure.
 */
int high_g_read(struct high_g_data *data);

/**
 * @brief Set custom shock impact trigger threshold (in g force).
 * 
 * @param threshold_g Acceleration threshold value in g (e.g. 5.0f, 10.0f, 50.0f).
 * @return 0 on success, negative POSIX error code on failure.
 */
int high_g_set_threshold(float threshold_g);

/**
 * @brief Reset latched peak acceleration metric to 0.0g.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int high_g_reset_peak(void);

/**
 * @brief Put High-G sensor into ultra-low-power sleep/suspended state.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int high_g_sleep(void);

#ifdef __cplusplus
}
#endif

#endif /* HIGH_G_H */
