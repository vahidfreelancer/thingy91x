#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "app.h"
#include "app_config.h"
#include "imu.h"
#include "gnss.h"
#include "attitude.h"
#include "kalman.h"
#include "storage.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

LOG_MODULE_REGISTER(app_travel_history);

static struct k_work_delayable log_work;
static struct kalman_filter roll_kalman;
static struct kalman_filter pitch_kalman;
static float yaw_accum = 0.0f;

static void log_work_handler(struct k_work *work)
{
    struct gnss_data gps;
    double accel[3];
    double gyro[3];
    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;

    /* 1. Read IMU data (accelerometer and gyroscope) */
    if (imu_read(accel, gyro) == 0) {
        float dt = (float)LOG_INTERVAL_MS / 1000.0f;
        
        /* Calculate Roll and Pitch angle from accelerometer (tilt sensing) */
        double roll_acc = atan2(accel[1], accel[2]) * 180.0 / M_PI;
        double pitch_acc = atan2(-accel[0], sqrt(accel[1] * accel[1] + accel[2] * accel[2])) * 180.0 / M_PI;

        /* Update Kalman filters with accelerometer measurements and gyro rates */
        roll = kalman_update(&roll_kalman, (float)roll_acc, (float)gyro[0], dt);
        pitch = kalman_update(&pitch_kalman, (float)pitch_acc, (float)gyro[1], dt);

        /* Integrate gyroscope Z-axis directly for Yaw (under starvation constraint bounds) */
        yaw_accum += (float)(gyro[2] * dt);
        if (yaw_accum > 180.0f) {
            yaw_accum -= 360.0f;
        } else if (yaw_accum < -180.0f) {
            yaw_accum += 360.0f;
        }
        yaw = yaw_accum;
    } else {
        LOG_WRN("Failed to read IMU data.");
    }

    /* 2. Read GNSS data */
    if (gnss_read(&gps) != 0) {
        LOG_WRN("Failed to read GNSS position.");
        gps.valid = false;
    }

    /* 3. Log to external flash storage (if position is valid) */
    if (gps.valid) {
        struct travel_entry entry = {
            .timestamp = (uint32_t)(k_uptime_get() / 1000),
            .latitude = gps.latitude,
            .longitude = gps.longitude,
            .altitude = gps.altitude,
            .speed = gps.speed,
            .roll = roll,
            .pitch = pitch,
            .yaw = yaw,
            .satellites = gps.satellites,
            .hdop = gps.hdop,
        };

        int err = storage_log_travel_entry(&entry);
        if (err == 0) {
            LOG_INF("[TRAVEL LOG] TS: %u | Lat: %.6f, Lon: %.6f | Roll: %.1f, Pitch: %.1f, Yaw: %.1f | Sats: %u",
                    entry.timestamp, entry.latitude, entry.longitude,
                    entry.roll, entry.pitch, entry.yaw, entry.satellites);
        } else {
            LOG_ERR("Failed to write travel record to flash (err: %d)", err);
        }
    } else {
        LOG_INF("Awaiting valid GNSS fix...");
    }

    /* Reschedule logging task */
    k_work_reschedule(&log_work, K_MSEC(LOG_INTERVAL_MS));
}

int app_init(void)
{
    LOG_INF("Initializing TravelHistory Application Profile...");

    /* Initialize roll and pitch Kalman filters with config parameters */
    kalman_init(&roll_kalman, KALMAN_Q_ANGLE, KALMAN_Q_BIAS, KALMAN_R_MEASURE);
    kalman_init(&pitch_kalman, KALMAN_Q_ANGLE, KALMAN_Q_BIAS, KALMAN_R_MEASURE);
    yaw_accum = 0.0f;

    /* Initialize drivers and services */
    if (imu_init() != 0) {
        LOG_ERR("IMU Initialization failed.");
        return -1;
    }

    if (gnss_init() != 0) {
        LOG_ERR("GNSS Initialization failed.");
        return -1;
    }

    if (storage_init() != 0) {
        LOG_ERR("Storage Service Initialization failed.");
        return -1;
    }

    /* Initialize the delayed work structure */
    k_work_init_delayable(&log_work, log_work_handler);

    return 0;
}

void app_run(void)
{
    LOG_INF("Running TravelHistory Application...");

    /* Trigger the first logging cycle immediately */
    k_work_reschedule(&log_work, K_NO_WAIT);

    /* Yield main thread to allow workqueues to process the logging task */
    while (1) {
        k_sleep(K_FOREVER);
    }
}
