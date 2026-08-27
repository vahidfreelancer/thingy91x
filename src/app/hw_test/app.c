#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "app.h"
#include "app_config.h"
#include "environment.h"
#include "high_g.h"
#include "pmic.h"
#include "led.h"
#include "buttons.h"
#include "magnetometer.h"
#include "wifi_scan.h"
#include "cellular.h"
#include "ekf_fusion.h"

LOG_MODULE_REGISTER(app_hw_test);

static struct k_work_delayable hw_test_work;
static int client_fd = -1;
static bool socket_active = false;
static bool connecting_requested = false;
static uint32_t connect_attempts = 0;

/* Sensor state caching structures */
static struct env_sensor_data env_data;
static struct high_g_data motion_data;
static struct pmic_battery_data batt_data;
static struct mag_sensor_data mag_data;
static struct wifi_scan_data wifi_data;
static struct cellular_signal_info cell_info;
static struct cellular_network_metadata cell_meta;
static struct cellular_neighbor_scan cell_neighbors;
static struct ekf_state ekf_filter;

enum hw_test_led_state {
    HW_STATE_IDLE_DISCONNECTED = 0, /**< Green Breathing: Idle system power OK, waiting for button press */
    HW_STATE_CONNECTING_TCP,        /**< Rapid Blue Blinking: Initiating network attach & TCP socket connect */
    HW_STATE_SOCKET_CONNECTED,       /**< Solid Cyan: TCP socket connected to s4.sytemonitor.co.uk:1200 */
    HW_STATE_PROCESSING_COMMAND,    /**< Rapid Magenta Flashes: Receiving JSON command & transmitting response */
    HW_STATE_ERROR_DISCONNECTED     /**< Slow Red Pulse: Error / Socket disconnection alert */
};

static enum hw_test_led_state current_led_state = HW_STATE_IDLE_DISCONNECTED;

static void set_hw_test_led_state(enum hw_test_led_state state)
{
    current_led_state = state;
    switch (state) {
        case HW_STATE_IDLE_DISCONNECTED:
            printk("\r\n[LED STATE] IDLE_DISCONNECTED -> Green Breathing (R:0, G:255, B:0)\r\n");
            LOG_INF("[LED STATE] IDLE_DISCONNECTED -> Green Breathing (R:0, G:255, B:0)");
            led_set_pattern(LED_PATTERN_BREATHE, 0, 255, 0);
            break;
        case HW_STATE_CONNECTING_TCP:
            printk("\r\n[LED STATE] CONNECTING_TCP -> Fast Blue Blinking (R:0, G:128, B:255)\r\n");
            LOG_INF("[LED STATE] CONNECTING_TCP -> Fast Blue Blinking (R:0, G:128, B:255)");
            led_set_pattern(LED_PATTERN_BLINK_FAST, 0, 128, 255);
            break;
        case HW_STATE_SOCKET_CONNECTED:
            printk("\r\n[LED STATE] SOCKET_CONNECTED -> Solid Cyan Glow (R:0, G:255, B:255)\r\n");
            LOG_INF("[LED STATE] SOCKET_CONNECTED -> Solid Cyan Glow (R:0, G:255, B:255)");
            led_set_pattern(LED_PATTERN_SOLID, 0, 255, 255);
            break;
        case HW_STATE_PROCESSING_COMMAND:
            printk("\r\n[LED STATE] PROCESSING_COMMAND -> Fast Magenta Flashes (R:255, G:0, B:255)\r\n");
            LOG_INF("[LED STATE] PROCESSING_COMMAND -> Fast Magenta Flashes (R:255, G:0, B:255)");
            led_set_pattern(LED_PATTERN_BLINK_FAST, 255, 0, 255);
            break;
        case HW_STATE_ERROR_DISCONNECTED:
            printk("\r\n[LED STATE] ERROR_DISCONNECTED -> Slow Red Pulse (R:255, G:0, B:0)\r\n");
            LOG_INF("[LED STATE] ERROR_DISCONNECTED -> Slow Red Pulse (R:255, G:0, B:0)");
            led_set_pattern(LED_PATTERN_BLINK_SLOW, 255, 0, 0);
            break;
    }
}

/**
 * @brief Handle incoming TCP JSON commands from s4.sytemonitor.co.uk:1200
 * and serialize JSON responses containing raw values, calculated values, and measurement units.
 */
