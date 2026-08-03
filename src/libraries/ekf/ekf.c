#include "ekf.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define DEG_TO_RAD (M_PI / 180.0f)
#define RAD_TO_DEG (180.0f / M_PI)

void ekf_init(struct ekf_filter *filter, const struct ekf_config *config,
              float initial_roll, float initial_pitch, float initial_yaw)
{
    /* Initialize state */
    quaternion_from_euler(&filter->q, initial_roll, initial_pitch, initial_yaw);
    filter->gyro_bias[0] = 0.0f;
    filter->gyro_bias[1] = 0.0f;
    filter->gyro_bias[2] = 0.0f;

    /* Initialize P (7x7 error covariance) */
    matrix_init(&filter->P, 7, 7);
    /* Small initial variances for quaternion states */
    filter->P.data[0][0] = 1e-4f;
    filter->P.data[1][1] = 1e-4f;
    filter->P.data[2][2] = 1e-4f;
    filter->P.data[3][3] = 1e-4f;
    /* Larger initial uncertainty for gyro biases */
    filter->P.data[4][4] = 1e-3f;
    filter->P.data[5][5] = 1e-3f;
    filter->P.data[6][6] = 1e-3f;

    /* Save measurement noises */
    filter->r_accel = config->r_accel;
    filter->r_yaw = config->r_yaw;

    /* Construct process noise matrix Q (7x7) */
    matrix_init(&filter->Q, 7, 7);
    filter->Q.data[0][0] = config->q_gyro;
    filter->Q.data[1][1] = config->q_gyro;
    filter->Q.data[2][2] = config->q_gyro;
    filter->Q.data[3][3] = config->q_gyro;
    
    filter->Q.data[4][4] = config->q_gyro_bias;
    filter->Q.data[5][5] = config->q_gyro_bias;
    filter->Q.data[6][6] = config->q_gyro_bias;
}

void ekf_predict(struct ekf_filter *filter, const float gyro_raw[3], float dt)
{
    float q0 = filter->q.q0;
    float q1 = filter->q.q1;
    float q2 = filter->q.q2;
    float q3 = filter->q.q3;

    /* 1. Correct angular rates using estimated biases */
    float wx = gyro_raw[0] - filter->gyro_bias[0];
    float wy = gyro_raw[1] - filter->gyro_bias[1];
    float wz = gyro_raw[2] - filter->gyro_bias[2];

    /* 2. Propagate state (quaternion kinematics) */
    filter->q.q0 += 0.5f * (-q1 * wx - q2 * wy - q3 * wz) * dt;
    filter->q.q1 += 0.5f * ( q0 * wx + q2 * wz - q3 * wy) * dt;
    filter->q.q2 += 0.5f * ( q0 * wy - q1 * wz + q3 * wx) * dt;
    filter->q.q3 += 0.5f * ( q0 * wz + q1 * wy - q2 * wx) * dt;
    
    quaternion_normalize(&filter->q);

    /* Update local state variables for Jacobian calculation */
    q0 = filter->q.q0;
    q1 = filter->q.q1;
    q2 = filter->q.q2;
    q3 = filter->q.q3;

    /* 3. Construct 7x7 state transition Jacobian F */
    struct matrix F;
    matrix_init(&F, 7, 7);
    matrix_set_identity(&F);

    /* d(q_next)/d(q) */
    F.data[0][0] = 1.0f;
    F.data[0][1] = -0.5f * wx * dt;
    F.data[0][2] = -0.5f * wy * dt;
    F.data[0][3] = -0.5f * wz * dt;

    F.data[1][0] = 0.5f * wx * dt;
    F.data[1][1] = 1.0f;
    F.data[1][2] = 0.5f * wz * dt;
    F.data[1][3] = -0.5f * wy * dt;

    F.data[2][0] = 0.5f * wy * dt;
    F.data[2][1] = -0.5f * wz * dt;
    F.data[2][2] = 1.0f;
    F.data[2][3] = 0.5f * wx * dt;

    F.data[3][0] = 0.5f * wz * dt;
    F.data[3][1] = 0.5f * wy * dt;
    F.data[3][2] = -0.5f * wx * dt;
    F.data[3][3] = 1.0f;

    /* d(q_next)/d(bias) */
    F.data[0][4] = 0.5f * q1 * dt;
    F.data[0][5] = 0.5f * q2 * dt;
    F.data[0][6] = 0.5f * q3 * dt;

    F.data[1][4] = -0.5f * q0 * dt;
    F.data[1][5] = 0.5f * q3 * dt;
    F.data[1][6] = -0.5f * q2 * dt;

    F.data[2][4] = -0.5f * q3 * dt;
    F.data[2][5] = -0.5f * q0 * dt;
    F.data[2][6] = 0.5f * q1 * dt;

    F.data[3][4] = 0.5f * q2 * dt;
    F.data[3][5] = -0.5f * q1 * dt;
    F.data[3][6] = -0.5f * q0 * dt;

    /* 4. Propagate covariance P = F * P * F^T + Q */
    struct matrix FP;
    matrix_mult(&F, &filter->P, &FP);

    struct matrix FT;
    matrix_transpose(&F, &FT);

    struct matrix FPFT;
    matrix_mult(&FP, &FT, &FPFT);

    /* Scale process noise by time step (Euler integration approximation) */
    struct matrix Qd;
    memcpy(&Qd, &filter->Q, sizeof(struct matrix));
    for (int i = 0; i < 4; i++) {
        Qd.data[i][i] *= dt;
    }
    for (int i = 4; i < 7; i++) {
        Qd.data[i][i] *= dt;
    }

    matrix_add(&FPFT, &Qd, &filter->P);
}

