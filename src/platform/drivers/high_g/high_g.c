#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/pm/device.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <math.h>
#include "high_g.h"

LOG_MODULE_REGISTER(high_g_driver);

static const struct device *high_g_dev = NULL;
static float impact_threshold_g = 5.0f;    /* Default 5.0g shock impact threshold */
static float freefall_threshold_g = 0.25f;  /* Free-fall threshold (< 0.25g) */
static float latched_peak_g = 0.0f;

int high_g_init(void)
{
    /* Check devicetree compatible nodes safely at compile-time */
#if DT_HAS_COMPAT_STATUS_OKAY(adi_adxl372)
    high_g_dev = DEVICE_DT_GET_ANY(adi_adxl372);
#elif DT_HAS_COMPAT_STATUS_OKAY(bosch_bma400)
    high_g_dev = DEVICE_DT_GET_ANY(bosch_bma400);
#elif DT_HAS_COMPAT_STATUS_OKAY(analog_devices_adxl362)
    high_g_dev = DEVICE_DT_GET_ANY(analog_devices_adxl362);
#endif

    if (high_g_dev && device_is_ready(high_g_dev)) {
        LOG_INF("High-G Impact sensor device '%s' initialized successfully.", high_g_dev->name);
    } else {
        LOG_WRN("Physical High-G sensor device unattached or not ready. Falling back to software simulation.");
        high_g_dev = NULL;
    }

    latched_peak_g = 0.0f;
    return 0;
}

int high_g_set_threshold(float threshold_g)
{
    if (threshold_g <= 0.0f) {
        return -EINVAL;
    }
    impact_threshold_g = threshold_g;
    LOG_INF("Updated High-G impact trigger threshold to %.2f g", (double)impact_threshold_g);
    return 0;
}

int high_g_reset_peak(void)
{
    latched_peak_g = 0.0f;
    LOG_INF("Latched peak acceleration reset to 0.0g");
    return 0;
}

int high_g_read(struct high_g_data *data)
{
    if (!data) {
        return -EINVAL;
    }

    if (!high_g_dev) {
        /* Realistic simulation: 1.0g gravity baseline + periodic impact spikes */
        static double step = 0.0;
        step += 1.0;

        /* Simulate stationary 1g gravity on Z-axis with minor jitter */
        data->accel_x = 0.04f * (float)sin(step * 0.2);
        data->accel_y = 0.02f * (float)cos(step * 0.15);
        data->accel_z = 1.0f + 0.05f * (float)sin(step * 0.1);

        /* Inject simulated shock impact spike every 6 samples for testing */
        if ((int)step % 6 == 0) {
            data->accel_x += 4.5f;
            data->accel_y += 3.2f;
            data->accel_z += 6.1f;
            LOG_WRN("[SIM SHOCK] Simulated physical drop/impact event triggered!");
        }

        /* Compute 3D Acceleration Vector Magnitude |a| = sqrt(Ax^2 + Ay^2 + Az^2) */
        float sum_sq = (data->accel_x * data->accel_x) +
                       (data->accel_y * data->accel_y) +
                       (data->accel_z * data->accel_z);
        data->magnitude = sqrtf(sum_sq);

        /* Track peak acceleration */
        if (data->magnitude > latched_peak_g) {
            latched_peak_g = data->magnitude;
        }
        data->peak_g = latched_peak_g;

        /* Evaluate Impact and Free-Fall conditions */
        data->impact_detected = (data->magnitude >= impact_threshold_g);
        data->freefall_detected = (data->magnitude <= freefall_threshold_g);
        data->valid = true;

        LOG_INF("[HIGH-G SIM] |a|=%.2fg (Ax=%.2f, Ay=%.2f, Az=%.2f) | Peak=%.2fg | Impact=%s | FreeFall=%s",
                (double)data->magnitude, (double)data->accel_x, (double)data->accel_y, (double)data->accel_z,
                (double)data->peak_g, data->impact_detected ? "YES" : "NO",
                data->freefall_detected ? "YES" : "NO");

        return 0;
    }

    /* Hardware Sensor Fetch */
    int err = sensor_sample_fetch(high_g_dev);
    if (err < 0) {
        LOG_ERR("Failed to fetch High-G sensor sample (err: %d)", err);
        data->valid = false;
        return err;
    }

    struct sensor_value accel[3];
    err = sensor_channel_get(high_g_dev, SENSOR_CHAN_ACCEL_XYZ, accel);
    if (err < 0) {
        LOG_ERR("Failed to get Accel channels (err: %d)", err);
        data->valid = false;
        return err;
    }

    /* Convert m/s^2 to g force (1g = 9.80665 m/s^2) */
    data->accel_x = (float)sensor_value_to_double(&accel[0]) / 9.80665f;
    data->accel_y = (float)sensor_value_to_double(&accel[1]) / 9.80665f;
    data->accel_z = (float)sensor_value_to_double(&accel[2]) / 9.80665f;

    /* Vector Magnitude Calculation */
    float sum_sq = (data->accel_x * data->accel_x) +
                   (data->accel_y * data->accel_y) +
                   (data->accel_z * data->accel_z);
    data->magnitude = sqrtf(sum_sq);

    /* Update Latched Peak */
    if (data->magnitude > latched_peak_g) {
        latched_peak_g = data->magnitude;
    }
    data->peak_g = latched_peak_g;

    data->impact_detected = (data->magnitude >= impact_threshold_g);
    data->freefall_detected = (data->magnitude <= freefall_threshold_g);
    data->valid = true;

    if (data->impact_detected) {
        LOG_WRN("[HIGH-G IMPACT ALERT] Magnitude %.2fg exceeded threshold %.2fg!",
                (double)data->magnitude, (double)impact_threshold_g);
    } else {
        LOG_INF("[HIGH-G DATA] |a|=%.2fg | Peak=%.2fg", (double)data->magnitude, (double)data->peak_g);
    }

    return 0;
}

int high_g_sleep(void)
{
    if (!high_g_dev) {
        LOG_DBG("[SIM] High-G sensor entered low-power sleep mode.");
        return 0;
    }

#if defined(CONFIG_PM_DEVICE)
    int ret = pm_device_action_run(high_g_dev, PM_DEVICE_ACTION_SUSPEND);
    if (ret < 0 && ret != -ENOSYS) {
        LOG_ERR("Failed to suspend High-G sensor device (err: %d)", ret);
        return ret;
    }
    LOG_INF("High-G sensor device suspended.");
#endif

    return 0;
}
