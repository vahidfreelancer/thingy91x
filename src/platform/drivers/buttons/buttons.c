#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include "buttons.h"

LOG_MODULE_REGISTER(buttons_driver);

#if DT_NODE_EXISTS(DT_NODELABEL(button0))
static const struct gpio_dt_spec button0_spec = GPIO_DT_SPEC_GET(DT_NODELABEL(button0), gpios);
static struct gpio_callback button0_cb_data;
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(button1))
static const struct gpio_dt_spec button1_spec = GPIO_DT_SPEC_GET(DT_NODELABEL(button1), gpios);
static struct gpio_callback button1_cb_data;
#endif

static button_event_cb_t registered_cb = NULL;
static bool button_pressed[BUTTON_COUNT] = {false, false};

#if DT_NODE_EXISTS(DT_NODELABEL(button0))
static void button0_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    LOG_INF("[PHYSICAL BUTTON INTERRUPT] BUTTON1 (P0.18) Pressed!");
    button_pressed[BUTTON_ID_1] = true;
    if (registered_cb) {
        registered_cb(BUTTON_ID_1, BUTTON_EVENT_SINGLE_CLICK);
    }
}
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(button1))
static void button1_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    LOG_INF("[PHYSICAL BUTTON INTERRUPT] BUTTON2 (P0.19) Pressed!");
    button_pressed[BUTTON_ID_2] = true;
    if (registered_cb) {
        registered_cb(BUTTON_ID_2, BUTTON_EVENT_SINGLE_CLICK);
    }
}
#endif

int buttons_driver_init(void)
{
    LOG_INF("Initializing Thingy:91 X Dual Buttons Hardware Interrupt Driver (BUTTON1: P0.18, BUTTON2: P0.19)...");

#if DT_NODE_EXISTS(DT_NODELABEL(button0))
    if (gpio_is_ready_dt(&button0_spec)) {
        gpio_pin_configure_dt(&button0_spec, GPIO_INPUT | button0_spec.dt_flags);
        gpio_pin_interrupt_configure_dt(&button0_spec, GPIO_INT_EDGE_TO_ACTIVE);
        gpio_init_callback(&button0_cb_data, button0_isr, BIT(button0_spec.pin));
        gpio_add_callback(button0_spec.port, &button0_cb_data);
        LOG_INF("BUTTON1 (P0.18) Pull-Up & Hardware Interrupt Configured.");
    }
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(button1))
    if (gpio_is_ready_dt(&button1_spec)) {
        gpio_pin_configure_dt(&button1_spec, GPIO_INPUT | button1_spec.dt_flags);
        gpio_pin_interrupt_configure_dt(&button1_spec, GPIO_INT_EDGE_TO_ACTIVE);
        gpio_init_callback(&button1_cb_data, button1_isr, BIT(button1_spec.pin));
        gpio_add_callback(button1_spec.port, &button1_cb_data);
        LOG_INF("BUTTON2 (P0.19) Pull-Up & Hardware Interrupt Configured.");
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
    /* Driven strictly by native Zephyr GPIO hardware interrupts (button0_isr and button1_isr) */
    return 0;
}
