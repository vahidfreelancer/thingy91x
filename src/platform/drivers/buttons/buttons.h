#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Identifiers for User Push Buttons.
 */
enum button_id {
    BUTTON_ID_1 = 0,    /**< User Button 1 (BUTTON1) */
    BUTTON_ID_2 = 1,    /**< User Button 2 (BUTTON2) */
    BUTTON_COUNT
};

/**
 * @brief User Button interaction event types.
 */
enum button_event {
    BUTTON_EVENT_SINGLE_CLICK = 0,  /**< Single short press & release (< 500ms) */
    BUTTON_EVENT_DOUBLE_CLICK,      /**< Two rapid presses within 400ms */
    BUTTON_EVENT_LONG_PRESS,        /**< Sustained hold press (> 1500ms) */
    BUTTON_EVENT_RELEASED           /**< Button release transition */
};

/**
 * @brief Function pointer callback type for button events.
 * 
 * @param id Button identifier (BUTTON_ID_1 or BUTTON_ID_2).
 * @param event Button event type (single click, double click, long press, released).
 */
typedef void (*button_event_cb_t)(enum button_id id, enum button_event event);

/**
 * @brief Initialize the Dual User Buttons driver.
 * 
 * Queries Devicetree for GPIO button input nodes for BUTTON1 and BUTTON2, configures active-low
 * GPIO interrupts with debouncing logic. Registers simulation fallback if hardware buttons are unattached.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int buttons_driver_init(void);

/**
 * @brief Register callback handler for button press events.
 * 
 * @param callback Function pointer to button event handler.
 * @return 0 on success, negative POSIX error code on failure.
 */
int buttons_register_callback(button_event_cb_t callback);

/**
 * @brief Read instantaneous physical state of specified button.
 * 
 * @param id Button identifier (BUTTON_ID_1 or BUTTON_ID_2).
 * @param is_pressed Pointer to bool output (true if currently held down).
 * @return 0 on success, negative POSIX error code on failure.
 */
int buttons_read_state(enum button_id id, bool *is_pressed);

/**
 * @brief Service button debouncing and hold duration timers. Call periodically.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int buttons_update(void);

#ifdef __cplusplus
}
#endif

#endif /* BUTTONS_H */
