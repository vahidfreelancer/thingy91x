#include "quaternion.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define DEG_TO_RAD (M_PI / 180.0f)
#define RAD_TO_DEG (180.0f / M_PI)

void quaternion_init(struct quaternion *q, float q0, float q1, float q2, float q3)
{
    q->q0 = q0;
    q->q1 = q1;
    q->q2 = q2;
    q->q3 = q3;
}

void quaternion_normalize(struct quaternion *q)
{
    float norm = sqrtf(q->q0 * q->q0 + q->q1 * q->q1 + q->q2 * q->q2 + q->q3 * q->q3);
    if (norm > 1e-9f) {
        q->q0 /= norm;
        q->q1 /= norm;
        q->q2 /= norm;
        q->q3 /= norm;
    } else {
        /* Fallback to identity quaternion if degenerate */
        q->q0 = 1.0f;
        q->q1 = 0.0f;
        q->q2 = 0.0f;
        q->q3 = 0.0f;
    }
}

void quaternion_multiply(const struct quaternion *a, const struct quaternion *b, struct quaternion *out)
{
    struct quaternion temp;
    temp.q0 = a->q0 * b->q0 - a->q1 * b->q1 - a->q2 * b->q2 - a->q3 * b->q3;
    temp.q1 = a->q0 * b->q1 + a->q1 * b->q0 + a->q2 * b->q3 - a->q3 * b->q2;
    temp.q2 = a->q0 * b->q2 - a->q1 * b->q3 + a->q2 * b->q0 + a->q3 * b->q1;
    temp.q3 = a->q0 * b->q3 + a->q1 * b->q2 - a->q2 * b->q1 + a->q3 * b->q0;
    *out = temp;
}

void quaternion_rotate_vector(const struct quaternion *q, const float vec_in[3], float vec_out[3])
{
    /* Express input vector as a pure quaternion */
    struct quaternion p = {0.0f, vec_in[0], vec_in[1], vec_in[2]};
    
    /* Conjugate of q */
    struct quaternion q_conj = {q->q0, -q->q1, -q->q2, -q->q3};
    
    /* Temp = q * p */
    struct quaternion temp;
    quaternion_multiply(q, &p, &temp);
    
    /* Out_q = Temp * q* */
    struct quaternion out_q;
    quaternion_multiply(&temp, &q_conj, &out_q);
    
    vec_out[0] = out_q.q1;
    vec_out[1] = out_q.q2;
    vec_out[2] = out_q.q3;
}

void quaternion_to_euler(const struct quaternion *q, float *roll, float *pitch, float *yaw)
{
    /* Roll (x-axis rotation) */
    float sinr_cosp = 2.0f * (q->q0 * q->q1 + q->q2 * q->q3);
    float cosr_cosp = 1.0f - 2.0f * (q->q1 * q->q1 + q->q2 * q->q2);
    *roll = atan2f(sinr_cosp, cosr_cosp) * RAD_TO_DEG;

    /* Pitch (y-axis rotation) */
    float sinp = 2.0f * (q->q0 * q->q2 - q->q3 * q->q1);
    if (fabsf(sinp) >= 1.0f) {
        /* Use 90 degrees if out of range */
        *pitch = copysignf(M_PI / 2.0f, sinp) * RAD_TO_DEG;
    } else {
        *pitch = asinf(sinp) * RAD_TO_DEG;
    }

    /* Yaw (z-axis rotation) */
    float siny_cosp = 2.0f * (q->q0 * q->q3 + q->q1 * q->q2);
    float cosy_cosp = 1.0f - 2.0f * (q->q2 * q->q2 + q->q3 * q->q3);
    *yaw = atan2f(siny_cosp, cosy_cosp) * RAD_TO_DEG;
}

void quaternion_from_euler(struct quaternion *q, float roll, float pitch, float yaw)
{
    float r = roll * DEG_TO_RAD * 0.5f;
    float p = pitch * DEG_TO_RAD * 0.5f;
    float y = yaw * DEG_TO_RAD * 0.5f;

    float cr = cosf(r);
    float sr = sinf(r);
    float cp = cosf(p);
    float sp = sinf(p);
    float cy = cosf(y);
    float sy = sinf(y);

    q->q0 = cr * cp * cy + sr * sp * sy;
    q->q1 = sr * cp * cy - cr * sp * sy;
    q->q2 = cr * sp * cy + sr * cp * sy;
    q->q3 = cr * cp * sy - sr * sp * cy;
    
    quaternion_normalize(q);
}
