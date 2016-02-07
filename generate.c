#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "matrix.h"
#include "generate.h"

/* Pick random integer */
int pickInt(int max) {
    return (int) ((double) rand() / (double) RAND_MAX * max) + 1;
}

/* Pick random value */
double pickValue() {
    return 10.0*(0.5-((double)rand())/((double)RAND_MAX));
}

int main(int argc, char **argv)
{
    unsigned int nbMult;
    size_t i, j, k, val, n, rows, cols;
    FILE * fd;
    struct matrix **mat, **mat2;
    char rep, buf[64];
    pid_t pid;
    
    /* Check args */
    if (argc != 3) {
        fprintf(stderr, "Usage : %s nbMult fichier\n", argv[0]);
        exit(1);
    }
    if ((nbMult = atoi(argv[1])) <= 0) {
        fprintf(stderr, "nbMult is the number of products to generate. It's a positive integer\n");
        exit(1);
    }
    
    /* Matrix array allocation */
    mat = (struct matrix **) malloc(sizeof(struct matrix *)*nbMult);
    mat2 = (struct matrix **) malloc(sizeof(struct matrix *)*nbMult);
    
    /* File opening */
    if ((fd = fopen(argv[2], "w")) == NULL) {
        fprintf(stderr, "Cannot open file \"%s\"\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    
    /* Edit or random generation */
    do {
        printf("Do you want edit matrix ?\n");
        printf("y : I would create my own !\n");
        printf("n : There is a random generation, why piss off ?\n");
        scanf("%c", &rep);
    } while (rep != 'y' && rep != 'n');
    
    
    /* Edit choice */
    if (rep == 'y')
    {
        /* Matrix edit */
        for (i=0; i < nbMult; i++)
        {
            printf("---- Product #%zu ----\n", i);
            printf("Number of rows for M1 : ");
            scanf("%zu", &rows);
            printf("Number of columns for de M1 : ");
            scanf("%zu", &cols);
            
            mat[i] = matrix_init(rows, cols);
            
            for(j=0 ; j < mat[i]->rows ; j++) {
                for(k=0 ; k < mat[i]->cols ; k++) {
                    printf("M1(%zu,%zu)", j+1, k+1);
                    scanf("%zu", &val);
                    matrix_set(mat[i], j, k, val);
                }
            }
            
            printf("Number of rows for M2 : %zu\n", cols);
            rows = cols;
            printf("Number of columns for M2 : ");
            scanf("%zu", &cols);
            
            mat2[i] = matrix_init(rows, cols);
            for(j=0 ; j < mat2[i]->rows ; j++) {
                for(k=0 ; k < mat2[i]->cols ; k++) {
                    printf("M2(%zu,%zu)", j+1, k+1);
                    scanf("%zu", &val);
                    matrix_set(mat2[i], j, k, val);
                }
            }
        }
    }
    /* Random generation */
    else 
    {
        printf("What is the maximum size of matrix ? ");
        scanf("%zu", &val);
        printf("Generation in progress...\n");
        
        srand(time((time_t *)0)); // init rand()
        
        // M1
        for (i=0; i<nbMult; i++)
        {
            rows = pickInt(val);
            cols = pickInt(val);
            
            mat[i] = matrix_init(rows, cols);
            
            for (j=0; j<rows; j++) {
                for (k=0; k<cols; k++) {
                    matrix_set(mat[i], j, k, pickValue());
                }
            }
        }

        // M2
        for (i=0; i<nbMult; i++)
        {
            rows = mat[i]->cols;
            cols = pickInt(val);
            
            mat2[i] = matrix_init(rows, cols);
            
            for (j=0; j<rows; j++) {
                for (k=0; k<cols; k++) {
                    matrix_set(mat2[i], j, k, pickValue());
                }
            }
        }
    }
    
    // Number of products
    fprintf(fd, "%d\n", nbMult);
    
    for (i=0; i<nbMult; i++) {
        // Matrix dimentions
        fprintf(fd, "%zu %zu\n", mat[i]->rows, mat[i]->cols);
        fprintf(fd, "%zu %zu\n", mat2[i]->rows, mat2[i]->cols);
        // Matrix values
        for (j=0; j<mat[i]->rows; j++) {
            for (k=0; k<mat[i]->cols; k++) {
                fprintf(fd, "%f ", matrix_get(mat[i], j, k));
            }
            fprintf(fd, "\n");
        }
        for (j=0; j<mat2[i]->rows; j++) {
            for (k=0; k<mat2[i]->cols; k++) {
                fprintf(fd, "%f ", matrix_get(mat2[i], j, k));
            }
            fprintf(fd, "\n");
        }
    }
    
    printf("Matrix generation done.\n");
    
    /* Free resources */
    for (i=0; i<nbMult; i++) {
        matrix_destroy(mat[i]);
        matrix_destroy(mat2[i]);
    }
    free(mat);
    free(mat2);
    fclose(fd);
    
    
    return 0;
}
