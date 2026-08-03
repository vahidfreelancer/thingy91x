#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "app.h"
#include "app_config.h"
#include "imu.h"
#include "ekf.h"
#include <math.h>

LOG_MODULE_REGISTER(app_autopilot);

/* 
 * Define high-priority cooperative thread for the control loop
 * to ensure deterministic, jitter-free execution.
 */
#define CONTROL_THREAD_STACK_SIZE 2048

#ifdef CONFIG_APP_MAIN_THREAD_PRIORITY
#define CONTROL_THREAD_PRIORITY   CONFIG_APP_MAIN_THREAD_PRIORITY
#else
#define CONTROL_THREAD_PRIORITY   -10
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define DEG_TO_RAD (M_PI / 180.0f)

static struct ekf_filter filter;
K_THREAD_STACK_DEFINE(control_thread_stack, CONTROL_THREAD_STACK_SIZE);
static struct k_thread control_thread_data;

static void control_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("Real-time control thread started with priority %d.", CONTROL_THREAD_PRIORITY);

    double accel_d[3];
    double gyro_d[3];
    float accel_f[3];
    float gyro_rad[3];
    
    float roll, pitch, yaw;
    uint32_t log_timestamp = 0;
    
    float dt = (float)CONTROL_LOOP_PERIOD_MS / 1000.0f;

    while (1) {
        /* 1. Read IMU sensors high-speed */
        if (imu_read(accel_d, gyro_d) == 0) {
            /* Convert double to float */
            accel_f[0] = (float)accel_d[0];
            accel_f[1] = (float)accel_d[1];
            accel_f[2] = (float)accel_d[2];

            /* Convert gyro rates from deg/s to rad/s for EKF update equations */
            gyro_rad[0] = (float)gyro_d[0] * DEG_TO_RAD;
            gyro_rad[1] = (float)gyro_d[1] * DEG_TO_RAD;
            gyro_rad[2] = (float)gyro_d[2] * DEG_TO_RAD;

            /* 2. Run EKF prediction and correction steps */
            ekf_predict(&filter, gyro_rad, dt);
            ekf_update_accel(&filter, accel_f);
            
            /* Periodically run a simulated yaw correction to demonstrate modular updates */
            static uint32_t step_count = 0;
            if (++step_count % 50 == 0) { /* Update yaw at ~1 Hz */
                ekf_update_yaw(&filter, 0.0f); /* Correct yaw towards north */
            }

            /* Retrieve estimated attitude */
            ekf_get_attitude(&filter, &roll, &pitch, &yaw);
            
            /* 3. Output results periodically to avoid flooding logs */
            uint32_t now = k_uptime_get();
            if (now - log_timestamp >= 500) {
                log_timestamp = now;
                LOG_INF("EKF Attitude: Roll = %6.2f | Pitch = %6.2f | Yaw = %6.2f (deg)",
                        (double)roll, (double)pitch, (double)yaw);
                LOG_INF("Estimated Bias: X = %.4f | Y = %.4f | Z = %.4f (deg/s)",
                        (double)(filter.gyro_bias[0] * 180.0f / M_PI),
                        (double)(filter.gyro_bias[1] * 180.0f / M_PI),
                        (double)(filter.gyro_bias[2] * 180.0f / M_PI));
            }
        } else {
            LOG_WRN("Failed to read IMU sensor data.");
        }

        /* Yield control to allow other tasks to execute */
        k_sleep(K_MSEC(CONTROL_LOOP_PERIOD_MS));
    }
}

int app_init(void)
{
    LOG_INF("Initializing Autopilot Profile...");

    /* 1. Initialize IMU Driver */
    if (imu_init() != 0) {
        LOG_ERR("Failed to initialize IMU driver!");
        return -1;
    }

    /* 2. Configure EKF parameters */
    struct ekf_config config = {
        .q_gyro = 1e-4f,       /* Gyro noise covariance */
        .q_gyro_bias = 1e-6f,  /* Gyro bias stability covariance */
        .r_accel = 1e-2f,      /* Accelerometer noise covariance */
        .r_yaw = 1e-2f         /* Yaw measurement noise covariance */
    };

    /* 3. Initialize EKF states (assume starting at levelling conditions) */
    ekf_init(&filter, &config, 0.0f, 0.0f, 0.0f);

    return 0;
}

void app_run(void)
{
    LOG_INF("Running Autopilot System...");

    /* Spawn the real-time control thread */
    k_thread_create(&control_thread_data, control_thread_stack,
                    K_THREAD_STACK_SIZEOF(control_thread_stack),
                    control_thread_entry, NULL, NULL, NULL,
                    CONTROL_THREAD_PRIORITY, 0, K_NO_WAIT);

    /* Yield the main thread permanently; the control thread & workqueues run */
    while (1) {
        k_sleep(K_FOREVER);
    }
}