static void process_json_command(const char *cmd_json, char *resp_buf, size_t max_len)
{
    if (!cmd_json || !resp_buf) return;

    /* Flash Magenta LED to indicate active JSON command processing */
    set_hw_test_led_state(HW_STATE_PROCESSING_COMMAND);

    printk("\r\n[TCP RECV s4.sytemonitor.co.uk:1200] Raw Command: %s\r\n", cmd_json);

    if (strstr(cmd_json, "PING")) {
        snprintf(resp_buf, max_len,
            "{\"status\":\"SUCCESS\",\"cmd\":\"PING\",\"board\":\"Thingy:91 X PCA20065\",\"uptime_sec\":%u,\"units\":{\"uptime\":\"sec\"}}",
            (unsigned int)(k_uptime_get() / 1000));
    }
    else if (strstr(cmd_json, "GET_ENV_DATA")) {
        env_sensor_read(&env_data);
        int32_t raw_temp_adc = (int32_t)(env_data.temperature * 100.0f);
        uint32_t raw_press_pa = (uint32_t)(env_data.pressure * 100.0f);
        uint32_t raw_gas_ohm = (uint32_t)env_data.gas_resistance;

        snprintf(resp_buf, max_len,
            "{\"status\":\"SUCCESS\",\"cmd\":\"GET_ENV_DATA\",\"data\":{"
            "\"temperature\":{\"raw_adc\":%d,\"calculated\":%.2f,\"unit\":\"degC\"},"
            "\"humidity\":{\"raw_adc\":%u,\"calculated\":%.2f,\"unit\":\"%%RH\"},"
            "\"pressure\":{\"raw_pa\":%u,\"calculated\":%.2f,\"unit\":\"hPa\"},"
            "\"gas_resistance\":{\"raw_ohm\":%u,\"calculated\":%.0f,\"unit\":\"Ohm\"},"
            "\"iaq_index\":{\"calculated\":%u,\"unit\":\"IAQ_0_500\"}"
            "}}",
            raw_temp_adc, (double)env_data.temperature,
            (unsigned int)(env_data.humidity * 10.0f), (double)env_data.humidity,
            raw_press_pa, (double)env_data.pressure,
            raw_gas_ohm, (double)env_data.gas_resistance,
            env_data.iaq_index);
    }
    else if (strstr(cmd_json, "GET_MOTION_DATA")) {
        high_g_read(&motion_data);
        int16_t raw_ax = (int16_t)(motion_data.accel_x * 1000.0f);
        int16_t raw_ay = (int16_t)(motion_data.accel_y * 1000.0f);
        int16_t raw_az = (int16_t)(motion_data.accel_z * 1000.0f);

        snprintf(resp_buf, max_len,
            "{\"status\":\"SUCCESS\",\"cmd\":\"GET_MOTION_DATA\",\"data\":{"
            "\"accel_x\":{\"raw_lsb\":%d,\"calculated_g\":%.2f,\"calculated_ms2\":%.2f,\"unit\":\"g\"},"
            "\"accel_y\":{\"raw_lsb\":%d,\"calculated_g\":%.2f,\"calculated_ms2\":%.2f,\"unit\":\"g\"},"
            "\"accel_z\":{\"raw_lsb\":%d,\"calculated_g\":%.2f,\"calculated_ms2\":%.2f,\"unit\":\"g\"},"
            "\"magnitude\":{\"calculated_g\":%.2f,\"unit\":\"g\"},"
            "\"peak_hold\":{\"calculated_g\":%.2f,\"unit\":\"g\"},"
            "\"impact_detected\":%s,\"freefall_detected\":%s"
            "}}",
            raw_ax, (double)motion_data.accel_x, (double)(motion_data.accel_x * 9.80665f),
            raw_ay, (double)motion_data.accel_y, (double)(motion_data.accel_y * 9.80665f),
            raw_az, (double)motion_data.accel_z, (double)(motion_data.accel_z * 9.80665f),
            (double)motion_data.magnitude, (double)motion_data.peak_g,
            motion_data.impact_detected ? "true" : "false",
            motion_data.freefall_detected ? "true" : "false");
    }
    else if (strstr(cmd_json, "GET_MAG_DATA")) {
        mag_sensor_read(&mag_data);
        int16_t raw_mx = (int16_t)(mag_data.mag_x_ut * 10.0f);
        int16_t raw_my = (int16_t)(mag_data.mag_y_ut * 10.0f);
        int16_t raw_mz = (int16_t)(mag_data.mag_z_ut * 10.0f);

        snprintf(resp_buf, max_len,
            "{\"status\":\"SUCCESS\",\"cmd\":\"GET_MAG_DATA\",\"data\":{"
            "\"mag_x\":{\"raw_lsb\":%d,\"calculated_ut\":%.2f,\"unit\":\"uT\"},"
            "\"mag_y\":{\"raw_lsb\":%d,\"calculated_ut\":%.2f,\"unit\":\"uT\"},"
            "\"mag_z\":{\"raw_lsb\":%d,\"calculated_ut\":%.2f,\"unit\":\"uT\"},"
            "\"magnitude\":{\"calculated_ut\":%.2f,\"unit\":\"uT\"},"
            "\"compass_heading\":{\"calculated_deg\":%.2f,\"unit\":\"deg\"},"
            "\"tamper_detected\":%s"
            "}}",
            raw_mx, (double)mag_data.mag_x_ut,
            raw_my, (double)mag_data.mag_y_ut,
            raw_mz, (double)mag_data.mag_z_ut,
            (double)mag_data.magnitude_ut, (double)mag_data.heading_deg,
            mag_data.tamper_detected ? "true" : "false");
    }
    else if (strstr(cmd_json, "GET_EKF_FUSION")) {
        float r = 0, p = 0, y = 0;
        ekf_fusion_get_orientation(&ekf_filter, &r, &p, &y);

        snprintf(resp_buf, max_len,
            "{\"status\":\"SUCCESS\",\"cmd\":\"GET_EKF_FUSION\",\"data\":{"
            "\"quaternion\":{\"q0\":%.4f,\"q1\":%.4f,\"q2\":%.4f,\"q3\":%.4f},"
            "\"fused_roll\":{\"calculated\":%.2f,\"unit\":\"deg\"},"
            "\"fused_pitch\":{\"calculated\":%.2f,\"unit\":\"deg\"},"
            "\"fused_yaw_heading\":{\"calculated\":%.2f,\"unit\":\"deg\"},"
            "\"gyro_bias\":{\"bx\":%.4f,\"by\":%.4f,\"bz\":%.4f,\"unit\":\"rad/s\"}"
            "}}",
            (double)ekf_filter.q[0], (double)ekf_filter.q[1],
            (double)ekf_filter.q[2], (double)ekf_filter.q[3],
            (double)r, (double)p, (double)y,
            (double)ekf_filter.gyro_bias[0], (double)ekf_filter.gyro_bias[1], (double)ekf_filter.gyro_bias[2]);
    }
    else if (strstr(cmd_json, "GET_BATTERY_DATA")) {
        pmic_read(&batt_data);

        snprintf(resp_buf, max_len,
            "{\"status\":\"SUCCESS\",\"cmd\":\"GET_BATTERY_DATA\",\"data\":{"
            "\"voltage\":{\"raw_mv\":%u,\"calculated_v\":%.3f,\"unit\":\"mV\"},"
            "\"current\":{\"raw_ma\":%d,\"calculated_ma\":%d,\"unit\":\"mA\"},"
            "\"power\":{\"calculated_mw\":%.2f,\"unit\":\"mW\"},"
            "\"state_of_charge\":{\"calculated_pct\":%.1f,\"unit\":\"%%\"},"
            "\"remaining_capacity\":{\"calculated_mah\":%u,\"unit\":\"mAh\"},"
            "\"low_battery_alert\":%s"
            "}}",
            batt_data.voltage_mv, (double)(batt_data.voltage_mv / 1000.0f),
            batt_data.current_ma, batt_data.current_ma,
            (double)batt_data.power_mw, (double)batt_data.soc_percent,
            batt_data.remaining_capacity_mah,
            batt_data.low_battery_alert ? "true" : "false");
    }
    else if (strstr(cmd_json, "GET_WIFI_SCAN")) {
        wifi_scan_get_results(&wifi_data);

        snprintf(resp_buf, max_len,
            "{\"status\":\"SUCCESS\",\"cmd\":\"GET_WIFI_SCAN\",\"data\":{"
            "\"ap_count\":%u,"
            "\"ap_1\":{\"ssid\":\"%s\",\"bssid\":\"%02X:%02X:%02X:%02X:%02X:%02X\",\"rssi\":%d,\"unit\":\"dBm\",\"channel\":%u,\"band\":\"5GHz\"},"
            "\"ap_2\":{\"ssid\":\"%s\",\"bssid\":\"%02X:%02X:%02X:%02X:%02X:%02X\",\"rssi\":%d,\"unit\":\"dBm\",\"channel\":%u,\"band\":\"2.4GHz\"}"
            "}}",
            wifi_data.ap_count,
            wifi_data.results[0].ssid, wifi_data.results[0].bssid[0], wifi_data.results[0].bssid[1], wifi_data.results[0].bssid[2], wifi_data.results[0].bssid[3], wifi_data.results[0].bssid[4], wifi_data.results[0].bssid[5], wifi_data.results[0].rssi_dbm, wifi_data.results[0].channel,
            wifi_data.results[1].ssid, wifi_data.results[1].bssid[0], wifi_data.results[1].bssid[1], wifi_data.results[1].bssid[2], wifi_data.results[1].bssid[3], wifi_data.results[1].bssid[4], wifi_data.results[1].bssid[5], wifi_data.results[1].rssi_dbm, wifi_data.results[1].channel);
    }
    else if (strstr(cmd_json, "GET_CELLULAR_INFO")) {
        cellular_modem_get_signal_info(&cell_info);
        cellular_modem_get_network_metadata(&cell_meta);
        cellular_modem_scan_neighbor_cells(&cell_neighbors);

        snprintf(resp_buf, max_len,
            "{\"status\":\"SUCCESS\",\"cmd\":\"GET_CELLULAR_INFO\",\"data\":{"
            "\"signal\":{\"rsrp\":%d,\"unit_rsrp\":\"dBm\",\"rsrq\":%d,\"unit_rsrq\":\"dB\",\"snr\":%d,\"unit_snr\":\"dB\",\"cell_id\":\"0x%08X\"},"
            "\"network\":{\"operator\":\"%s\",\"rat\":\"%s\",\"band\":%u,\"tac\":%u,\"ip\":\"%s\"},"
            "\"neighbors\":{\"station_count\":%u,\"station_1_cell_id\":\"0x%08X\",\"station_1_rsrp\":%d,\"unit_rsrp\":\"dBm\"}"
            "}}",
            cell_info.rsrp_dbm, cell_info.rsrq_db, cell_info.snr_db, cell_info.cell_id,
            cell_meta.operator_name, cell_meta.rat_name, cell_meta.band_number, cell_meta.tac, cell_meta.ip_address,
            cell_neighbors.station_count, cell_neighbors.stations[0].cell_id, cell_neighbors.stations[0].rsrp_dbm);
    }
    else {
        /* Default ALL SENSORS Summary JSON */
        snprintf(resp_buf, max_len,
            "{\"status\":\"SUCCESS\",\"cmd\":\"GET_ALL_SENSORS\",\"data\":{"
            "\"env\":{\"temp_degC\":%.2f,\"humidity_pct\":%.2f,\"press_hpa\":%.2f},"
            "\"motion\":{\"mag_g\":%.2f,\"peak_g\":%.2f},"
            "\"mag\":{\"mag_ut\":%.2f,\"heading_deg\":%.2f},"
            "\"battery\":{\"v_mv\":%u,\"soc_pct\":%.1f},"
            "\"cellular\":{\"rsrp_dbm\":%d,\"cell_id\":\"0x%08X\"}"
            "}}",
            (double)env_data.temperature, (double)env_data.humidity, (double)env_data.pressure,
            (double)motion_data.magnitude, (double)motion_data.peak_g,
            (double)mag_data.magnitude_ut, (double)mag_data.heading_deg,
            batt_data.voltage_mv, (double)batt_data.soc_percent,
            cell_info.rsrp_dbm, cell_info.cell_id);
    }

    printk("[TCP SEND s4.sytemonitor.co.uk:1200] Response Serialized (%u bytes): %s\r\n",
           (unsigned int)strlen(resp_buf), resp_buf);

    /* Send payload over real POSIX BSD socket if active */
    if (client_fd >= 0) {
        int sent = zsock_send(client_fd, resp_buf, strlen(resp_buf), 0);
        if (sent < 0) {
            printk("[TCP SEND ERROR] zsock_send failed (err: %d)\r\n", errno);
        } else {
            printk("[TCP TX SUCCESS] Transmitted %d bytes over cellular socket to s4.sytemonitor.co.uk:1200\r\n", sent);
        }
    }

    /* Return to Solid Cyan LED to reflect established TCP socket connection */
    set_hw_test_led_state(HW_STATE_SOCKET_CONNECTED);
}

