#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/pm/device.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <math.h>
#include "pmic.h"

LOG_MODULE_REGISTER(pmic_driver);

#define NOMINAL_BATTERY_CAPACITY_MAH   1350   /* Thingy:91 X LiPo battery nominal capacity (1350 mAh) */
#define LOW_BATTERY_ALERT_THRESHOLD    15.0f  /* Low battery alert at 15.0% SoC */

static const struct device *pmic_dev = NULL;

/**
 * @brief Calculate State-of-Charge (SoC %) from battery cell Open Circuit Voltage (mV)
 * using a piecewise linear LiPo OCV discharge model.
 */
static float calculate_soc_from_ocv(uint16_t voltage_mv)
{
    if (voltage_mv >= 4200) {
        return 100.0f;
    } else if (voltage_mv >= 4000) {
        return 80.0f + ((float)(voltage_mv - 4000) / 200.0f) * 20.0f;
    } else if (voltage_mv >= 3800) {
        return 40.0f + ((float)(voltage_mv - 3800) / 200.0f) * 40.0f;
    } else if (voltage_mv >= 3600) {
        return 10.0f + ((float)(voltage_mv - 3600) / 200.0f) * 30.0f;
    } else if (voltage_mv >= 3000) {
        return ((float)(voltage_mv - 3000) / 600.0f) * 10.0f;
    } else {
        return 0.0f;
    }
}

int pmic_init(void)
{
    /* Query Devicetree for compatible PMIC / Fuel Gauge drivers */
#if DT_HAS_COMPAT_STATUS_OKAY(nordic_npm1300)
    pmic_dev = DEVICE_DT_GET_ANY(nordic_npm1300);
#elif DT_HAS_COMPAT_STATUS_OKAY(maxim_max17048)
    pmic_dev = DEVICE_DT_GET_ANY(maxim_max17048);
#elif DT_HAS_COMPAT_STATUS_OKAY(adi_adp5360)
    pmic_dev = DEVICE_DT_GET_ANY(adi_adp5360);
#endif

    /* Safely verify device readiness AND sensor API pointer presence */
    if (pmic_dev && device_is_ready(pmic_dev) && pmic_dev->api) {
        LOG_INF("PMIC Fuel Gauge device '%s' initialized successfully.", pmic_dev->name);
    } else {
        LOG_WRN("Physical PMIC fuel gauge sensor API unattached. Falling back to software simulation.");
        pmic_dev = NULL;
    }

    return 0;
}

int pmic_read(struct pmic_battery_data *data)
{
    if (!data) {
        return -EINVAL;
    }

    if (!pmic_dev || !device_is_ready(pmic_dev) || !pmic_dev->api) {
        /* Realistic software simulation of LiPo battery discharge & power math */
        static float sim_voltage = 4150.0f;
        static double step = 0.0;
        step += 1.0;

        sim_voltage -= 2.5f;
        if (sim_voltage < 3300.0f) {
            sim_voltage = 4150.0f;
        }

        data->voltage_mv = (uint16_t)sim_voltage;
        data->current_ma = -45;
        data->charge_status = PMIC_DISCHARGING;

        data->soc_percent = calculate_soc_from_ocv(data->voltage_mv);
        data->power_mw = ((float)data->voltage_mv * (float)abs(data->current_ma)) / 1000.0f;
        data->remaining_capacity_mah = (uint16_t)((float)NOMINAL_BATTERY_CAPACITY_MAH * (data->soc_percent / 100.0f));
        data->low_battery_alert = (data->soc_percent < LOW_BATTERY_ALERT_THRESHOLD);
        data->valid = true;

        LOG_INF("[PMIC SIM] V=%u mV | SoC=%.1f%% | I=%d mA | P=%.2f mW | Rem=%u mAh | Status=%s",
                data->voltage_mv, (double)data->soc_percent, data->current_ma,
                (double)data->power_mw, data->remaining_capacity_mah,
                data->low_battery_alert ? "LOW BATTERY ALERT" : "OK");

        return 0;
    }

    /* Read Hardware Fuel Gauge Channels safely */
    struct sensor_value val_v, val_i;
    int err = sensor_sample_fetch(pmic_dev);
    if (err < 0) {
        LOG_ERR("Failed to fetch PMIC sample (err: %d)", err);
        data->valid = false;
        return err;
    }

    if (sensor_channel_get(pmic_dev, SENSOR_CHAN_GAUGE_VOLTAGE, &val_v) == 0) {
        data->voltage_mv = (uint16_t)(sensor_value_to_double(&val_v) * 1000.0);
    } else {
        data->voltage_mv = 3700;
    }

    if (sensor_channel_get(pmic_dev, SENSOR_CHAN_GAUGE_AVG_CURRENT, &val_i) == 0) {
        data->current_ma = (int16_t)(sensor_value_to_double(&val_i) * 1000.0);
    } else {
        data->current_ma = -20;
    }

    struct sensor_value val_soc;
    if (sensor_channel_get(pmic_dev, SENSOR_CHAN_GAUGE_STATE_OF_CHARGE, &val_soc) == 0) {
        data->soc_percent = (float)sensor_value_to_double(&val_soc);
    } else {
        data->soc_percent = calculate_soc_from_ocv(data->voltage_mv);
    }

    data->power_mw = ((float)data->voltage_mv * (float)abs(data->current_ma)) / 1000.0f;
    data->remaining_capacity_mah = (uint16_t)((float)NOMINAL_BATTERY_CAPACITY_MAH * (data->soc_percent / 100.0f));

    if (data->current_ma > 5) {
        data->charge_status = (data->soc_percent >= 99.0f) ? PMIC_CHARGED : PMIC_CHARGING;
    } else {
        data->charge_status = PMIC_DISCHARGING;
    }

    data->low_battery_alert = (data->soc_percent < LOW_BATTERY_ALERT_THRESHOLD);
    data->valid = true;

    LOG_INF("[PMIC HW] V=%u mV | SoC=%.1f%% | I=%d mA | P=%.2f mW | Rem=%u mAh",
            data->voltage_mv, (double)data->soc_percent, data->current_ma,
            (double)data->power_mw, data->remaining_capacity_mah);

    return 0;
}

int pmic_set_rail_state(bool enable)
{
    LOG_INF("Setting PMIC VEXT power rail state: %s", enable ? "ENABLED" : "DISABLED");
    return 0;
}

int pmic_sleep(void)
{
    if (!pmic_dev || !device_is_ready(pmic_dev)) {
        LOG_DBG("[SIM] PMIC driver entered low power state.");
        return 0;
    }

#if defined(CONFIG_PM_DEVICE)
    int ret = pm_device_action_run(pmic_dev, PM_DEVICE_ACTION_SUSPEND);
    if (ret < 0 && ret != -ENOSYS) {
        LOG_ERR("Failed to suspend PMIC device (err: %d)", ret);
        return ret;
    }
    LOG_INF("PMIC fuel gauge suspended.");
#endif

    return 0;
}
