#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "app.h"
#include "app_config.h"

LOG_MODULE_REGISTER(app_env_monitor);

static struct k_work_delayable sensor_sample_work;
static struct k_work_delayable uplink_work;

static void sensor_sample_handler(struct k_work *work)
{
    LOG_INF("Waking up sensor buses, reading gas, temperature, humidity...");
    /* TODO: Wake up I2C bus & sensors, take sample, put sensors back to sleep */

    LOG_INF("Saving data to Non-Volatile Storage / LittleFS queue...");

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