int ekf_update_accel(struct ekf_filter *filter, const float accel_raw[3])
{
    /* 1. Verify accelerometer reading is near 1g to prevent dynamic corruption */
    float norm = sqrtf(accel_raw[0] * accel_raw[0] + accel_raw[1] * accel_raw[1] + accel_raw[2] * accel_raw[2]);
    if (norm < 0.85f || norm > 1.15f) {
        return -1; /* Skip update (dynamic acceleration detected) */
    }

    /* Normalize accelerometer measurements */
    float ax = accel_raw[0] / norm;
    float ay = accel_raw[1] / norm;
    float az = accel_raw[2] / norm;

    float q0 = filter->q.q0;
    float q1 = filter->q.q1;
    float q2 = filter->q.q2;
    float q3 = filter->q.q3;

    /* 2. Compute expected measurement in body frame: h_a = R(q)^T * [0, 0, 1]^T */
    float hx = 2.0f * (q1 * q3 - q0 * q2);
    float hy = 2.0f * (q2 * q3 + q0 * q1);
    float hz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;

    /* Innovation */
    float y_data[3];
    y_data[0] = ax - hx;
    y_data[1] = ay - hy;
    y_data[2] = az - hz;

    /* 3. Compute measurement Jacobian H (3x7) */
    struct matrix H;
    matrix_init(&H, 3, 7);
    
    H.data[0][0] = -2.0f * q2;
    H.data[0][1] =  2.0f * q3;
    H.data[0][2] = -2.0f * q0;
    H.data[0][3] =  2.0f * q1;

    H.data[1][0] =  2.0f * q1;
    H.data[1][1] =  2.0f * q0;
    H.data[1][2] =  2.0f * q3;
    H.data[1][3] =  2.0f * q2;

    H.data[2][0] =  2.0f * q0;
    H.data[2][1] = -2.0f * q1;
    H.data[2][2] = -2.0f * q2;
    H.data[2][3] =  2.0f * q3;

    /* 4. Compute Innovation Covariance: S = H * P * H^T + R */
    struct matrix HP;
    matrix_mult(&H, &filter->P, &HP);

    struct matrix HT;
    matrix_transpose(&H, &HT);

    struct matrix S;
    matrix_mult(&HP, &HT, &S);

    /* Add measurement noise covariance R */
    S.data[0][0] += filter->r_accel;
    S.data[1][1] += filter->r_accel;
    S.data[2][2] += filter->r_accel;

    /* 5. Invert S (3x3 matrix) */
    struct matrix S_inv;
    if (matrix_invert(&S, &S_inv) != 0) {
        return -2; /* Inversion failed (singular covariance) */
    }

    /* 6. Compute Kalman Gain: K = P * H^T * S_inv (7x3) */
    struct matrix PHT;
    matrix_mult(&filter->P, &HT, &PHT);

    struct matrix K;
    matrix_mult(&PHT, &S_inv, &K);

    /* 7. Correct State: dx = K * y */
    float dx[7];
    for (int i = 0; i < 7; i++) {
        dx[i] = K.data[i][0] * y_data[0] + K.data[i][1] * y_data[1] + K.data[i][2] * y_data[2];
    }

    /* Apply correction to quaternion and normalize */
    filter->q.q0 += dx[0];
    filter->q.q1 += dx[1];
    filter->q.q2 += dx[2];
    filter->q.q3 += dx[3];
    quaternion_normalize(&filter->q);

    /* Apply correction to gyro bias */
    filter->gyro_bias[0] += dx[4];
    filter->gyro_bias[1] += dx[5];
    filter->gyro_bias[2] += dx[6];

    /* 8. Correct Covariance: P = (I - K * H) * P */
    struct matrix KH;
    matrix_mult(&K, &H, &KH);

    struct matrix I;
    matrix_init(&I, 7, 7);
    matrix_set_identity(&I);

    struct matrix I_KH;
    matrix_sub(&I, &KH, &I_KH);

    struct matrix P_updated;
    matrix_mult(&I_KH, &filter->P, &P_updated);
    memcpy(&filter->P, &P_updated, sizeof(struct matrix));

    return 0;
}

