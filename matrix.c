#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

struct matrix * matrix_init(size_t m, size_t n)
{
    struct matrix * pmat = (struct matrix *) malloc(sizeof(struct matrix));
    int i;
    
    pmat->rows = m;
    pmat->cols = n;
    
    pmat->data = (double **) malloc(sizeof(double *)*m);
    for (i=0; i<m; i++) {
        pmat->data[i] = (double *) malloc(sizeof(double)*n);
    }
    
    return pmat;
}

void matrix_set(struct matrix * pmat, size_t row, size_t col, double value)
{
    if (row >= pmat->rows || col >= pmat->cols) {
        fprintf(stderr, "matrix_set : Out of bound\n");
        exit(EXIT_FAILURE);
    }
    pmat->data[row][col] = value;
}

double matrix_get(struct matrix * pmat, size_t row, size_t col)
{
    if (row >= pmat->rows || col >= pmat->cols) {
        fprintf(stderr, "Try to access to (%zu,%zu) on a %zux%zu matrix\n", row, col, pmat->rows, pmat->cols);
        exit(EXIT_FAILURE);
    }
    return pmat->data[row][col];
}

void matrix_print(struct matrix * pmat)
{
    size_t i, j;
    for (i=0; i < pmat->rows; i++) {
        for (j=0; j < pmat->cols; j++) {
            printf("%f\t", pmat->data[i][j]);
        }
        printf("\n");
    }
}

void matrix_fprint(struct matrix * pmat, FILE * out)
{
    size_t i, j;
    for (i=0; i < pmat->rows; i++) {
        for (j=0; j < pmat->cols; j++) {
            fprintf(out, "%f\t", pmat->data[i][j]);
        }
        fprintf(out, "\n");
    }
}

void matrix_destroy(struct matrix * pmat)
{
    int i;
    
    for (i=0; i<pmat->rows; i++) {
        free(pmat->data[i]);
    }
    free(pmat);
}