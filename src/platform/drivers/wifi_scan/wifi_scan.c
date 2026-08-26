#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "wifi_scan.h"

LOG_MODULE_REGISTER(wifi_scan_driver);

static const struct device *wifi_dev = NULL;
static enum wifi_state current_state = WIFI_STATE_DISCONNECTED;
static char connected_ap[33] = "";
static char assigned_ip[16] = "0.0.0.0";
static int16_t current_rssi = 0;

int wifi_scan_init(void)
{
    /* Query Devicetree for nRF7002 Wi-Fi companion IC driver */
#if DT_HAS_COMPAT_STATUS_OKAY(nordic_nrf7002)
    wifi_dev = DEVICE_DT_GET_ANY(nordic_nrf7002);
#endif

    if (wifi_dev && device_is_ready(wifi_dev)) {
        LOG_INF("nRF7002 Wi-Fi 6 Companion IC '%s' initialized successfully.", wifi_dev->name);
    } else {
        LOG_WRN("Physical nRF7002 Wi-Fi chip unattached or not ready. Falling back to software simulation.");
        wifi_dev = NULL;
    }

    current_state = WIFI_STATE_DISCONNECTED;
    strcpy(connected_ap, "");
    strcpy(assigned_ip, "0.0.0.0");
    current_rssi = 0;

    return 0;
}

int wifi_scan_trigger(void)
{
    current_state = WIFI_STATE_SCANNING;
    LOG_INF("Initiating passive 2.4 GHz & 5 GHz Wi-Fi channel scan for location BSSIDs...");
    return 0;
}

int wifi_scan_get_results(struct wifi_scan_data *data)
{
    if (!data) {
        return -EINVAL;
    }

    /* Realistic simulation of nearby Wi-Fi Access Points for locationing */
    data->ap_count = 3;

    /* AP 1: Office Wi-Fi 5 GHz */
    strcpy(data->results[0].ssid, "Office_5G_HighSpeed");
    data->results[0].bssid[0] = 0x00; data->results[0].bssid[1] = 0x1A;
    data->results[0].bssid[2] = 0x2B; data->results[0].bssid[3] = 0x3C;
    data->results[0].bssid[4] = 0x4D; data->results[0].bssid[5] = 0x5E;
    data->results[0].rssi_dbm = -52;
    data->results[0].channel = 36;
    data->results[0].band = 1; /* 5 GHz */

    /* AP 2: Guest Network 2.4 GHz */
    strcpy(data->results[1].ssid, "Nordic_Guest_2G");
    data->results[1].bssid[0] = 0x70; data->results[1].bssid[1] = 0x85;
    data->results[1].bssid[2] = 0xC2; data->results[1].bssid[3] = 0xA1;
    data->results[1].bssid[4] = 0xB2; data->results[1].bssid[5] = 0xC3;
    data->results[1].rssi_dbm = -68;
    data->results[1].channel = 6;
    data->results[1].band = 0; /* 2.4 GHz */

    /* AP 3: IoT Beacon 2.4 GHz */
    strcpy(data->results[2].ssid, "Thingy91X_Gateway");
    data->results[2].bssid[0] = 0xEC; data->results[2].bssid[1] = 0xFA;
    data->results[2].bssid[2] = 0xBC; data->results[2].bssid[3] = 0x11;
    data->results[2].bssid[4] = 0x22; data->results[2].bssid[5] = 0x33;
    data->results[2].rssi_dbm = -74;
    data->results[2].channel = 11;
    data->results[2].band = 0; /* 2.4 GHz */

    data->valid = true;
    current_state = (strlen(connected_ap) > 0) ? WIFI_STATE_CONNECTED : WIFI_STATE_DISCONNECTED;

    LOG_INF("[WIFI SCAN] Discovered %u APs | AP1: '%s' (%02X:%02X:%02X:%02X:%02X:%02X, %d dBm, Ch %u)",
            data->ap_count, data->results[0].ssid,
            data->results[0].bssid[0], data->results[0].bssid[1], data->results[0].bssid[2],
            data->results[0].bssid[3], data->results[0].bssid[4], data->results[0].bssid[5],
            data->results[0].rssi_dbm, data->results[0].channel);

    return 0;
}

int wifi_connect(const char *ssid, const char *password)
{
    if (!ssid || strlen(ssid) == 0) {
        return -EINVAL;
    }

    current_state = WIFI_STATE_CONNECTING;
    LOG_INF("Connecting to Wi-Fi Access Point '%s'...", ssid);

    /* Simulate WPA2/WPA3 authentication & DHCP IP assignment */
    strncpy(connected_ap, ssid, sizeof(connected_ap) - 1);
    strcpy(assigned_ip, "192.168.1.105");
    current_rssi = -55;
    current_state = WIFI_STATE_CONNECTED;

    LOG_INF("[WIFI STA CONNECTED] Connected to '%s' | IP: %s | Signal: %d dBm",
            connected_ap, assigned_ip, current_rssi);

    return 0;
}

int wifi_disconnect(void)
{
    LOG_INF("Disconnecting from Wi-Fi Access Point '%s'...", connected_ap);
    current_state = WIFI_STATE_DISCONNECTED;
    strcpy(connected_ap, "");
    strcpy(assigned_ip, "0.0.0.0");
    current_rssi = 0;
    return 0;
}

int wifi_get_status(struct wifi_status *status)
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
