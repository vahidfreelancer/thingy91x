#include "kalman.h"

void kalman_init(struct kalman_filter *filter, float Q_angle, float Q_bias, float R_measure)
{
    filter->angle = 0.0f;
    filter->bias = 0.0f;

    filter->Q_angle = Q_angle;
    filter->Q_bias = Q_bias;
    filter->R_measure = R_measure;

    /* Initialize error covariance matrix to zero */
    filter->P[0][0] = 0.0f;
    filter->P[0][1] = 0.0f;
    filter->P[1][0] = 0.0f;
    filter->P[1][1] = 0.0f;
}

float kalman_update(struct kalman_filter *filter, float new_angle, float new_rate, float dt)
{
    /* Step 1: Predict state */
    /* Estimate bias-corrected angular velocity (rate) */
    float rate = new_rate - filter->bias;
    filter->angle += dt * rate;

    /* Step 2: Predict error covariance matrix P */
    /* P_predicted = A * P * A^T + Q */
    filter->P[0][0] += dt * (dt * filter->P[1][1] - filter->P[0][1] - filter->P[1][0] + filter->Q_angle);
    filter->P[0][1] -= dt * filter->P[1][1];
    filter->P[1][0] -= dt * filter->P[1][1];
    filter->P[1][1] += filter->Q_bias * dt;

    /* Step 3: Calculate Measurement Innovation (y) and Innovation Covariance (S) */
    /* y = z - H * x */
    float y = new_angle - filter->angle;
    /* S = H * P * H^T + R */
    float S = filter->P[0][0] + filter->R_measure;

    /* Step 4: Calculate Kalman Gain (K) */
    /* K = P * H^T * S^-1 */
    float K[2];
    K[0] = filter->P[0][0] / S;
    K[1] = filter->P[1][0] / S;

    /* Step 5: Correct/Update state */
    /* x = x + K * y */
    filter->angle += K[0] * y;
    filter->bias  += K[1] * y;

    /* Step 6: Correct/Update error covariance matrix P */
    /* P = (I - K * H) * P */
    float P00_temp = filter->P[0][0];
    float P01_temp = filter->P[0][1];

    filter->P[0][0] -= K[0] * P00_temp;
    filter->P[0][1] -= K[0] * P01_temp;
    filter->P[1][0] -= K[1] * P00_temp;
    filter->P[1][1] -= K[1] * P01_temp;

    return filter->angle;
}
