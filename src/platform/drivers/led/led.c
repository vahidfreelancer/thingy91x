#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <math.h>
#include "led.h"

LOG_MODULE_REGISTER(led_driver);

static const struct device *led_dev = NULL;
static struct led_color current_color = {0, 0, 0};
static enum led_pattern current_pattern = LED_PATTERN_OFF;
static uint32_t anim_step = 0;

int led_driver_init(void)
{
    /* Query Devicetree for LED devices */
#if DT_HAS_COMPAT_STATUS_OKAY(gpio_leds)
    led_dev = DEVICE_DT_GET_ANY(gpio_leds);
#elif DT_HAS_COMPAT_STATUS_OKAY(pwm_leds)
    led_dev = DEVICE_DT_GET_ANY(pwm_leds);
#endif

    if (led_dev && device_is_ready(led_dev)) {
        LOG_INF("RGB LED controller '%s' initialized successfully.", led_dev->name);
    } else {
        LOG_WRN("Physical RGB LED hardware unattached or not ready. Falling back to software simulation.");
        led_dev = NULL;
    }

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
            /* 1 Hz blink: On 5 steps (500ms), Off 5 steps (500ms) */
            if ((anim_step % 10) < 5) {
                out_r = current_color.r;
                out_g = current_color.g;
                out_b = current_color.b;
            }
            break;

        case LED_PATTERN_BLINK_FAST:
            /* 4 Hz fast strobe: On 1 step (100ms), Off 1 step (100ms) */
            if ((anim_step % 2) == 0) {
                out_r = current_color.r;
                out_g = current_color.g;
                out_b = current_color.b;
            }
            break;

        case LED_PATTERN_BREATHE: {
            /* Sinusoidal brightness fading: factor = (1 + sin(step * 0.2)) / 2 */
            float factor = (1.0f + (float)sin((double)anim_step * 0.2)) / 2.0f;
            out_r = (uint8_t)((float)current_color.r * factor);
            out_g = (uint8_t)((float)current_color.g * factor);
            out_b = (uint8_t)((float)current_color.b * factor);
            break;
        }

        case LED_PATTERN_BOOT: {
            /* Rainbow color rotation sequence */
            int phase = (anim_step / 2) % 3;
            if (phase == 0)      { out_r = 255; out_g = 0;   out_b = 0;   } /* Red */
            else if (phase == 1) { out_r = 0;   out_g = 255; out_b = 0;   } /* Green */
            else                 { out_r = 0;   out_g = 0;   out_b = 255; } /* Blue */
            break;
        }

        case LED_PATTERN_ERROR:
            /* Red emergency alert strobe */
            if ((anim_step % 2) == 0) {
                out_r = 255; out_g = 0; out_b = 0;
            }
            break;

        default:
            break;
    }

    if (!led_dev) {
        LOG_DBG("[RGB LED ANIM] Step %u | Active Levels: R=%u, G=%u, B=%u",
                anim_step, out_r, out_g, out_b);
    }

    return 0;
}

int led_off(void)
{
    current_pattern = LED_PATTERN_OFF;
    current_color = (struct led_color){0, 0, 0};
    LOG_INF("[RGB LED] Turned OFF all channels.");
    return 0;
}
