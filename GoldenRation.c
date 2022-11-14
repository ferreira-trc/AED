#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double Golden_Ration(int count){

    double gr;
    double n;

    n = 1;    

    for (int i = 0; i < count; i++)
    {
        n = 1 + (1/n);
        printf("AP%3d: %.20f \n",i ,n);
    }

    gr = 1 + (1/n);

    return gr;
    
}

int main(int argc, char *argV[]){

    int x = atoi(argV[1]);
    double gr = (1 + sqrt(5))/2;

    printf("\nEX   : %.20f \nAP %2d: %.20f \n", Golden_Ration(x), x, gr);
    return 0;
}