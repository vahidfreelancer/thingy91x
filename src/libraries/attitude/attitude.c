#include <math.h>
#include <stdbool.h>
#include "attitude.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD_TO_DEG(r) ((r) * 180.0 / M_PI)

void attitude_filter_init(struct attitude_filter *filter, float alpha)
{
    filter->roll = 0.0f;
    filter->pitch = 0.0f;
    filter->yaw = 0.0f;
    filter->alpha = alpha;
    filter->initialized = false;
}

void attitude_filter_update(struct attitude_filter *filter, 
                            double ax, double ay, double az,
                            double gx, double gy, double gz,
                            float dt)
{
    /* Calculate Roll and Pitch from accelerometer data (tilt sensing) */
    double roll_acc = RAD_TO_DEG(atan2(ay, az));
    double pitch_acc = RAD_TO_DEG(atan2(-ax, sqrt(ay * ay + az * az)));

    if (!filter->initialized) {
        /* Initialize state directly from accelerometer readings */
        filter->roll = (float)roll_acc;
        filter->pitch = (float)pitch_acc;
        filter->yaw = 0.0f;
        filter->initialized = true;
        return;
    }

    /* Gyro rates assumed to be in degrees/second */
    /* Roll Complementary Filter */
    filter->roll = filter->alpha * (filter->roll + (float)(gx * dt)) + 
                   (1.0f - filter->alpha) * (float)roll_acc;

    /* Pitch Complementary Filter */
    filter->pitch = filter->alpha * (filter->pitch + (float)(gy * dt)) + 
                    (1.0f - filter->alpha) * (float)pitch_acc;

    /* Yaw integration (Yaw cannot be determined by gravity) */
    filter->yaw += (float)(gz * dt);
    
    /* Keep Yaw bounded between -180 and 180 degrees */
    if (filter->yaw > 180.0f) {
        filter->yaw -= 360.0f;
    } else if (filter->yaw < -180.0f) {
        filter->yaw += 360.0f;
    }
}

void attitude_filter_get_rpy(const struct attitude_filter *filter, 
                             float *roll, float *pitch, float *yaw)
{
    if (roll) {
        *roll = filter->roll;
    }
    if (pitch) {
        *pitch = filter->pitch;
    }
    if (yaw) {
        *yaw = filter->yaw;
    }
}
