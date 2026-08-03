#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* 
 * Profile-specific configurations for the Environmental Monitor profile.
 * Low rate polling, long sleep periods, low-power mode active.
 */

#define SENSOR_READ_INTERVAL_SEC         300   /* Sample sensors every 5 minutes */
#define UPLINK_INTERVAL_SEC              3600  /* Telemetry uplink every hour */

#endif
