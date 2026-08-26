#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Environmental sensor measurement data structure.
 */
struct env_sensor_data {
    float temperature;      /**< Ambient temperature in degrees Celsius (°C) */
    float humidity;         /**< Relative humidity in percentage (% RH) */
    float pressure;         /**< Atmospheric pressure in hectopascals (hPa) */
    float gas_resistance;   /**< VOC gas sensor resistance in Ohms (Ω) */
    uint16_t iaq_index;     /**< Indoor Air Quality Index (0 - 500 scale) */
    bool valid;             /**< True if sensor sample read successfully */
};

/**
 * @brief Initialize the environmental sensor hardware device.
 * 
 * Searches Devicetree for compatible environmental sensor nodes (Bosch BME688/BME680,
 * Sensirion SHTC3, Bosch BMP388/390) and verifies device readiness.
 * Falls back to software simulation if physical sensor is absent.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int env_sensor_init(void);

/**
 * @brief Read environmental measurements from the sensor.
 * 
 * Fetches sample channels from hardware (or simulation) and updates the data structure.
 * 
 * @param data Pointer to env_sensor_data structure to populate.
 * @return 0 on success, negative POSIX error code on failure.
 */
int env_sensor_read(struct env_sensor_data *data);

/**
 * @brief Put the environmental sensor into low-power sleep/suspended state.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int env_sensor_sleep(void);

#ifdef __cplusplus
}
#endif

#endif /* ENVIRONMENT_H */
