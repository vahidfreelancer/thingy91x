#include "matrix.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

void matrix_init(struct matrix *m, int rows, int cols)
{
    m->rows = (rows > MATRIX_MAX_DIM) ? MATRIX_MAX_DIM : rows;
    m->cols = (cols > MATRIX_MAX_DIM) ? MATRIX_MAX_DIM : cols;
    matrix_set_zero(m);
}

void matrix_set_identity(struct matrix *m)
{
    matrix_set_zero(m);
    int min_dim = (m->rows < m->cols) ? m->rows : m->cols;
    for (int i = 0; i < min_dim; i++) {
        m->data[i][i] = 1.0f;
    }
}

void matrix_set_zero(struct matrix *m)
{
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->cols; j++) {
            m->data[i][j] = 0.0f;
        }
    }
}

int matrix_add(const struct matrix *a, const struct matrix *b, struct matrix *out)
{
    if (a->rows != b->rows || a->cols != b->cols) {
        return -1; /* Dimension mismatch */
    }
    out->rows = a->rows;
    out->cols = a->cols;
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < a->cols; j++) {
            out->data[i][j] = a->data[i][j] + b->data[i][j];
        }
    }
    return 0;
}

int matrix_sub(const struct matrix *a, const struct matrix *b, struct matrix *out)
{
    if (a->rows != b->rows || a->cols != b->cols) {
        return -1; /* Dimension mismatch */
    }
    out->rows = a->rows;
    out->cols = a->cols;
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < a->cols; j++) {
            out->data[i][j] = a->data[i][j] - b->data[i][j];
        }
    }
    return 0;
}

int matrix_mult(const struct matrix *a, const struct matrix *b, struct matrix *out)
{
    if (a->cols != b->rows) {
        return -1; /* Dimension mismatch */
    }
    
    /* Use a temporary matrix to allow in-place multiplication (e.g. matrix_mult(&A, &B, &A)) */
    struct matrix temp;
    temp.rows = a->rows;
    temp.cols = b->cols;

    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < b->cols; j++) {
            float sum = 0.0f;
            for (int k = 0; k < a->cols; k++) {
                sum += a->data[i][k] * b->data[k][j];
            }
            temp.data[i][j] = sum;
        }
    }
    
    memcpy(out, &temp, sizeof(struct matrix));
    return 0;
}

int matrix_transpose(const struct matrix *in, struct matrix *out)
{
    struct matrix temp;
    temp.rows = in->cols;
    temp.cols = in->rows;
    for (int i = 0; i < in->rows; i++) {
        for (int j = 0; j < in->cols; j++) {
            temp.data[j][i] = in->data[i][j];
        }
    }
    memcpy(out, &temp, sizeof(struct matrix));
    return 0;
}

int matrix_invert(const struct matrix *in, struct matrix *out)
{
    if (in->rows != in->cols) {
        return -1; /* Must be square */
    }
    
    int n = in->rows;
    struct matrix temp;
    memcpy(&temp, in, sizeof(struct matrix));
    
    out->rows = n;
    out->cols = n;
    matrix_set_identity(out);
    
    /* Gaussian elimination with partial pivoting */
    for (int i = 0; i < n; i++) {
        /* Find pivot row */
        int pivot = i;
        float max_val = fabsf(temp.data[i][i]);
        for (int r = i + 1; r < n; r++) {
            float val = fabsf(temp.data[r][i]);
            if (val > max_val) {
                max_val = val;
                pivot = r;
            }
        }
        
        /* Check singularity */
        if (max_val < 1e-9f) {
            return -2; /* Singular matrix */
        }
        
        /* Swap rows if necessary */
        if (pivot != i) {
            for (int c = 0; c < n; c++) {
                float t = temp.data[i][c];
                temp.data[i][c] = temp.data[pivot][c];
                temp.data[pivot][c] = t;
                
                t = out->data[i][c];
                out->data[i][c] = out->data[pivot][c];
                out->data[pivot][c] = t;
            }
        }
        
        /* Scale pivot row */
        float pivot_val = temp.data[i][i];
        for (int c = 0; c < n; c++) {
            temp.data[i][c] /= pivot_val;
            out->data[i][c] /= pivot_val;
        }
        
        /* Eliminate other rows */
        for (int r = 0; r < n; r++) {
            if (r == i) {
                continue;
            }
            float factor = temp.data[r][i];
            for (int c = 0; c < n; c++) {
                temp.data[r][c] -= factor * temp.data[i][c];
                out->data[r][c] -= factor * out->data[i][c];
            }
        }
    }
    
    return 0;
}

void matrix_print(const struct matrix *m, const char *name)
{
    printf("Matrix '%s' (%dx%d):\n", name, m->rows, m->cols);
    for (int i = 0; i < m->rows; i++) {
        printf("  [ ");
        for (int j = 0; j < m->cols; j++) {
            printf("%9.4f ", (double)m->data[i][j]);
        }
        printf("]\n");
    }
}
