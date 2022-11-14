#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct cont_frac_aprox cont_frac_aprox_t;

struct cont_frac_aprox
{
    int indep;
    int num;
    int lastnum;
    cont_frac_aprox_t *cont;
};

