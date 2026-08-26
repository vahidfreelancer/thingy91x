#ifndef WIFI_SCAN_H
#define WIFI_SCAN_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Wi-Fi Connection States.
 */
enum wifi_state {
    WIFI_STATE_DISCONNECTED = 0,    /**< Wi-Fi interface idle / disconnected */
    WIFI_STATE_SCANNING,            /**< Active or passive channel scan in progress */
    WIFI_STATE_CONNECTING,          /**< Connecting / authenticating to Wi-Fi AP */
    WIFI_STATE_CONNECTED            /**< Connected with IP address assigned */
};

/**
 * @brief Scanned Wi-Fi Access Point information.
 */
struct wifi_scan_result {
    char ssid[33];          /**< Wi-Fi network SSID (null-terminated string) */
    uint8_t bssid[6];       /**< 6-byte MAC BSSID address */
    int16_t rssi_dbm;       /**< Received signal strength in dBm (-100 to 0) */
    uint8_t channel;        /**< Wi-Fi channel number (1 - 165) */
    uint8_t band;           /**< Band: 0 = 2.4 GHz, 1 = 5 GHz */
};

/**
 * @brief Wi-Fi scan results container.
 */
struct wifi_scan_data {
    uint16_t ap_count;                      /**< Total access points discovered */
    struct wifi_scan_result results[8];     /**< Access points array */
    bool valid;                             /**< True if scan completed cleanly */
};

/**
 * @brief Active Wi-Fi connection status.
 */
struct wifi_status {
    enum wifi_state state;      /**< Current Wi-Fi state */
    char connected_ssid[33];    /**< Connected SSID */
    char ip_addr[16];           /**< Assigned IPv4 address string */
    int16_t rssi_dbm;           /**< Connected AP signal RSSI (dBm) */
    uint8_t channel;            /**< Active Wi-Fi channel */
};

/**
 * @brief Initialize the nRF7002 Wi-Fi Location Scanner and Station driver.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int wifi_scan_init(void);

/**
 * @brief Trigger a passive 2.4 GHz / 5 GHz Wi-Fi channel scan for location BSSIDs.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int wifi_scan_trigger(void);

/**
 * @brief Retrieve scanned Wi-Fi access point BSSID and RSSI metrics.
 * 
 * @param data Pointer to wifi_scan_data structure to populate.
 * @return 0 on success, negative POSIX error code on failure.
 */
int wifi_scan_get_results(struct wifi_scan_data *data);

/**
 * @brief Connect to a Wi-Fi Access Point in Station (STA) mode.
 * 
 * @param ssid Target Wi-Fi network SSID name.
 * @param password Network security passphrase / key.
 * @return 0 on success, negative POSIX error code on failure.
 */
int wifi_connect(const char *ssid, const char *password);

/**
 * @brief Disconnect from current Wi-Fi network.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int wifi_disconnect(void);

/**
 * @brief Query current Wi-Fi connection status, IP address, and RSSI.
 * 
 * @param status Pointer to wifi_status structure to populate.
 * @return 0 on success, negative POSIX error code on failure.
 */
int wifi_get_status(struct wifi_status *status);

/**
 * @brief Put nRF7002 Wi-Fi companion IC into low-power sleep state.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int wifi_scan_sleep(void);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_SCAN_H */
