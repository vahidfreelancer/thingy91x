#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <math.h>
#include "gnss.h"

LOG_MODULE_REGISTER(gnss_driver);

int gnss_init(void)
{
    LOG_INF("GNSS receiver interface initialized successfully.");
    return 0;
}

int gnss_read(struct gnss_data *data)
{
    /* Initialize simulation coordinates (e.g. standard start position) */
    static double sim_lat = 45.4642;  // Starting latitude
    static double sim_lon = 9.1900;   // Starting longitude
    static float sim_alt = 120.0f;    // Elevation in meters
    static double time_accum = 0.0;

    time_accum += 1.0;

    /* Simulate movement along a path (e.g. vehicle driving) */
    sim_lat += 0.00008 + 0.000015 * sin(time_accum * 0.05);
    sim_lon += 0.00011 + 0.000010 * cos(time_accum * 0.04);
    sim_alt += 0.05f * sin(time_accum * 0.1);

    data->latitude = sim_lat;
    data->longitude = sim_lon;
    data->altitude = sim_alt;
    
    /* Calculate speed dynamically from movement */
    data->speed = 12.5f + 2.0f * sin(time_accum * 0.08); // ~45 km/h average
    data->satellites = 7 + (uint8_t)(3.0f * sin(time_accum * 0.02) + 3.0f); // 7 to 13 sats
    data->hdop = 0.85f + 0.15f * cos(time_accum * 0.05);
    data->valid = true;

    return 0;
}
