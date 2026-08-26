#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "app.h"
#include "app_config.h"
#include "high_g.h"
#include "pmic.h"
#include "led.h"
#include "buttons.h"

LOG_MODULE_REGISTER(app_asset_tracker);

static struct k_work_delayable telemetry_work;
static struct high_g_data motion_data;
static struct pmic_battery_data batt_data;

static void on_button_event(enum button_id id, enum button_event event)
{
    const char *btn_name = (id == BUTTON_ID_1) ? "BUTTON1" : "BUTTON2";
    const char *evt_name = "UNKNOWN";

    switch (event) {
        case BUTTON_EVENT_SINGLE_CLICK: evt_name = "SINGLE_CLICK"; break;
        case BUTTON_EVENT_DOUBLE_CLICK: evt_name = "DOUBLE_CLICK"; break;
        case BUTTON_EVENT_LONG_PRESS:   evt_name = "LONG_PRESS";   break;
        case BUTTON_EVENT_RELEASED:     evt_name = "RELEASED";     break;
    }

    LOG_INF("[USER BUTTON EVENT] %s -> %s", btn_name, evt_name);
}

static void telemetry_work_handler(struct k_work *work)
{
    LOG_INF("Sampling High-G impact, PMIC battery, servicing LED & Buttons...");

    /* Update RGB LED pattern animation step */
    led_update();

    /* Service button debouncing & events */
    buttons_update();

    /* Read High-G Impact metrics */
    int err = high_g_read(&motion_data);
    if (err == 0 && motion_data.valid) {
        LOG_INF("[HIGH-G METRICS] Mag: %.2f g | Peak: %.2f g | Ax: %.2f | Ay: %.2f | Az: %.2f",
                (double)motion_data.magnitude, (double)motion_data.peak_g,
                (double)motion_data.accel_x, (double)motion_data.accel_y, (double)motion_data.accel_z);

        if (motion_data.impact_detected) {
            LOG_WRN("!!! HIGH-G IMPACT SHOCK DETECTED !!! Magnitude = %.2f g", (double)motion_data.magnitude);
            led_set_pattern(LED_PATTERN_BLINK_FAST, 255, 0, 0);
        } else {
            led_set_pattern(LED_PATTERN_BREATHE, 0, 255, 0);
        }
    } else {
        LOG_ERR("Failed to read High-G motion sensor (err: %d)", err);
    }
    high_g_sleep();

    /* Read PMIC & Battery metrics */
    err = pmic_read(&batt_data);
    if (err == 0 && batt_data.valid) {
        LOG_INF("[BATTERY METRICS] V: %u mV | SoC: %.1f %% | I: %d mA | Power: %.2f mW | Rem: %u mAh",
                batt_data.voltage_mv, (double)batt_data.soc_percent, batt_data.current_ma,
                (double)batt_data.power_mw, batt_data.remaining_capacity_mah);

        if (batt_data.low_battery_alert) {
            LOG_WRN("!!! LOW BATTERY WARNING !!! SoC = %.1f %% (< 15.0%%)", (double)batt_data.soc_percent);
        }
    } else {
        LOG_ERR("Failed to read PMIC Fuel Gauge (err: %d)", err);
    }
    pmic_sleep();

    /* Reschedule telemetry publishing */
    k_work_reschedule(&telemetry_work, K_SECONDS(TELEMETRY_PUBLISH_INTERVAL_SEC));
}

int app_init(void)
{
    LOG_INF("Initializing Asset Tracker Profile with Buttons, LED, PMIC & High-G Drivers...");
    
    int err = buttons_driver_init();
    if (err < 0) {
        LOG_ERR("Failed to initialize Dual Buttons driver (err: %d)", err);
    } else {
        buttons_register_callback(on_button_event);
    }

    err = led_driver_init();
    if (err < 0) {
        LOG_ERR("Failed to initialize RGB LED HAL driver (err: %d)", err);
    }
    led_set_pattern(LED_PATTERN_BREATHE, 0, 255, 0);

    err = high_g_init();
    if (err < 0) {
        LOG_ERR("Failed to initialize High-G HAL driver (err: %d)", err);
    }

    err = pmic_init();
    if (err < 0) {
        LOG_ERR("Failed to initialize PMIC driver (err: %d)", err);
    }
    
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
