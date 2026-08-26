#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* 
 * Profile-specific configurations for the Environmental Monitor profile.
 * Low rate polling, long sleep periods, low-power mode active.
 */

#define SENSOR_READ_INTERVAL_SEC         5     /* Sample sensors every 5 seconds for live hardware testing */
#define UPLINK_INTERVAL_SEC              60    /* Telemetry uplink every 60 seconds */

#endif
