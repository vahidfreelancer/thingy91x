#ifndef QUATERNION_H
#define QUATERNION_H

#ifdef __cplusplus
extern "C" {
#endif

struct quaternion {
    float q0; /**< Scalar component (w) */
    float q1; /**< Vector component (x) */
    float q2; /**< Vector component (y) */
    float q3; /**< Vector component (z) */
};

/**
 * @brief Initialize a quaternion.
 */
void quaternion_init(struct quaternion *q, float q0, float q1, float q2, float q3);

/**
 * @brief Normalize the quaternion to ensure unit length.
 */
void quaternion_normalize(struct quaternion *q);

/**
 * @brief Multiply two quaternions: out = a * b.
 */
void quaternion_multiply(const struct quaternion *a, const struct quaternion *b, struct quaternion *out);

/**
 * @brief Rotate a 3D vector using a unit quaternion: vec_out = q * vec_in * q*.
 */
void quaternion_rotate_vector(const struct quaternion *q, const float vec_in[3], float vec_out[3]);

/**
 * @brief Convert a unit quaternion to Euler angles (in degrees, Tait-Bryan ZYX convention).
 */
void quaternion_to_euler(const struct quaternion *q, float *roll, float *pitch, float *yaw);

/**
 * @brief Convert Euler angles (in degrees) to a unit quaternion.
 */
void quaternion_from_euler(struct quaternion *q, float roll, float pitch, float yaw);

#ifdef __cplusplus
}
#endif

#endif /* QUATERNION_H */
