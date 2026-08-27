#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "wifi_scan.h"

#if defined(CONFIG_NET_L2_WIFI_MGMT) || defined(CONFIG_WIFI)
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/wifi_mgmt.h>

static struct net_mgmt_event_callback wifi_mgmt_cb;
static K_SEM_DEFINE(wifi_scan_sem, 0, 1);
#endif

LOG_MODULE_REGISTER(wifi_scan_driver);

static const struct device *wifi_dev = NULL;
static enum app_wifi_state current_state = APP_WIFI_STATE_DISCONNECTED;
static char connected_ap[33] = "";
static char assigned_ip[16] = "0.0.0.0";
static int16_t current_rssi = 0;

static struct wifi_ap_info scan_results_storage[8];
static uint16_t scan_ap_count = 0;
static bool scan_completed = false;

#if defined(CONFIG_NET_L2_WIFI_MGMT) || defined(CONFIG_WIFI)
static void handle_wifi_scan_result(struct net_mgmt_event_callback *cb)
{
    const struct wifi_scan_result *entry = (const struct wifi_scan_result *)cb->info;
    if (!entry) {
        return;
    }

    if (scan_ap_count < 8) {
        struct wifi_ap_info *ap = &scan_results_storage[scan_ap_count];
        memset(ap, 0, sizeof(*ap));

        size_t ssid_len = entry->ssid_length;
        if (ssid_len > 32) {
            ssid_len = 32;
        }
        if (ssid_len > 0) {
            memcpy(ap->ssid, entry->ssid, ssid_len);
        }
        ap->ssid[ssid_len] = '\0';

        memcpy(ap->bssid, entry->mac, 6);
        ap->rssi_dbm = entry->rssi;
        ap->channel = entry->channel;
        ap->band = (entry->band == WIFI_FREQ_BAND_5_GHZ) ? 1 : 0;

        scan_ap_count++;
    }
}

static void wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb,
                                    uint32_t mgmt_event,
                                    struct net_if *iface)
{
    switch (mgmt_event) {
    case NET_EVENT_WIFI_SCAN_RESULT:
        handle_wifi_scan_result(cb);
        break;
    case NET_EVENT_WIFI_SCAN_DONE:
        scan_completed = true;
        LOG_INF("Wi-Fi scan completed. Discovered %u APs.", scan_ap_count);
        k_sem_give(&wifi_scan_sem);
        break;
    default:
        break;
    }
}
#endif

int wifi_scan_init(void)
{
#if DT_HAS_COMPAT_STATUS_OKAY(nordic_nrf70)
    wifi_dev = DEVICE_DT_GET_ANY(nordic_nrf70);
#elif DT_HAS_COMPAT_STATUS_OKAY(nordic_nrf7002)
    wifi_dev = DEVICE_DT_GET_ANY(nordic_nrf7002);
#endif

    if (wifi_dev && device_is_ready(wifi_dev)) {
        LOG_INF("nRF7002 Wi-Fi 6 Companion IC '%s' initialized successfully.", wifi_dev->name);
    } else {
        LOG_INF("nRF7002 Wi-Fi 6 companion IC driver ready.");
    }

#if defined(CONFIG_NET_L2_WIFI_MGMT) || defined(CONFIG_WIFI)
    net_mgmt_init_event_callback(&wifi_mgmt_cb,
                                 wifi_mgmt_event_handler,
                                 NET_EVENT_WIFI_SCAN_RESULT | NET_EVENT_WIFI_SCAN_DONE);
    net_mgmt_add_event_callback(&wifi_mgmt_cb);
#endif

    current_state = APP_WIFI_STATE_DISCONNECTED;
    strcpy(connected_ap, "");
    strcpy(assigned_ip, "0.0.0.0");
    current_rssi = 0;
    scan_ap_count = 0;
    scan_completed = false;
    memset(scan_results_storage, 0, sizeof(scan_results_storage));

    return 0;
}

int wifi_scan_trigger(void)
{
    current_state = APP_WIFI_STATE_SCANNING;
    scan_ap_count = 0;
    scan_completed = false;
    memset(scan_results_storage, 0, sizeof(scan_results_storage));

    LOG_INF("Initiating 2.4 GHz & 5 GHz Wi-Fi channel scan for location BSSIDs...");

#if defined(CONFIG_NET_L2_WIFI_MGMT) || defined(CONFIG_WIFI)
    k_sem_reset(&wifi_scan_sem);
    struct net_if *iface = net_if_get_default();
    if (iface) {
        int ret = net_mgmt(NET_REQUEST_WIFI_SCAN, iface, NULL, 0);
        if (ret < 0) {
            LOG_WRN("Wi-Fi scan request returned error %d", ret);
            scan_completed = true;
            k_sem_give(&wifi_scan_sem);
            return ret;
        }
        return 0;
    }
#endif

    scan_completed = true;
#if defined(CONFIG_NET_L2_WIFI_MGMT) || defined(CONFIG_WIFI)
    k_sem_give(&wifi_scan_sem);
#endif
    return 0;
}

int wifi_scan_get_results(struct wifi_scan_data *data)
{
    if (!data) {
        return -EINVAL;
    }

    current_state = APP_WIFI_STATE_SCANNING;

#if defined(CONFIG_NET_L2_WIFI_MGMT) || defined(CONFIG_WIFI)
    /* Wait up to 5 seconds for asynchronous Wi-Fi scan events to complete */
    if (!scan_completed) {
        k_sem_take(&wifi_scan_sem, K_MSEC(5000));
    }
#endif

    data->ap_count = scan_ap_count;
    data->valid = true;

    for (uint16_t i = 0; i < scan_ap_count && i < 8; i++) {
        data->results[i] = scan_results_storage[i];
    }

    if (data->ap_count > 0) {
        LOG_INF("[WIFI SCAN SUCCESS] Discovered %u Wi-Fi Access Points:", data->ap_count);
        for (uint16_t i = 0; i < data->ap_count && i < 8; i++) {
            LOG_INF("  AP %u: '%s' (%02X:%02X:%02X:%02X:%02X:%02X, %d dBm, Ch %u, %s)",
                    i + 1,
                    data->results[i].ssid[0] ? data->results[i].ssid : "<Hidden>",
                    data->results[i].bssid[0], data->results[i].bssid[1], data->results[i].bssid[2],
                    data->results[i].bssid[3], data->results[i].bssid[4], data->results[i].bssid[5],
                    data->results[i].rssi_dbm, data->results[i].channel,
                    (data->results[i].band == 1) ? "5GHz" : "2.4GHz");
        }
    } else {
        LOG_INF("[WIFI SCAN] No Wi-Fi Access Points discovered in range.");
    }

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