static void on_hw_test_button(enum button_id id, enum button_event event)
{
    const char *btn_name = (id == BUTTON_ID_1) ? "BUTTON1" : "BUTTON2";
    printk("\r\n=================================================\r\n");
    printk("[USER ACTION] %s Pressed! Initiating real TCP connection to %s:%d...\r\n", btn_name, SERVER_HOST, SERVER_PORT);
    printk("=================================================\r\n");

    /* Set LED to Fast Blue Blinking during TCP socket handshake */
    set_hw_test_led_state(HW_STATE_CONNECTING_TCP);

    connecting_requested = true;
    connect_attempts = 0;
}

static int open_real_tcp_socket(void)
{
    struct zsock_addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct zsock_addrinfo *res = NULL;
    char port_str[8];
    snprintf(port_str, sizeof(port_str), "%d", SERVER_PORT);

    printk("[DNS RESOLUTION] Resolving host '%s' on port %s...\r\n", SERVER_HOST, port_str);
    int err = zsock_getaddrinfo(SERVER_HOST, port_str, &hints, &res);
    if (err != 0 || !res) {
        printk("[DNS ERROR] zsock_getaddrinfo failed for '%s' (err: %d)\r\n", SERVER_HOST, err);
        return -EHOSTUNREACH;
    }

    int fd = zsock_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd < 0) {
        printk("[SOCKET ERROR] zsock_socket creation failed (err: %d)\r\n", errno);
        zsock_freeaddrinfo(res);
        return -errno;
    }

    /* Set 5-second socket read/write timeouts */
    struct timeval timeout = {
        .tv_sec = 5,
        .tv_usec = 0
    };
    zsock_setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    zsock_setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    printk("[TCP CONNECT] Executing zsock_connect() to %s:%d...\r\n", SERVER_HOST, SERVER_PORT);
    err = zsock_connect(fd, res->ai_addr, res->ai_addrlen);
    zsock_freeaddrinfo(res);

    if (err < 0) {
        printk("[TCP ERROR] zsock_connect to %s:%d failed (err: %d)\r\n", SERVER_HOST, SERVER_PORT, errno);
        zsock_close(fd);
        return -errno;
    }

    printk("[TCP CONNECT SUCCESS] Real POSIX BSD socket connected to %s:%d (fd=%d)\r\n",
           SERVER_HOST, SERVER_PORT, fd);
    return fd;
}

