#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "app.h"
#include "app_config.h"

LOG_MODULE_REGISTER(app_asset_tracker);

static struct k_work_delayable telemetry_work;

static void telemetry_work_handler(struct k_work *work)
{
    LOG_INF("Sampling environmental & location sensors...");
    /* TODO: Call location and telemetry services */

    LOG_INF("Publishing telemetry to cloud...");

    /* Reschedule telemetry publishing */
    k_work_reschedule(&telemetry_work, K_SECONDS(TELEMETRY_PUBLISH_INTERVAL_SEC));
}

int app_init(void)
{
    LOG_INF("Initializing Asset Tracker Profile...");
    
    /* Initialize workqueue structure */
    k_work_init_delayable(&telemetry_work, telemetry_work_handler);
    
    return 0;
}

void app_run(void)
{
    LOG_INF("Running Asset Tracker...");
    
    /* Start periodic telemetry collection */
    k_work_reschedule(&telemetry_work, K_NO_WAIT);

    /* Yield the main thread permanently; background workqueues handle scheduling */
    while (1) {
        k_sleep(K_FOREVER);
    }
}
