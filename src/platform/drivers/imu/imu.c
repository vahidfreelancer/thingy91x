#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "imu.h"

LOG_MODULE_REGISTER(imu_driver);

static const struct device *imu_dev = NULL;

int imu_init(void)
{
    /* Check devicetree compatible nodes safely at compile-time */
#if DT_HAS_COMPAT_STATUS_OKAY(bosch_bmi270)
    imu_dev = DEVICE_DT_GET_ANY(bosch_bmi270);
#elif DT_HAS_COMPAT_STATUS_OKAY(analog_devices_adxl362)
    imu_dev = DEVICE_DT_GET_ANY(analog_devices_adxl362);
#endif

    if (imu_dev && device_is_ready(imu_dev)) {
        LOG_INF("IMU device successfully initialized.");
    } else {
        LOG_WRN("Physical IMU device not found or ready; falling back to software simulation.");
        imu_dev = NULL;
    }
    return 0;
}

int imu_read(double accel[3], double gyro[3])
{
    if (!imu_dev) {
        /* Software simulation of a stationary board with minor jitter */
        static double mock_time = 0.0;
        mock_time += 1.0;
        
        accel[0] = 0.05 * sin(mock_time * 0.1);
        accel[1] = 0.03 * cos(mock_time * 0.2);
        accel[2] = 9.80665 + 0.02 * sin(mock_time * 0.3); // Gravity + minor noise
        
        gyro[0] = 0.2 * sin(mock_time * 0.1);
        gyro[1] = 0.1 * cos(mock_time * 0.15);
        gyro[2] = 0.05 * sin(mock_time * 0.05);
        return 0;
    }

    struct sensor_value val_accel[3];
    struct sensor_value val_gyro[3];
    int err;

    err = sensor_sample_fetch(imu_dev);
    if (err) {
        LOG_ERR("Failed to fetch sensor sample (err: %d)", err);
        return err;
    }

    err = sensor_channel_get(imu_dev, SENSOR_CHAN_ACCEL_XYZ, val_accel);
    if (err) {
        LOG_ERR("Failed to get Accel data (err: %d)", err);
        return err;
    }
    
    accel[0] = sensor_value_to_double(&val_accel[0]);
    accel[1] = sensor_value_to_double(&val_accel[1]);
    accel[2] = sensor_value_to_double(&val_accel[2]);

    err = sensor_channel_get(imu_dev, SENSOR_CHAN_GYRO_XYZ, val_gyro);
    if (err) {
        /* Gyro might not be available on all accelerometers (like ADXL362) */
        gyro[0] = 0.0;
        gyro[1] = 0.0;
        gyro[2] = 0.0;
    } else {
        gyro[0] = sensor_value_to_double(&val_gyro[0]);
        gyro[1] = sensor_value_to_double(&val_gyro[1]);
        gyro[2] = sensor_value_to_double(&val_gyro[2]);
    }

    return 0;
}
