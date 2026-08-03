#ifndef MATRIX_H
#define MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

#define MATRIX_MAX_DIM 7

struct matrix {
    int rows;
    int cols;
    float data[MATRIX_MAX_DIM][MATRIX_MAX_DIM];
};

/**
 * @brief Initialize a matrix with zeros.
 */
void matrix_init(struct matrix *m, int rows, int cols);

/**
 * @brief Set matrix to identity.
 */
void matrix_set_identity(struct matrix *m);

/**
 * @brief Set matrix to all zeros.
 */
void matrix_set_zero(struct matrix *m);

/**
 * @brief Add two matrices: out = a + b.
 * @return 0 on success, negative error code on dimension mismatch.
 */
int matrix_add(const struct matrix *a, const struct matrix *b, struct matrix *out);

/**
 * @brief Subtract two matrices: out = a - b.
 * @return 0 on success, negative error code on dimension mismatch.
 */
int matrix_sub(const struct matrix *a, const struct matrix *b, struct matrix *out);

/**
 * @brief Multiply two matrices: out = a * b.
 * @return 0 on success, negative error code on dimension mismatch.
 */
int matrix_mult(const struct matrix *a, const struct matrix *b, struct matrix *out);

/**
 * @brief Transpose a matrix: out = in^T.
 * @return 0 on success.
 */
int matrix_transpose(const struct matrix *in, struct matrix *out);

/**
 * @brief Invert a square matrix using Gaussian elimination with partial pivoting: out = in^-1.
 * @return 0 on success, negative error code if matrix is singular or not square.
 */
int matrix_invert(const struct matrix *in, struct matrix *out);

/**
 * @brief Print matrix content to console (useful for debugging).
 */
void matrix_print(const struct matrix *m, const char *name);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_H */
