#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/pm/device.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <math.h>
#include "environment.h"

LOG_MODULE_REGISTER(env_driver);

static const struct device *env_dev = NULL;

int env_sensor_init(void)
{
    /* Query Devicetree for compatible environmental sensor drivers */
#if DT_HAS_COMPAT_STATUS_OKAY(bosch_bme680)
    env_dev = DEVICE_DT_GET_ANY(bosch_bme680);
#elif DT_HAS_COMPAT_STATUS_OKAY(bosch_bme688)
    env_dev = DEVICE_DT_GET_ANY(bosch_bme688);
#elif DT_HAS_COMPAT_STATUS_OKAY(sensirion_shtc3)
    env_dev = DEVICE_DT_GET_ANY(sensirion_shtc3);
#elif DT_HAS_COMPAT_STATUS_OKAY(bosch_bmp388)
    env_dev = DEVICE_DT_GET_ANY(bosch_bmp388);
#endif

    if (env_dev && device_is_ready(env_dev)) {
        LOG_INF("Environmental sensor device '%s' successfully initialized.", env_dev->name);
    } else {
        LOG_WRN("Physical environmental sensor hardware not ready or unattached. Falling back to software simulation.");
        env_dev = NULL;
    }

    return 0;
}

int env_sensor_read(struct env_sensor_data *data)
{
    if (!data) {
        return -EINVAL;
    }

    if (!env_dev) {
        /* Realistic software simulation for testing & headless environments */
        static double time_step = 0.0;
        time_step += 1.0;

        data->temperature = 22.5f + 1.8f * (float)sin(time_step * 0.05);
        data->humidity = 48.0f + 5.0f * (float)cos(time_step * 0.04);
        data->pressure = 1013.25f + 2.5f * (float)sin(time_step * 0.02);
        data->gas_resistance = 52000.0f + 4000.0f * (float)sin(time_step * 0.08);

        /* Calculate estimated Indoor Air Quality (IAQ) index (0-500 scale, <50 is Good) */
        float iaq_val = 25.0f + 10.0f * (float)cos(time_step * 0.06);
        data->iaq_index = (uint16_t)(iaq_val < 0.0f ? 0.0f : iaq_val);
        data->valid = true;

        LOG_DBG("[SIM] Environmental read: Temp=%.2f°C, Hum=%.2f%%, Press=%.2fhPa, Gas=%.0fΩ, IAQ=%u",
                (double)data->temperature, (double)data->humidity,
                (double)data->pressure, (double)data->gas_resistance, data->iaq_index);

        return 0;
    }

    int ret = sensor_sample_fetch(env_dev);
    if (ret < 0) {
        LOG_ERR("Failed to fetch environmental sensor sample (err: %d)", ret);
        data->valid = false;
        return ret;
    }

    struct sensor_value val;

    /* Read Temperature */
    if (sensor_channel_get(env_dev, SENSOR_CHAN_AMBIENT_TEMP, &val) == 0) {
        data->temperature = (float)sensor_value_to_double(&val);
    } else {
        data->temperature = 0.0f;
    }

    /* Read Humidity */
    if (sensor_channel_get(env_dev, SENSOR_CHAN_HUMIDITY, &val) == 0) {
        data->humidity = (float)sensor_value_to_double(&val);
    } else {
        data->humidity = 0.0f;
    }

    /* Read Pressure (convert kPa to hPa if applicable) */
    if (sensor_channel_get(env_dev, SENSOR_CHAN_PRESS, &val) == 0) {
        float press_val = (float)sensor_value_to_double(&val);
        /* Zephyr pressure unit is kPa; convert to hPa (1 kPa = 10 hPa) */
        data->pressure = (press_val < 200.0f) ? (press_val * 10.0f) : press_val;
    } else {
        data->pressure = 0.0f;
    }

    /* Read Gas Resistance (VOC) */
    if (sensor_channel_get(env_dev, SENSOR_CHAN_GAS_RES, &val) == 0) {
        data->gas_resistance = (float)sensor_value_to_double(&val);
        /* Derive basic IAQ estimate from gas resistance baseline */
        data->iaq_index = (data->gas_resistance > 50000.0f) ? 25 : 75;
    } else {
        data->gas_resistance = 0.0f;
        data->iaq_index = 0;
    }

    data->valid = true;
    LOG_INF("Environmental sample: Temp=%.1f°C, Hum=%.1f%%, Press=%.1fhPa, Gas=%.0fΩ",
            (double)data->temperature, (double)data->humidity,
            (double)data->pressure, (double)data->gas_resistance);

    return 0;
}

int env_sensor_sleep(void)
{
    if (!env_dev) {
        LOG_DBG("[SIM] Environmental sensor entered low power sleep mode.");
        return 0;
    }

#if defined(CONFIG_PM_DEVICE)
    int ret = pm_device_action_run(env_dev, PM_DEVICE_ACTION_SUSPEND);
    if (ret < 0 && ret != -ENOSYS) {
        LOG_ERR("Failed to suspend environmental sensor device (err: %d)", ret);
        return ret;
    }
    LOG_INF("Environmental sensor device suspended.");
#endif

    return 0;
}