static void hw_test_work_handler(struct k_work *work)
{
    static uint32_t heartbeat_cnt = 0;
    heartbeat_cnt++;

    /* Service LED animation step */
    led_update();
    buttons_update();

    const char *state_names[] = {
        "IDLE_DISCONNECTED (Green Breathing)",
        "CONNECTING_TCP (Blue Blinking)",
        "SOCKET_CONNECTED (Solid Cyan Glow)",
        "PROCESSING_COMMAND (Magenta Flashes)",
        "ERROR_DISCONNECTED (Red Pulse)"
    };

    /* Query network status for serial heartbeat log */
    cellular_modem_get_signal_info(&cell_info);
    cellular_modem_get_network_metadata(&cell_meta);

    const char *op_name = (cell_meta.operator_name[0] != '\0') ? cell_meta.operator_name : "Searching Cellular...";
    const char *ip_addr = (cell_meta.ip_address[0] != '\0') ? cell_meta.ip_address : "0.0.0.0";

    /* Periodic 1-second Serial UART Heartbeat directly on COM port */
    printk("[HEARTBEAT #%u] Uptime: %us | LED State: %s | LTE Signal: %d dBm (%s) | IP: %s\r\n",
           heartbeat_cnt, (unsigned int)(k_uptime_get() / 1000),
           state_names[current_led_state], cell_info.rsrp_dbm, op_name, ip_addr);

    if (connecting_requested) {
        connect_attempts++;
        printk("[TCP CONNECTING attempt #%u] Opening real cellular socket to %s:%d...\r\n",
               connect_attempts, SERVER_HOST, SERVER_PORT);

        int fd = open_real_tcp_socket();
        if (fd >= 0) {
            client_fd = fd;
            socket_active = true;
            connecting_requested = false;
            set_hw_test_led_state(HW_STATE_SOCKET_CONNECTED);
            printk("[TCP SUCCESS] Socket active on %s:%d -> Transitioning to Solid Cyan Glow\r\n", SERVER_HOST, SERVER_PORT);
        } else {
            client_fd = -1;
            socket_active = false;
            connecting_requested = false;
            set_hw_test_led_state(HW_STATE_ERROR_DISCONNECTED);
            printk("[TCP ERROR] Connection to %s:%d failed (%d) -> Transitioning to Slow Red Pulse\r\n",
                   SERVER_HOST, SERVER_PORT, fd);
        }
    }

    if (socket_active) {
        printk("--- TCP SOCKET ACTIVE [%s:%d] ---\r\n", SERVER_HOST, SERVER_PORT);
        printk("Listening for incoming JSON diagnostic test commands...\r\n");

        static char rx_buf[512];
        static char json_resp[1024];

        /* Check for incoming data over real BSD socket */
        if (client_fd >= 0) {
            int recved = zsock_recv(client_fd, rx_buf, sizeof(rx_buf) - 1, ZSOCK_MSG_DONTWAIT);
            if (recved > 0) {
                rx_buf[recved] = '\0';
                printk("[REAL TCP RECV] Received %d bytes from %s:%d: %s\r\n",
                       recved, SERVER_HOST, SERVER_PORT, rx_buf);
                process_json_command(rx_buf, json_resp, sizeof(json_resp));
            } else if (recved < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                printk("[REAL TCP RX DISCONNECT] Connection closed by remote host (err: %d)\r\n", errno);
                zsock_close(client_fd);
                client_fd = -1;
                socket_active = false;
                set_hw_test_led_state(HW_STATE_ERROR_DISCONNECTED);
            }
        }

        /* Also allow local serial terminal diagnostic command execution */
        static uint8_t cmd_step = 0;
        cmd_step++;

        const char *test_cmds[] = {
            "{\"cmd\":\"PING\"}",
            "{\"cmd\":\"GET_ENV_DATA\"}",
            "{\"cmd\":\"GET_MOTION_DATA\"}",
            "{\"cmd\":\"GET_MAG_DATA\"}",
            "{\"cmd\":\"GET_EKF_FUSION\"}",
            "{\"cmd\":\"GET_BATTERY_DATA\"}",
            "{\"cmd\":\"GET_WIFI_SCAN\"}",
            "{\"cmd\":\"GET_CELLULAR_INFO\"}"
        };

        const char *active_cmd = test_cmds[cmd_step % 8];
        process_json_command(active_cmd, json_resp, sizeof(json_resp));
    }

    k_work_reschedule(&hw_test_work, K_SECONDS(TELEMETRY_SAMPLE_INTERVAL_SEC));
}

