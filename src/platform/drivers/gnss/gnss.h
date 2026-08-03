#ifndef GNSS_H
#define GNSS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure to hold GNSS fix position and metadata.
 */
struct gnss_data {
    double latitude;      /**< Latitude in degrees */
    double longitude;     /**< Longitude in degrees */
    float altitude;       /**< Altitude in meters above sea level */
    float speed;          /**< Speed in meters per second */
    uint8_t satellites;   /**< Number of satellites in view/used */
    float hdop;           /**< Horizontal dilution of precision */
    bool valid;           /**< True if the fix is valid */
};

/**
 * @brief Initialize the GNSS receiver interface.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int gnss_init(void);

/**
 * @brief Retrieve the latest GNSS fix and metadata.
 * 
 * @param data Pointer to a gnss_data structure to write to.
 * @return 0 on success, negative POSIX error code on failure.
 */
int gnss_read(struct gnss_data *data);

#ifdef __cplusplus
}
#endif

#endif /* GNSS_H */
