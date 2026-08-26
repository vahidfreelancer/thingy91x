#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/pm/device.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <math.h>
#include "magnetometer.h"

LOG_MODULE_REGISTER(mag_driver);

#define M_PI_F                       3.14159265358979323846f
#define MAGNETIC_TAMPER_THRESHOLD_UT 250.0f   /* 250 µT magnetic proximity threshold */

static const struct device *mag_dev = NULL;

int mag_sensor_init(void)
{
    /* Query Devicetree for compatible magnetometer drivers */
#if DT_HAS_COMPAT_STATUS_OKAY(bosch_bmm150)
    mag_dev = DEVICE_DT_GET_ANY(bosch_bmm150);
#elif DT_HAS_COMPAT_STATUS_OKAY(st_lis3mdl)
    mag_dev = DEVICE_DT_GET_ANY(st_lis3mdl);
#endif

    if (mag_dev && device_is_ready(mag_dev)) {
        LOG_INF("3-Axis Magnetometer device '%s' initialized successfully.", mag_dev->name);
    } else {
        LOG_WRN("Physical Magnetometer unattached or not ready. Falling back to software simulation.");
        mag_dev = NULL;
    }

    return 0;
}

int mag_sensor_read(struct mag_sensor_data *data)
{
    if (!data) {
        return -EINVAL;
    }

    if (!mag_dev) {
        /* Software simulation of earth magnetic field & rotating compass heading */
        static float sim_angle = 45.0f; /* 45° North-East */
        sim_angle += 12.5f;
        if (sim_angle >= 360.0f) {
            sim_angle -= 360.0f;
        }

        /* Earth magnetic field intensity ~45 µT */
        float rad = sim_angle * (M_PI_F / 180.0f);
        data->mag_x_ut = 30.0f * cosf(rad);
        data->mag_y_ut = 30.0f * sinf(rad);
        data->mag_z_ut = -33.5f;

        /* Magnitude |B| = sqrt(Bx^2 + By^2 + Bz^2) */
        data->magnitude_ut = sqrtf(data->mag_x_ut * data->mag_x_ut +
                                  data->mag_y_ut * data->mag_y_ut +
                                  data->mag_z_ut * data->mag_z_ut);

        /* 2D Compass Heading angle theta = atan2(By, Bx) * (180 / PI) */
        float heading = atan2f(data->mag_y_ut, data->mag_x_ut) * (180.0f / M_PI_F);
        if (heading < 0.0f) {
            heading += 360.0f;
        }
        data->heading_deg = heading;

        data->tamper_detected = (data->magnitude_ut > MAGNETIC_TAMPER_THRESHOLD_UT);
        data->valid = true;

        LOG_INF("[MAG SIM] Bx=%.1f, By=%.1f, Bz=%.1f µT | |B|=%.1f µT | Heading=%.1f° | Tamper=%s",
                (double)data->mag_x_ut, (double)data->mag_y_ut, (double)data->mag_z_ut,
                (double)data->magnitude_ut, (double)data->heading_deg,
                data->tamper_detected ? "DETECTED" : "NO");

        return 0;
    }

    /* Read Hardware Magnetometer Channels */
    struct sensor_value val[3];
    int err = sensor_sample_fetch(mag_dev);
    if (err < 0) {
        LOG_ERR("Failed to fetch magnetometer sample (err: %d)", err);
        data->valid = false;
        return err;
    }

    err = sensor_channel_get(mag_dev, SENSOR_CHAN_MAGN_XYZ, val);
    if (err < 0) {
        LOG_ERR("Failed to get SENSOR_CHAN_MAGN_XYZ channel (err: %d)", err);
        data->valid = false;
        return err;
    }

    /* Convert to microteslas (µT) */
    data->mag_x_ut = (float)sensor_value_to_double(&val[0]) * 1000000.0f;
    data->mag_y_ut = (float)sensor_value_to_double(&val[1]) * 1000000.0f;
    data->mag_z_ut = (float)sensor_value_to_double(&val[2]) * 1000000.0f;

    /* 3D Vector Magnitude |B| */
    data->magnitude_ut = sqrtf(data->mag_x_ut * data->mag_x_ut +
                              data->mag_y_ut * data->mag_y_ut +
                              data->mag_z_ut * data->mag_z_ut);

    /* 2D Compass Heading angle theta = atan2(By, Bx) * (180 / PI) */
    float heading = atan2f(data->mag_y_ut, data->mag_x_ut) * (180.0f / M_PI_F);
    if (heading < 0.0f) {
        heading += 360.0f;
    }
    data->heading_deg = heading;

    data->tamper_detected = (data->magnitude_ut > MAGNETIC_TAMPER_THRESHOLD_UT);
    data->valid = true;

    LOG_INF("[MAG HW] Bx=%.1f, By=%.1f, Bz=%.1f µT | |B|=%.1f µT | Heading=%.1f°",
            (double)data->mag_x_ut, (double)data->mag_y_ut, (double)data->mag_z_ut,
            (double)data->magnitude_ut, (double)data->heading_deg);

    return 0;
}

int mag_sensor_sleep(void)
{
    if (!mag_dev) {
        LOG_DBG("[SIM] Magnetometer driver entered low power state.");
        return 0;
    }

#if defined(CONFIG_PM_DEVICE)
    int ret = pm_device_action_run(mag_dev, PM_DEVICE_ACTION_SUSPEND);
    if (ret < 0 && ret != -ENOSYS) {
        LOG_ERR("Failed to suspend magnetometer device (err: %d)", ret);
        return ret;
    }
    LOG_INF("Magnetometer sensor suspended.");
#endif

    return 0;
}
