#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Animated RGB LED visual status patterns.
 */
enum led_pattern {
    LED_PATTERN_OFF = 0,        /**< LED turned completely off */
    LED_PATTERN_SOLID,          /**< Continuous solid RGB color */
    LED_PATTERN_BLINK_SLOW,     /**< Slow 1 Hz blinking pattern */
    LED_PATTERN_BLINK_FAST,     /**< Fast 4 Hz alert flashing pattern */
    LED_PATTERN_BREATHE,        /**< Sinusoidal brightness breathing pattern */
    LED_PATTERN_BOOT,           /**< Power-on boot rainbow sequence */
    LED_PATTERN_ERROR           /**< Red error alert strobe */
};

/**
 * @brief RGB color structure (0 - 255 channel levels).
 */
struct led_color {
    uint8_t r;  /**< Red channel (0 - 255) */
    uint8_t g;  /**< Green channel (0 - 255) */
    uint8_t b;  /**< Blue channel (0 - 255) */
};

/* Predefined Color Definitions */
#define LED_COLOR_OFF      (struct led_color){0, 0, 0}
#define LED_COLOR_RED      (struct led_color){255, 0, 0}
#define LED_COLOR_GREEN    (struct led_color){0, 255, 0}
#define LED_COLOR_BLUE     (struct led_color){0, 0, 255}
#define LED_COLOR_YELLOW   (struct led_color){255, 255, 0}
#define LED_COLOR_CYAN     (struct led_color){0, 255, 255}
#define LED_COLOR_MAGENTA  (struct led_color){255, 0, 255}
#define LED_COLOR_WHITE    (struct led_color){255, 255, 255}

/**
 * @brief Initialize the RGB LED hardware driver.
 * 
 * Queries Devicetree for PWM/GPIO/nPM1300 RGB LED nodes and verifies device readiness.
 * Registers simulation fallback if physical LED controller is unattached.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int led_driver_init(void);

/**
 * @brief Set a static raw RGB color.
 * 
 * @param red Red intensity (0 - 255)
 * @param green Green intensity (0 - 255)
 * @param blue Blue intensity (0 - 255)
 * @return 0 on success, negative POSIX error code on failure.
 */
int led_set_color(uint8_t red, uint8_t green, uint8_t blue);

/**
 * @brief Set an animated visual status pattern with target RGB color.
 * 
 * @param pattern Animation pattern (solid, blink slow/fast, breathe, boot, error).
 * @param red Base Red color intensity.
 * @param green Base Green color intensity.
 * @param blue Base Blue color intensity.
 * @return 0 on success, negative POSIX error code on failure.
 */
int led_set_pattern(enum led_pattern pattern, uint8_t red, uint8_t green, uint8_t blue);

/**
 * @brief Service pattern animation step. Call periodically (e.g. every 100ms).
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int led_update(void);

/**
 * @brief Turn off all RGB LED channels.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int led_off(void);

#ifdef __cplusplus
}
#endif

#endif /* LED_H */
