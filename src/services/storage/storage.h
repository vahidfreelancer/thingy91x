#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure representing a single logged travel record.
 */
struct travel_entry {
    uint32_t timestamp;   /**< Uptime timestamp (seconds) */
    double latitude;      /**< GPS Latitude (degrees) */
    double longitude;     /**< GPS Longitude (degrees) */
    float altitude;       /**< GPS Altitude (meters) */
    float speed;          /**< GPS Speed (m/s) */
    float roll;           /**< Pitch/Roll/Yaw: Roll (degrees) */
    float pitch;          /**< Pitch/Roll/Yaw: Pitch (degrees) */
    float yaw;            /**< Pitch/Roll/Yaw: Yaw (degrees) */
    uint8_t satellites;   /**< GPS Satellite count */
    float hdop;           /**< GPS HDOP */
};

/**
 * @brief Initialize the storage subsystem (mount flash partition).
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int storage_init(void);

/**
 * @brief Safely log/append a travel record to the storage medium.
 * 
 * @param entry Pointer to the travel entry to log.
 * @return 0 on success, negative POSIX error code on failure.
 */
int storage_log_travel_entry(const struct travel_entry *entry);

#ifdef __cplusplus
}
#endif

#endif /* STORAGE_H */
