#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "app.h"
#include "app_config.h"
#include "environment.h"

LOG_MODULE_REGISTER(app_env_monitor);

static struct k_work_delayable sensor_sample_work;
static struct k_work_delayable uplink_work;
static struct env_sensor_data env_data;

static void sensor_sample_handler(struct k_work *work)
{
    LOG_INF("Sampling environmental sensor data...");

    int err = env_sensor_read(&env_data);
    if (err == 0 && env_data.valid) {
        LOG_INF("[ENV DATA] Temp: %.2f °C | Humidity: %.2f %% | Pressure: %.2f hPa | Gas: %.0f Ω | IAQ: %u",
                (double)env_data.temperature, (double)env_data.humidity,
                (double)env_data.pressure, (double)env_data.gas_resistance,
                env_data.iaq_index);
    } else {
        LOG_ERR("Failed to read environmental measurements (err: %d)", err);
    }

    /* Put sensor into low-power mode until next sample interval */
    env_sensor_sleep();

    LOG_INF("Saving telemetry to storage queue...");

    /* Reschedule next sensor sampling */
    k_work_reschedule(&sensor_sample_work, K_SECONDS(SENSOR_READ_INTERVAL_SEC));
}

static void uplink_handler(struct k_work *work)
{
    LOG_INF("Initializing modem connection for periodic report...");
    /* TODO: Connect to LTE, fetch queued data from flash, serialize to CBOR, transmit */

    LOG_INF("Modem uplink complete. Returning to low-power Sleep/PSM mode.");

    /* Reschedule next uplink */
    k_work_reschedule(&uplink_work, K_SECONDS(UPLINK_INTERVAL_SEC));
}

int app_init(void)
{
    LOG_INF("Initializing Environmental Monitor Profile (Ultra-Low Power)...");

    int err = env_sensor_init();
    if (err < 0) {
        LOG_ERR("Failed to initialize environmental sensor HAL (err: %d)", err);
    }

    k_work_init_delayable(&sensor_sample_work, sensor_sample_handler);
    k_work_init_delayable(&uplink_work, uplink_handler);

    return 0;
}

void app_run(void)
{
    LOG_INF("Running Environmental Monitor Node...");

    /* Schedule initial triggers */
    k_work_reschedule(&sensor_sample_work, K_NO_WAIT);
    k_work_reschedule(&uplink_work, K_SECONDS(10)); /* First uplink in 10s */

    /* Yield main thread permanently; the system enters low power states automatically */
    while (1) {
        k_sleep(K_FOREVER);
    }
}
