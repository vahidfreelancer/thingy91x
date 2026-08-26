#ifndef CELLULAR_H
#define CELLULAR_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Cellular Modem Radio Access Technologies (RAT).
 */
enum cellular_modem_mode {
    CELLULAR_MODE_OFFLINE = 0,      /**< Flight mode / offline */
    CELLULAR_MODE_LTE_M,            /**< LTE-M (eMTC) Cat-M1 */
    CELLULAR_MODE_NB_IOT,           /**< NB-IoT Cat-NB1/NB2 */
    CELLULAR_MODE_DECT_NR_PLUS      /**< DECT NR+ non-cellular mesh */
};

/**
 * @brief Cellular Signal Quality and Serving Cell Information.
 */
struct cellular_signal_info {
    int16_t rsrp_dbm;       /**< Reference Signal Received Power (-140 to -44 dBm) */
    int16_t rsrq_db;        /**< Reference Signal Received Quality (-20 to -3 dB) */
    int16_t snr_db;         /**< Signal-to-Noise Ratio (0 to 30 dB) */
    uint32_t cell_id;       /**< 28-bit E-UTRAN Cell Identifier */
    uint16_t mcc;           /**< Mobile Country Code (e.g. 242) */
    uint16_t mnc;           /**< Mobile Network Code (e.g. 01) */
    bool registered;        /**< True if attached to LTE cellular network */
    bool valid;             /**< True if signal measurement read successfully */
};

/**
 * @brief Network Metadata and Operator Configuration.
 */
struct cellular_network_metadata {
    char operator_name[32]; /**< Network Operator name (e.g. "Telenor", "AT&T", "Vodafone") */
    char rat_name[16];      /**< Active Radio Access Technology ("LTE-M", "NB-IoT") */
    uint16_t band_number;   /**< Operating LTE E-UTRA Band (e.g. Band 3, 8, 20) */
    uint16_t tac;           /**< Tracking Area Code (TAC) */
    char ip_address[16];    /**< Assigned Cellular PDN IP address */
    bool roaming;           /**< True if roaming on partner network */
};

/**
 * @brief Neighboring Cellular Station Information (for cellular locationing).
 */
struct cellular_neighbor_cell {
    uint32_t cell_id;       /**< E-UTRAN Cell ID */
    uint16_t pci;           /**< Physical Cell ID (0 - 503) */
    int16_t rsrp_dbm;       /**< Signal RSRP (dBm) */
    int16_t rsrq_db;        /**< Signal RSRQ (dB) */
    uint16_t earfcn;        /**< E-UTRA Absolute Radio Frequency Channel Number */
};

/**
 * @brief Neighboring Cellular Stations Scan Results.
 */
struct cellular_neighbor_scan {
    uint16_t station_count;                     /**< Number of neighboring stations found */
    struct cellular_neighbor_cell stations[4];  /**< Array of neighboring cell stations */
    bool valid;                                 /**< True if neighbor scan completed cleanly */
};

/**
 * @brief Initialize the nRF9151 Cellular Modem Controller Driver.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int cellular_modem_init(void);

/**
 * @brief Connect to LTE cellular network in specified mode (LTE-M / NB-IoT).
 * 
 * @param mode Desired Radio Access Technology mode.
 * @return 0 on success, negative POSIX error code on failure.
 */
int cellular_modem_connect(enum cellular_modem_mode mode);

/**
 * @brief Query live cellular signal quality and serving cell ID metrics.
 * 
 * @param info Pointer to cellular_signal_info structure to populate.
 * @return 0 on success, negative POSIX error code on failure.
 */
int cellular_modem_get_signal_info(struct cellular_signal_info *info);

/**
 * @brief Query network operator metadata (Operator name, Band, TAC, IP address).
 * 
 * @param meta Pointer to cellular_network_metadata structure to populate.
 * @return 0 on success, negative POSIX error code on failure.
 */
int cellular_modem_get_network_metadata(struct cellular_network_metadata *meta);

/**
 * @brief Scan all available neighboring cellular stations (for cellular triangulation).
 * 
 * @param scan Pointer to cellular_neighbor_scan structure to populate.
 * @return 0 on success, negative POSIX error code on failure.
 */
int cellular_modem_scan_neighbor_cells(struct cellular_neighbor_scan *scan);

/**
 * @brief Configure Power Saving Mode (PSM) and eDRX low-power budget timers.
 * 
 * @param enable_psm Enable PSM timer
 * @param psm_sec Requested PSM periodic tau duration in seconds
 * @param enable_edrx Enable eDRX timer
 * @return 0 on success, negative POSIX error code on failure.
 */
int cellular_modem_set_psm_edrx(bool enable_psm, uint32_t psm_sec, bool enable_edrx);

/**
 * @brief Put cellular modem into flight mode / low-power sleep state.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int cellular_modem_sleep(void);

#ifdef __cplusplus
}
#endif

#endif /* CELLULAR_H */
