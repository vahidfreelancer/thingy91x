#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* 
 * Profile-specific configurations for the Autopilot profile.
 * High-speed sampling rate and PID control loop configurations.
 */

#define IMU_SAMPLING_RATE_HZ             100
#define CONTROL_LOOP_RATE_HZ             50

/* Derived time periods in milliseconds */
#define IMU_SAMPLING_PERIOD_MS           (1000 / IMU_SAMPLING_RATE_HZ)
#define CONTROL_LOOP_PERIOD_MS           (1000 / CONTROL_LOOP_RATE_HZ)

#endif
