#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <math.h>
#include "led.h"

LOG_MODULE_REGISTER(led_driver);

#if DT_NODE_EXISTS(DT_NODELABEL(red_led))
static const struct gpio_dt_spec red_led_spec   = GPIO_DT_SPEC_GET(DT_NODELABEL(red_led), gpios);
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(green_led))
static const struct gpio_dt_spec green_led_spec = GPIO_DT_SPEC_GET(DT_NODELABEL(green_led), gpios);
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(blue_led))
static const struct gpio_dt_spec blue_led_spec  = GPIO_DT_SPEC_GET(DT_NODELABEL(blue_led), gpios);
#endif

static struct led_color current_color = {0, 0, 0};
static enum led_pattern current_pattern = LED_PATTERN_OFF;
static uint32_t anim_step = 0;

int led_driver_init(void)
{
    LOG_INF("Initializing Thingy:91 X RGB LED GPIO Driver (Red: P0.29, Green: P0.31, Blue: P0.30)...");

#if DT_NODE_EXISTS(DT_NODELABEL(red_led))
    if (gpio_is_ready_dt(&red_led_spec)) {
        gpio_pin_configure_dt(&red_led_spec, GPIO_OUTPUT_INACTIVE);
    }
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(green_led))
    if (gpio_is_ready_dt(&green_led_spec)) {
        gpio_pin_configure_dt(&green_led_spec, GPIO_OUTPUT_INACTIVE);
    }
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(blue_led))
    if (gpio_is_ready_dt(&blue_led_spec)) {
        gpio_pin_configure_dt(&blue_led_spec, GPIO_OUTPUT_INACTIVE);
    }
#endif

    current_pattern = LED_PATTERN_OFF;
    current_color = (struct led_color){0, 0, 0};
    anim_step = 0;

    return 0;
}

int led_set_color(uint8_t red, uint8_t green, uint8_t blue)
{
    current_color.r = red;
    current_color.g = green;
    current_color.b = blue;
    current_pattern = LED_PATTERN_SOLID;

    LOG_INF("[RGB LED] Set solid color: R=%u, G=%u, B=%u", red, green, blue);
    return 0;
}

int led_set_pattern(enum led_pattern pattern, uint8_t red, uint8_t green, uint8_t blue)
{
    current_pattern = pattern;
    current_color.r = red;
    current_color.g = green;
    current_color.b = blue;
    anim_step = 0;

    const char *pattern_str = "OFF";
    switch (pattern) {
        case LED_PATTERN_SOLID:      pattern_str = "SOLID"; break;
        case LED_PATTERN_BLINK_SLOW: pattern_str = "BLINK_SLOW (1Hz)"; break;
        case LED_PATTERN_BLINK_FAST: pattern_str = "BLINK_FAST (4Hz)"; break;
        case LED_PATTERN_BREATHE:    pattern_str = "BREATHE (Sinusoidal)"; break;
        case LED_PATTERN_BOOT:       pattern_str = "BOOT_RAINBOW"; break;
        case LED_PATTERN_ERROR:      pattern_str = "ERROR_STROBE"; break;
        default: break;
    }

    LOG_INF("[RGB LED PATTERN] Switched to pattern '%s' | Base Color R=%u, G=%u, B=%u",
            pattern_str, red, green, blue);
    return 0;
}

int led_update(void)
{
    anim_step++;

    if (current_pattern == LED_PATTERN_OFF) {
#if DT_NODE_EXISTS(DT_NODELABEL(red_led))
        if (gpio_is_ready_dt(&red_led_spec)) gpio_pin_set_dt(&red_led_spec, 0);
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(green_led))
        if (gpio_is_ready_dt(&green_led_spec)) gpio_pin_set_dt(&green_led_spec, 0);
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(blue_led))
        if (gpio_is_ready_dt(&blue_led_spec)) gpio_pin_set_dt(&blue_led_spec, 0);
#endif
        return 0;
    }

    uint8_t out_r = 0, out_g = 0, out_b = 0;

    switch (current_pattern) {
        case LED_PATTERN_SOLID:
            out_r = current_color.r;
            out_g = current_color.g;
            out_b = current_color.b;
            break;

        case LED_PATTERN_BLINK_SLOW:
            if ((anim_step % 10) < 5) {
                out_r = current_color.r;
                out_g = current_color.g;
                out_b = current_color.b;
            }
            break;

        case LED_PATTERN_BLINK_FAST:
            if ((anim_step % 2) == 0) {
                out_r = current_color.r;
                out_g = current_color.g;
                out_b = current_color.b;
            }
            break;

        case LED_PATTERN_BREATHE: {
            float factor = (1.0f + (float)sin((double)anim_step * 0.2)) / 2.0f;
            out_r = (uint8_t)((float)current_color.r * factor);
            out_g = (uint8_t)((float)current_color.g * factor);
            out_b = (uint8_t)((float)current_color.b * factor);
            break;
        }

        case LED_PATTERN_BOOT: {
            int phase = (anim_step / 2) % 3;
            if (phase == 0)      { out_r = 255; out_g = 0;   out_b = 0;   }
            else if (phase == 1) { out_r = 0;   out_g = 255; out_b = 0;   }
            else                 { out_r = 0;   out_g = 0;   out_b = 255; }
            break;
        }

        case LED_PATTERN_ERROR:
            if ((anim_step % 2) == 0) {
                out_r = 255; out_g = 0; out_b = 0;
            }
            break;

        default:
            break;
    }

    /* Drive physical GPIO pins on Thingy:91 X */
#if DT_NODE_EXISTS(DT_NODELABEL(red_led))
    if (gpio_is_ready_dt(&red_led_spec)) gpio_pin_set_dt(&red_led_spec, out_r > 127 ? 1 : 0);
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(green_led))
    if (gpio_is_ready_dt(&green_led_spec)) gpio_pin_set_dt(&green_led_spec, out_g > 127 ? 1 : 0);
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(blue_led))
    if (gpio_is_ready_dt(&blue_led_spec)) gpio_pin_set_dt(&blue_led_spec, out_b > 127 ? 1 : 0);
#endif

    return 0;
}

int led_off(void)
{
    current_pattern = LED_PATTERN_OFF;
    current_color = (struct led_color){0, 0, 0};

#if DT_NODE_EXISTS(DT_NODELABEL(red_led))
    if (gpio_is_ready_dt(&red_led_spec)) gpio_pin_set_dt(&red_led_spec, 0);
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(green_led))
    if (gpio_is_ready_dt(&green_led_spec)) gpio_pin_set_dt(&green_led_spec, 0);
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(blue_led))
    if (gpio_is_ready_dt(&blue_led_spec)) gpio_pin_set_dt(&blue_led_spec, 0);
#endif

    LOG_INF("[RGB LED] Turned OFF all physical channels.");
    return 0;
}
