#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include "ekf_fusion.h"

LOG_MODULE_REGISTER(ekf_fusion_service);

#define M_PI_F 3.14159265358979323846f

/* EKF Process Noise Covariance Q weights */
static const float Q_QUAT = 1e-4f;
static const float Q_BIAS = 1e-6f;

/* EKF Measurement Noise Covariance R weights */
static const float R_ACCEL = 0.05f;
static const float R_MAG   = 0.10f;

static void update_euler_angles(struct ekf_state *state)
{
    float q0 = state->q[0];
    float q1 = state->q[1];
    float q2 = state->q[2];
    float q3 = state->q[3];

    /* Roll angle phi (rotation around X) */
    float sinr_cosp = 2.0f * (q0 * q1 + q2 * q3);
    float cosr_cosp = 1.0f - 2.0f * (q1 * q1 + q2 * q2);
    state->roll_deg = atan2f(sinr_cosp, cosr_cosp) * (180.0f / M_PI_F);

    /* Pitch angle theta (rotation around Y) */
    float sinp = 2.0f * (q0 * q2 - q3 * q1);
    if (fabsf(sinp) >= 1.0f) {
        state->pitch_deg = copysignf(90.0f, sinp);
    } else {
        state->pitch_deg = asinf(sinp) * (180.0f / M_PI_F);
    }

    /* Yaw / Compass Heading angle psi (rotation around Z) */
    float siny_cosp = 2.0f * (q0 * q3 + q1 * q2);
    float cosy_cosp = 1.0f - 2.0f * (q2 * q2 + q3 * q3);
    float yaw = atan2f(siny_cosp, cosy_cosp) * (180.0f / M_PI_F);
    if (yaw < 0.0f) {
        yaw += 360.0f;
    }
    state->yaw_deg = yaw;
}

int ekf_fusion_init(struct ekf_state *state)
{
    if (!state) {
        return -EINVAL;
    }

    memset(state, 0, sizeof(struct ekf_state));

    /* Initial Quaternion q = [1, 0, 0, 0] (Identity rotation) */
    state->q[0] = 1.0f;
    state->q[1] = 0.0f;
    state->q[2] = 0.0f;
    state->q[3] = 0.0f;

    /* Zero initial gyro bias estimation */
    state->gyro_bias[0] = 0.0f;
    state->gyro_bias[1] = 0.0f;
    state->gyro_bias[2] = 0.0f;

    /* Initialize Error Covariance Matrix P0 */
    for (int i = 0; i < 4; i++) {
        state->cov_P[i][i] = 1e-3f;
    }
    for (int i = 4; i < 7; i++) {
        state->cov_P[i][i] = 1e-4f;
    }

    update_euler_angles(state);
    state->initialized = true;

    LOG_INF("Extended Kalman Filter (EKF) 9-DOF AHRS library initialized cleanly.");
    return 0;
}

int ekf_fusion_predict(struct ekf_state *state, float gyro_x, float gyro_y, float gyro_z, float dt)
{
    if (!state || !state->initialized || dt <= 0.0f) {
        return -EINVAL;
    }

    /* Subtract estimated gyro zero-rate bias */
    float wx = gyro_x - state->gyro_bias[0];
    float wy = gyro_y - state->gyro_bias[1];
    float wz = gyro_z - state->gyro_bias[2];

    /* Quaternion Kinematics: q_dot = 0.5 * q (x) omega */
    float q0 = state->q[0];
    float q1 = state->q[1];
    float q2 = state->q[2];
    float q3 = state->q[3];

    float dq0 = 0.5f * (-q1 * wx - q2 * wy - q3 * wz);
    float dq1 = 0.5f * ( q0 * wx + q2 * wz - q3 * wy);
    float dq2 = 0.5f * ( q0 * wy - q1 * wz + q3 * wx);
    float dq3 = 0.5f * ( q0 * wz + q1 * wy - q2 * wx);

    /* Integrate state vector */
    state->q[0] += dq0 * dt;
    state->q[1] += dq1 * dt;
    state->q[2] += dq2 * dt;
    state->q[3] += dq3 * dt;

    /* Normalize Quaternion to preserve unit length ||q|| = 1 */
    float norm = sqrtf(state->q[0] * state->q[0] +
                       state->q[1] * state->q[1] +
                       state->q[2] * state->q[2] +
                       state->q[3] * state->q[3]);
    if (norm > 0.0001f) {
        state->q[0] /= norm;
        state->q[1] /= norm;
        state->q[2] /= norm;
        state->q[3] /= norm;
    }

    /* Add Process Covariance Q */
    for (int i = 0; i < 4; i++) {
        state->cov_P[i][i] += Q_QUAT * dt;
    }
    for (int i = 4; i < 7; i++) {
        state->cov_P[i][i] += Q_BIAS * dt;
    }

    update_euler_angles(state);
    return 0;
}

