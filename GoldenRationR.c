#include <math.h>
#include <stdio.h>
#include <assert.h>



typedef signed long long s64; // 64-bit signed integer


double Golden_Ration_R(int ctrl)
{
  if (ctrl == 0)
  {
    return 1;
  } else
  {
    return 1 + 1/Golden_Ration_R(ctrl-1);
  }  
}

int main(){

    double gr = (1 + sqrt(5))/2;

    printf("\nEX   : %.20f \nAP 10: %.20f \n", Golden_Ration_R(10), gr);
    return 0;
}