int ekf_update_yaw(struct ekf_filter *filter, float yaw_measured_deg)
{
    float q0 = filter->q.q0;
    float q1 = filter->q.q1;
    float q2 = filter->q.q2;
    float q3 = filter->q.q3;

    /* 1. Calculate current estimated yaw */
    float siny_cosp = 2.0f * (q0 * q3 + q1 * q2);
    float cosy_cosp = 1.0f - 2.0f * (q2 * q2 + q3 * q3);
    float yaw_est = atan2f(siny_cosp, cosy_cosp);

    /* Innovation */
    float yaw_measured_rad = yaw_measured_deg * DEG_TO_RAD;
    float y = yaw_measured_rad - yaw_est;

    /* Wrap yaw innovation to [-PI, PI] */
    while (y > M_PI)  y -= 2.0f * M_PI;
    while (y < -M_PI) y += 2.0f * M_PI;

    /* 2. Compute Yaw Jacobian H (1x7) */
    float a = siny_cosp;
    float b = cosy_cosp;
    float denom = a * a + b * b;
    if (denom < 1e-9f) {
        return -1; /* Singular denominator */
    }

    struct matrix H;
    matrix_init(&H, 1, 7);

    H.data[0][0] = 2.0f * b * q3 / denom;
    H.data[0][1] = 2.0f * b * q2 / denom;
    H.data[0][2] = 2.0f * (b * q1 + 2.0f * a * q2) / denom;
    H.data[0][3] = 2.0f * (b * q0 + 2.0f * a * q3) / denom;

    /* 3. Compute Innovation Covariance: S = H * P * H^T + R */
    struct matrix HP;
    matrix_mult(&H, &filter->P, &HP);

    struct matrix HT;
    matrix_transpose(&H, &HT);

    struct matrix S;
    matrix_mult(&HP, &HT, &S);

    /* Add measurement noise covariance R */
    S.data[0][0] += filter->r_yaw;

    /* Invert S (scalar 1x1 matrix) */
    float S_val = S.data[0][0];
    if (fabsf(S_val) < 1e-9f) {
        return -2;
    }
    float S_inv_val = 1.0f / S_val;

    /* 4. Compute Kalman Gain: K = P * H^T * S_inv (7x1) */
    struct matrix K;
    matrix_init(&K, 7, 1);
    for (int i = 0; i < 7; i++) {
        float P_HT_i = 0.0f;
        for (int k = 0; k < 7; k++) {
            P_HT_i += filter->P.data[i][k] * HT.data[k][0];
        }
        K.data[i][0] = P_HT_i * S_inv_val;
    }

    /* 5. Correct State: dx = K * y */
    filter->q.q0 += K.data[0][0] * y;
    filter->q.q1 += K.data[1][0] * y;
    filter->q.q2 += K.data[2][0] * y;
    filter->q.q3 += K.data[3][0] * y;
    quaternion_normalize(&filter->q);

    filter->gyro_bias[0] += K.data[4][0] * y;
    filter->gyro_bias[1] += K.data[5][0] * y;
    filter->gyro_bias[2] += K.data[6][0] * y;

    /* 6. Correct Covariance: P = (I - K * H) * P */
    struct matrix KH;
    matrix_mult(&K, &H, &KH);

    struct matrix I;
    matrix_init(&I, 7, 7);
    matrix_set_identity(&I);

    struct matrix I_KH;
    matrix_sub(&I, &KH, &I_KH);

    struct matrix P_updated;
    matrix_mult(&I_KH, &filter->P, &P_updated);
    memcpy(&filter->P, &P_updated, sizeof(struct matrix));

    return 0;
}

void ekf_get_attitude(const struct ekf_filter *filter, float *roll, float *pitch, float *yaw)
{
    quaternion_to_euler(&filter->q, roll, pitch, yaw);
}
