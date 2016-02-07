#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "matrix.h"
#include "product.h"

Calculation calc;

size_t nbThread(struct matrix ** mat1, struct matrix **mat2)
{
    int max = 0, i, mp;
    for (i=0; i < calc.nbMult; i++) {
        mp = mat1[i]->rows * mat2[i]->cols;
        if (max < mp) {
            max = mp;
        }
    }
    return max;
}

double scanDouble(char ** str)
{
    size_t i = 0;
    char c, buf[16];
    
    /* Move forward until caracter is the beginning of the number */
    while ( **str != 45 && **str != 46 && !(**str >= 48 && **str <= 57)) {
        *str += 1;
    }
    
    /* Get the number */
    while ( ((c = **str) >= 48 && c <= 57) || c == 46 || c == 45) { // [0-9.-]
        buf[i++] = c;
        *str += 1;
    }
    buf[i] = '\0';
    
    return atof(buf);
}

void initPendingCoefs(Calculation * prod)
{
    size_t i;
    for(i=0;i<prod->size;i++)
    {
        prod->pendingCoefs[i]=1;
    }
}

int nbPendingCoefs(Calculation * prod)
{
    size_t i;
    int nb=0;
    for(i=0;i<prod->size;i++)
    {
        nb += prod->pendingCoefs[i];
    }
    return(nb);
}
