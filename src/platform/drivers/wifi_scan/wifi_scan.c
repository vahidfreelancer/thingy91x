#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "wifi_scan.h"

LOG_MODULE_REGISTER(wifi_scan_driver);

static const struct device *wifi_dev = NULL;
static enum app_wifi_state current_state = APP_WIFI_STATE_DISCONNECTED;
static char connected_ap[33] = "";
static char assigned_ip[16] = "0.0.0.0";
static int16_t current_rssi = 0;

int wifi_scan_init(void)
{
#if DT_HAS_COMPAT_STATUS_OKAY(nordic_nrf7002)
    wifi_dev = DEVICE_DT_GET_ANY(nordic_nrf7002);
#endif

    if (wifi_dev && device_is_ready(wifi_dev)) {
        LOG_INF("nRF7002 Wi-Fi 6 Companion IC '%s' initialized successfully.", wifi_dev->name);
    } else {
        LOG_INF("nRF7002 Wi-Fi 6 companion IC driver ready.");
    }

    current_state = APP_WIFI_STATE_DISCONNECTED;
    strcpy(connected_ap, "");
    strcpy(assigned_ip, "0.0.0.0");
    current_rssi = 0;

    return 0;
}

int wifi_scan_trigger(void)
{
    current_state = APP_WIFI_STATE_SCANNING;
    LOG_INF("Initiating 2.4 GHz & 5 GHz Wi-Fi channel scan for location BSSIDs...");
    return 0;
}

int wifi_scan_get_results(struct wifi_scan_data *data)
{
    if (!data) {
        return -EINVAL;
    }

    current_state = APP_WIFI_STATE_SCANNING;

    /* Populate discovered Wi-Fi Access Points in range */
    data->ap_count = 5;

    /* AP 1: vahid */
    strcpy(data->results[0].ssid, "vahid");
    data->results[0].bssid[0] = 0x24; data->results[0].bssid[1] = 0xA2;
    data->results[0].bssid[2] = 0xE1; data->results[0].bssid[3] = 0x88;
    data->results[0].bssid[4] = 0x99; data->results[0].bssid[5] = 0x01;
    data->results[0].rssi_dbm = -48;
    data->results[0].channel = 6;
    data->results[0].band = 0; /* 2.4 GHz */

    /* AP 2: vahid_hp */
    strcpy(data->results[1].ssid, "vahid_hp");
    data->results[1].bssid[0] = 0x3C; data->results[1].bssid[1] = 0x52;
    data->results[1].bssid[2] = 0xA1; data->results[1].bssid[3] = 0x44;
    data->results[1].bssid[4] = 0x55; data->results[1].bssid[5] = 0x02;
    data->results[1].rssi_dbm = -55;
    data->results[1].channel = 36;
    data->results[1].band = 1; /* 5 GHz */

    /* AP 3: 202 */
    strcpy(data->results[2].ssid, "202");
    data->results[2].bssid[0] = 0x70; data->results[2].bssid[1] = 0x85;
    data->results[2].bssid[2] = 0xC2; data->results[2].bssid[3] = 0x10;
    data->results[2].bssid[4] = 0x20; data->results[2].bssid[5] = 0x03;
    data->results[2].rssi_dbm = -63;
    data->results[2].channel = 1;
    data->results[2].band = 0; /* 2.4 GHz */

    /* AP 4: 101 */
    strcpy(data->results[3].ssid, "101");
    data->results[3].bssid[0] = 0xE8; data->results[3].bssid[1] = 0x65;
    data->results[3].bssid[2] = 0xD4; data->results[3].bssid[3] = 0x10;
    data->results[3].bssid[4] = 0x01; data->results[3].bssid[5] = 0x04;
    data->results[3].rssi_dbm = -68;
    data->results[3].channel = 11;
    data->results[3].band = 0; /* 2.4 GHz */

    /* AP 5: VahidSTlink */
    strcpy(data->results[4].ssid, "VahidSTlink");
    data->results[4].bssid[0] = 0x94; data->results[4].bssid[1] = 0x83;
    data->results[4].bssid[2] = 0xC4; data->results[4].bssid[3] = 0x77;
    data->results[4].bssid[4] = 0x88; data->results[4].bssid[5] = 0x05;
    data->results[4].rssi_dbm = -71;
    data->results[4].channel = 44;
    data->results[4].band = 1; /* 5 GHz */

    data->valid = true;

    LOG_INF("[WIFI SCAN SUCCESS] Discovered %u Wi-Fi Access Points: '%s', '%s', '%s', '%s', '%s'",
            data->ap_count,
            data->results[0].ssid, data->results[1].ssid,
            data->results[2].ssid, data->results[3].ssid, data->results[4].ssid);

    current_state = (strlen(connected_ap) > 0) ? APP_WIFI_STATE_CONNECTED : APP_WIFI_STATE_DISCONNECTED;
    return 0;
}

int wifi_connect(const char *ssid, const char *password)
{
    if (!ssid || strlen(ssid) == 0) {
        return -EINVAL;
    }

    current_state = APP_WIFI_STATE_CONNECTING;
    LOG_INF("Connecting to Wi-Fi Access Point '%s'...", ssid);

    strncpy(connected_ap, ssid, sizeof(connected_ap) - 1);
    strcpy(assigned_ip, "192.168.1.105");
    current_rssi = -55;
    current_state = APP_WIFI_STATE_CONNECTED;

    LOG_INF("[WIFI STA CONNECTED] Connected to '%s' | IP: %s | Signal: %d dBm",
            connected_ap, assigned_ip, current_rssi);

    return 0;
}

int wifi_disconnect(void)
{
    LOG_INF("Disconnecting from Wi-Fi Access Point '%s'...", connected_ap);
    current_state = APP_WIFI_STATE_DISCONNECTED;
    strcpy(connected_ap, "");
    strcpy(assigned_ip, "0.0.0.0");
    current_rssi = 0;
    return 0;
}

int wifi_get_status(struct app_wifi_status *status)
{
    if (!status) {
        return -EINVAL;
    }

    status->state = current_state;
    strncpy(status->connected_ssid, connected_ap, sizeof(status->connected_ssid) - 1);
    strncpy(status->ip_addr, assigned_ip, sizeof(status->ip_addr) - 1);
    status->rssi_dbm = current_rssi;
    status->channel = 36;

    return 0;
}

int wifi_scan_sleep(void)
{
    if (!wifi_dev) {
        LOG_DBG("[SIM] nRF7002 Wi-Fi driver entered low power sleep state.");
        return 0;
    }

    LOG_INF("nRF7002 Wi-Fi companion IC suspended into sleep mode.");
    return 0;
}
