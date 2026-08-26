#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include "buttons.h"

LOG_MODULE_REGISTER(buttons_driver);

#define DEBOUNCE_TIME_MS      50     /* 50ms software debouncing noise filter */
#define LONG_PRESS_TIME_MS    1500   /* 1.5s long press threshold */
#define DOUBLE_CLICK_TIME_MS  400    /* 400ms double-click window */

static const struct device *btn_dev = NULL;
static button_event_cb_t registered_cb = NULL;

static bool button_pressed[BUTTON_COUNT] = {false, false};
static uint32_t press_start_time[BUTTON_COUNT] = {0, 0};
static uint32_t last_release_time[BUTTON_COUNT] = {0, 0};
static uint8_t click_count[BUTTON_COUNT] = {0, 0};

int buttons_driver_init(void)
{
    /* Query Devicetree for button keys node */
#if defined(CONFIG_INPUT_GPIO_KEYS) && DT_HAS_COMPAT_STATUS_OKAY(gpio_keys)
    btn_dev = DEVICE_DT_GET_ANY(gpio_keys);
#else
    btn_dev = NULL;
#endif

    if (btn_dev && device_is_ready(btn_dev)) {
        LOG_INF("Dual Buttons controller '%s' initialized successfully.", btn_dev->name);
    } else {
        LOG_WRN("Physical User Buttons unattached or not ready. Falling back to software simulation.");
        btn_dev = NULL;
    }

    button_pressed[BUTTON_ID_1] = false;
    button_pressed[BUTTON_ID_2] = false;
    registered_cb = NULL;

    return 0;
}

int buttons_register_callback(button_event_cb_t callback)
{
    registered_cb = callback;
    LOG_INF("Registered Dual User Buttons event callback handler.");
    return 0;
}

int buttons_read_state(enum button_id id, bool *is_pressed)
{
    if (id >= BUTTON_COUNT || !is_pressed) {
        return -EINVAL;
    }

    *is_pressed = button_pressed[id];
    return 0;
}

int buttons_update(void)
{
    static uint32_t sim_step = 0;
    sim_step++;

    if (!btn_dev) {
        /* Software simulation: Trigger simulated BUTTON1 single-click every 8 steps */
        if (sim_step % 8 == 0) {
            LOG_INF("[BUTTON SIM] Simulated BUTTON1 single-click event triggered.");
            if (registered_cb) {
                registered_cb(BUTTON_ID_1, BUTTON_EVENT_SINGLE_CLICK);
            }
        }
        /* Trigger simulated BUTTON2 long-press every 15 steps */
        if (sim_step % 15 == 0) {
            LOG_INF("[BUTTON SIM] Simulated BUTTON2 long-press event triggered.");
            if (registered_cb) {
                registered_cb(BUTTON_ID_2, BUTTON_EVENT_LONG_PRESS);
            }
        }
        return 0;
    }

    return 0;
}
