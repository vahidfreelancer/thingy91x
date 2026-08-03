#ifndef ATTITUDE_H
#define ATTITUDE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Complementary filter state for attitude estimation.
 */
struct attitude_filter {
    float roll;      /**< Roll angle in degrees */
    float pitch;     /**< Pitch angle in degrees */
    float yaw;       /**< Yaw angle in degrees */
    float alpha;     /**< Complementary filter coefficient (e.g. 0.98) */
    bool initialized;/**< Set to true after first accelerometer sample */
};

/**
 * @brief Initialize the attitude filter state.
 * 
 * @param filter Pointer to the filter structure.
 * @param alpha Filter coefficient (0.0 to 1.0; 0.98 is recommended).
 */
void attitude_filter_init(struct attitude_filter *filter, float alpha);

/**
 * @brief Update the attitude estimation using new accelerometer and gyroscope values.
 * 
 * @param filter Pointer to the filter structure.
 * @param ax Accelerometer X reading (m/s^2 or g).
 * @param ay Accelerometer Y reading (m/s^2 or g).
 * @param az Accelerometer Z reading (m/s^2 or g).
 * @param gx Gyroscope X reading (degrees/s or rad/s, depending on unit conversion).
 * @param gy Gyroscope Y reading (degrees/s or rad/s).
 * @param gz Gyroscope Z reading (degrees/s or rad/s).
 * @param dt Time delta since last update in seconds.
 */
void attitude_filter_update(struct attitude_filter *filter, 
                            double ax, double ay, double az,
                            double gx, double gy, double gz,
                            float dt);

/**
 * @brief Retrieve the estimated Roll, Pitch, and Yaw angles.
 * 
 * @param filter Pointer to the filter structure.
 * @param roll Pointer to store the estimated roll in degrees.
 * @param pitch Pointer to store the estimated pitch in degrees.
 * @param yaw Pointer to store the estimated yaw in degrees.
 */
void attitude_filter_get_rpy(const struct attitude_filter *filter, 
                             float *roll, float *pitch, float *yaw);

#ifdef __cplusplus
}
#endif

#endif /* ATTITUDE_H */
