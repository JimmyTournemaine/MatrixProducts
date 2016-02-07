#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "product.h"
#include "matrix.h"

Calculation calc;

/*
 * Coefficients calculation
 */
void * calcul(void * data)
{
    /* Get index from parameter */
    size_t index = * (size_t *) data;
    size_t i, j, row, col;
    double result;
    
    for (i=0; i<calc.nbMult; i++)
    {
        /* Wait authorization */
        pthread_mutex_lock(&(calc.mutex));
        while( calc.state != STATE_CALC || !(calc.pendingCoefs[index] == 1)) {
            pthread_cond_wait(&(calc.cond), &(calc.mutex));
        }
        pthread_mutex_unlock(&calc.mutex);
        
        /* We need this thread yet ? */
        if (index < calc.result->rows * calc.result->cols) {
            
            /* Index to two-dimensional indexes */
            size_t row = index / calc.result->cols;
            size_t col = index % calc.result->cols;
            
            /* Calculation */
            result = 0;
            for (j=0; j < calc.m1->cols ; j++) {
                result += matrix_get(calc.m1, row, j) * matrix_get(calc.m2, j, col);
            }
            matrix_set(calc.result, row, col, result);
        }
        
        /* Prevent that calculation is ended */
        pthread_mutex_lock(&(calc.mutex));
        calc.pendingCoefs[index] = 0;
        
        /* If it is the last one */
        if(nbPendingCoefs(&calc) == 0)
        {
            /* Printing can start */
            calc.state = STATE_PRINT;
            pthread_cond_broadcast(&(calc.cond));
        }
        pthread_mutex_unlock(&(calc.mutex));
    }
    
    return(data);
}

int main(int argc,char ** argv)
{
    /* To get running time */
    time_t start = time(NULL);
    
    /* Vars */
    size_t i=0, j, k;
    size_t * coefData;
    int rows, cols, fd;
    pthread_t *threads;
    FILE * fileOut;
    
    char * fileaddr, *addr;
    struct matrix **mat1, **mat2;
    struct stat sb;
    
    /* Check args */
    if(argc < 2) {
        fprintf(stderr, "Usage: %s matrix_file [result_file]\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    
    /* Open file(s) */
    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        fprintf(stderr, "Cannot read file %s", argv[1]);
        exit(EXIT_FAILURE);
    }
    if (argv[2]) {
        if ((fileOut = fopen(argv[2], "w")) == NULL) {
            printf("Cannot open or create file \"%s\"\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    } else {
        fileOut = stdout;
    }
    
    
    /* Semaphore initialization */
    pthread_mutex_init( &(calc.mutex), NULL);
    pthread_cond_init( &(calc.cond), NULL);
    
    /* File-in size */
    if (fstat(fd, &sb) == -1) {
        perror("fstat"); exit(1);
    }
    
    /* Store file data */
    fileaddr = (char *) mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (fileaddr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    /* Get the number of products */
    addr = fileaddr;
    calc.nbMult = (int) scanDouble(&addr);
    
    /* Two matrix' arrays allocation */
    mat1 = (struct matrix **) malloc(calc.nbMult * sizeof(struct matrix *));
    mat2 = (struct matrix **) malloc(calc.nbMult * sizeof(struct matrix *));
    
    /* Read and set data */
    for (i=0; i<calc.nbMult; i++)
    {
        /* First matrix sizes */
        rows = (int) scanDouble(&addr);
        cols = (int) scanDouble(&addr);
        mat1[i] = matrix_init(rows, cols);
        
        /* Second matrix sizes */
        rows = (int) scanDouble(&addr);
        cols = (int) scanDouble(&addr);
        mat2[i] = matrix_init(rows, cols);
        
        /* First matrix data */
        for (j=0; j<mat1[i]->rows; j++) {
            for (k=0; k<mat1[i]->cols; k++) {
                matrix_set(mat1[i], j, k, scanDouble(&addr));
            }
        }
        
        /* Second matrix data */
        for (j=0; j<mat2[i]->rows; j++) {
            for (k=0; k<mat2[i]->cols; k++) {
                matrix_set(mat2[i], j, k, scanDouble(&addr));
            }
        }
    }
    
    fprintf(stderr, "Start...\n");
    
    /* Allocate and initialize threads, structure, arrays ... */
    calc.size = nbThread(mat1, mat2);
    threads = (pthread_t *) malloc( calc.size * sizeof(pthread_t) );
    
    calc.state=STATE_WAIT;
    calc.pendingCoefs = (size_t *) malloc(calc.size * sizeof(size_t));
    
    coefData=(size_t *) malloc( calc.size * sizeof(size_t *));
    for(i=0; i < calc.size; i++) {
        coefData[i]=i;
    }
    
    for(i=0; i<calc.size; i++) {
        pthread_create(&threads[i], NULL, calcul, (void *) &coefData[i]);
    }
    
    /* Start calculations */
    for(i=0; i<calc.nbMult; i++)
    {
        /* (Re)-Initialize the calculation structure */
        pthread_mutex_lock(&(calc.mutex));
        calc.result = matrix_init(mat1[i]->rows, mat2[i]->cols);
        initPendingCoefs(&calc);
        calc.m1 = mat1[i];
        calc.m2 = mat2[i];
        calc.state = STATE_CALC;
        
        /* Prevent starting calculation */
        pthread_cond_broadcast(&(calc.cond));
        
        /* Wait the end of the current calcultions */
        while(!(calc.state == STATE_PRINT)) {
            pthread_cond_wait(&(calc.cond), &(calc.mutex));
        }
        pthread_mutex_unlock(&(calc.mutex));
        
        /* Print the result of the current one */
        fprintf(fileOut, "Résultat #%zu :\n", i+1);
        matrix_fprint(calc.result, fileOut);
        fprintf(fileOut, "\n");
        
        /* Destroy the result */
        matrix_destroy(calc.result);
    }
    
    /* Wait for all threads termination */
    for(i=0;i<calc.size;i++) {
        pthread_join(threads[i], NULL);
    }
    
    /* Free resources */
    pthread_cond_destroy(&(calc.cond));
    pthread_mutex_destroy(&(calc.mutex));
    
    free(threads);
    
    for (i=0; i<calc.nbMult; i++) {
        matrix_destroy(mat1[i]);
        matrix_destroy(mat2[i]);
    }
    free(mat1);
    free(mat2);
    free(calc.pendingCoefs);
    
    munmap(fileaddr, sb.st_size);
    close(fd);
    
    /* Times up ! Print running time */
    printf("Temps d'exécution : %zu secondes.\n", time(NULL)-start);
    
    return EXIT_SUCCESS;
}