int app_init(void)
{
    printk("\r\n=============================================================\r\n");
    printk("Initializing Hardware Diagnostic & Test Suite Profile (APP_PROFILE_HW_TEST)\r\n");
    printk("Target Server: %s | Port: %d\r\n", SERVER_HOST, SERVER_PORT);
    printk("=============================================================\r\n");

    /* Wake up and initialize all 9 onboard hardware drivers */
    env_sensor_init();
    high_g_init();
    pmic_init();
    led_driver_init();
    buttons_driver_init();
    buttons_register_callback(on_hw_test_button);
    mag_sensor_init();
    wifi_scan_init();
    cellular_modem_init();
    cellular_modem_connect(CELLULAR_MODE_LTE_M);
    ekf_fusion_init(&ekf_filter);

    /* Initial Green Breathing pattern for IDLE_DISCONNECTED state */
    set_hw_test_led_state(HW_STATE_IDLE_DISCONNECTED);

    k_work_init_delayable(&hw_test_work, hw_test_work_handler);
    return 0;
}

void app_run(void)
{
    printk("Running Hardware Diagnostic & Remote Test Suite Application...\r\n");
    printk(">>> PRESS BUTTON1 or BUTTON2 to open real TCP Socket to %s:%d <<<\r\n", SERVER_HOST, SERVER_PORT);

    k_work_reschedule(&hw_test_work, K_NO_WAIT);

    while (1) {
        k_sleep(K_FOREVER);
    }
}
