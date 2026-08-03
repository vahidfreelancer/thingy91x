#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app);

int app_init(void)
{
    LOG_INF("Application initialized");
    return 0;
}

void app_run(void)
{
    while (1)
    {
        LOG_INF("Hello World!");
        k_sleep(K_SECONDS(1));
    }
}