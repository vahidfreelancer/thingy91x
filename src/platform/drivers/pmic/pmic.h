#ifndef PMIC_H
#define PMIC_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief PMIC Charger and Power Source Status.
 */
enum pmic_charge_status {
    PMIC_DISCHARGING = 0,   /**< Running on battery power */
    PMIC_CHARGING    = 1,   /**< External power connected, battery charging */
    PMIC_CHARGED     = 2,   /**< Battery fully charged on external power */
    PMIC_POWERED_USB = 3    /**< Powered via USB, no battery present */
};

/**
 * @brief PMIC & Battery Fuel Gauge measurement data structure.
 */
struct pmic_battery_data {
    uint16_t voltage_mv;            /**< Battery cell terminal voltage in mV */
    int16_t current_ma;             /**< Charge (+mA) or discharge (-mA) current */
    float soc_percent;              /**< Battery State of Charge percentage (0.0% to 100.0%) */
    float power_mw;                 /**< Instantaneous power draw P = V * I in mW */
    uint16_t remaining_capacity_mah;/**< Estimated remaining energy capacity in mAh */
    enum pmic_charge_status charge_status; /**< Charging status */
    bool low_battery_alert;         /**< True if soc_percent < 15.0% */
    bool valid;                     /**< True if sensor sample read successfully */
};

/**
 * @brief Initialize the PMIC and Battery Fuel Gauge driver.
 * 
 * Searches Devicetree for compatible PMIC/Fuel Gauge nodes (Nordic nPM1300, Maxim MAX17048, ADI ADP5360)
 * and verifies device readiness. Registers software simulation fallback if hardware chip is absent.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int pmic_init(void);

/**
 * @brief Read battery cell voltage, current, compute SoC %, power P=V*I, and check alerts.
 * 
 * Uses mathematical OCV curve interpolation formulas for SoC estimation and calculates power draw.
 * 
 * @param data Pointer to pmic_battery_data structure to populate.
 * @return 0 on success, negative POSIX error code on failure.
 */
int pmic_read(struct pmic_battery_data *data);

/**
 * @brief Enable or disable external sensor VEXT power rail.
 * 
 * @param enable True to turn on VEXT power rail, false to disable.
 * @return 0 on success, negative POSIX error code on failure.
 */
int pmic_set_rail_state(bool enable);

/**
 * @brief Put PMIC driver interface into low-power suspended state.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int pmic_sleep(void);

#ifdef __cplusplus
}
#endif

#endif /* PMIC_H */
