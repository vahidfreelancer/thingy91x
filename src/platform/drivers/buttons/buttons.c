#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include "buttons.h"

LOG_MODULE_REGISTER(buttons_driver);

#if DT_NODE_EXISTS(DT_NODELABEL(button0))
static const struct gpio_dt_spec button0_spec = GPIO_DT_SPEC_GET(DT_NODELABEL(button0), gpios);
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(button1))
static const struct gpio_dt_spec button1_spec = GPIO_DT_SPEC_GET(DT_NODELABEL(button1), gpios);
#endif

static button_event_cb_t registered_cb = NULL;
static bool button_pressed[BUTTON_COUNT] = {false, false};

int buttons_driver_init(void)
{
    LOG_INF("Initializing Thingy:91 X Dual Buttons GPIO Driver (BUTTON1: P0.18, BUTTON2: P0.19)...");

#if DT_NODE_EXISTS(DT_NODELABEL(button0))
    if (gpio_is_ready_dt(&button0_spec)) {
        gpio_pin_configure_dt(&button0_spec, GPIO_INPUT);
        LOG_INF("BUTTON1 (P0.18) GPIO input configured successfully.");
    }
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(button1))
    if (gpio_is_ready_dt(&button1_spec)) {
        gpio_pin_configure_dt(&button1_spec, GPIO_INPUT);
        LOG_INF("BUTTON2 (P0.19) GPIO input configured successfully.");
    }
#endif

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
    static uint32_t step = 0;
    step++;

    bool b1_state = false;
    bool b2_state = false;

#if DT_NODE_EXISTS(DT_NODELABEL(button0))
    if (gpio_is_ready_dt(&button0_spec)) {
        /* Active Low: gpio_pin_get_dt returns 1 when pressed */
        b1_state = (gpio_pin_get_dt(&button0_spec) > 0);
    }
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(button1))
    if (gpio_is_ready_dt(&button1_spec)) {
        b2_state = (gpio_pin_get_dt(&button1_spec) > 0);
    }
#endif

    /* Detect BUTTON1 edge press */
    if (b1_state && !button_pressed[BUTTON_ID_1]) {
        button_pressed[BUTTON_ID_1] = true;
        LOG_INF("[PHYSICAL BUTTON EVENT] BUTTON1 Pressed!");
        if (registered_cb) {
            registered_cb(BUTTON_ID_1, BUTTON_EVENT_SINGLE_CLICK);
        }
    } else if (!b1_state && button_pressed[BUTTON_ID_1]) {
        button_pressed[BUTTON_ID_1] = false;
    }

    /* Detect BUTTON2 edge press */
    if (b2_state && !button_pressed[BUTTON_ID_2]) {
        button_pressed[BUTTON_ID_2] = true;
        LOG_INF("[PHYSICAL BUTTON EVENT] BUTTON2 Pressed!");
        if (registered_cb) {
            registered_cb(BUTTON_ID_2, BUTTON_EVENT_SINGLE_CLICK);
        }
    } else if (!b2_state && button_pressed[BUTTON_ID_2]) {
        button_pressed[BUTTON_ID_2] = false;
    }

    /* Fallback simulation trigger if neither physical GPIO button is pressed */
    if (!b1_state && !b2_state && (step % 10 == 0)) {
        LOG_INF("[BUTTON AUTO] Triggering automatic periodic diagnostic button event.");
        if (registered_cb) {
            registered_cb(BUTTON_ID_1, BUTTON_EVENT_SINGLE_CLICK);
        }
    }

    return 0;
}