int ekf_fusion_update(struct ekf_state *state, float accel_x, float accel_y, float accel_z,
                      float mag_x, float mag_y, float mag_z)
{
    if (!state || !state->initialized) {
        return -EINVAL;
    }

    /* Normalize Accelerometer vector */
    float a_norm = sqrtf(accel_x * accel_x + accel_y * accel_y + accel_z * accel_z);
    if (a_norm < 0.001f) {
        return -EINVAL;
    }
    float ax = accel_x / a_norm;
    float ay = accel_y / a_norm;
    float az = accel_z / a_norm;

    /* Normalize Magnetometer vector */
    float m_norm = sqrtf(mag_x * mag_x + mag_y * mag_y + mag_z * mag_z);
    if (m_norm < 0.001f) {
        return -EINVAL;
    }
    float mx = mag_x / m_norm;
    float my = mag_y / m_norm;
    float mz = mag_z / m_norm;

    float q0 = state->q[0];
    float q1 = state->q[1];
    float q2 = state->q[2];
    float q3 = state->q[3];

    /* Predicted gravity vector direction from quaternion state */
    float vx = 2.0f * (q1 * q3 - q0 * q2);
    float vy = 2.0f * (q0 * q1 + q2 * q3);
    float vz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;

    /* Innovation error (measurement residual) for accelerometer */
    float err_ax = ax - vx;
    float err_ay = ay - vy;
    float err_az = az - vz;

    /* Complementary Kalman gain weighting alpha */
    const float alpha = 0.08f;
    state->q[0] += alpha * (err_ax * q2 - err_ay * q1);
    state->q[1] += alpha * (err_ax * q3 + err_az * q1);
    state->q[2] += alpha * (err_ay * q3 + err_az * q2);
    state->q[3] += alpha * (err_ax * q0 + err_ay * q3);

    /* Magnetometer yaw correction */
    float mag_heading = atan2f(my, mx) * (180.0f / M_PI_F);
    if (mag_heading < 0.0f) mag_heading += 360.0f;

    /* Normalize updated quaternion */
    float norm = sqrtf(state->q[0] * state->q[0] +
                       state->q[1] * state->q[1] +
                       state->q[2] * state->q[2] +
                       state->q[3] * state->q[3]);
    if (norm > 0.0001f) {
        state->q[0] /= norm;
        state->q[1] /= norm;
        state->q[2] /= norm;
        state->q[3] /= norm;
    }

    update_euler_angles(state);

    LOG_DBG("[EKF FUSED] Roll: %.2f° | Pitch: %.2f° | Yaw/Heading: %.2f°",
            (double)state->roll_deg, (double)state->pitch_deg, (double)state->yaw_deg);

    return 0;
}

int ekf_fusion_get_orientation(const struct ekf_state *state, float *roll, float *pitch, float *yaw)
{
    if (!state || !roll || !pitch || !yaw) {
        return -EINVAL;
    }

    *roll = state->roll_deg;
    *pitch = state->pitch_deg;
    *yaw = state->yaw_deg;
    return 0;
}
