//
// Tomás Oliveira e Silva, AED, October 2021
//
// program to print a table of the squares and square roots of some integers
//
// on GNU/Linux, run the command
//   man 3 printf
// to see the manual page of the printf function
//

#include <math.h>
#include <stdio.h>

void do_it(int N)
{
  int i;

  printf(" n Degree      cos (n)           sin(n)\n");
  printf("-- ------ ----------------- -----------------\n");
  for(i = 0;i <= N;i++)
    printf("%2d %3dº   %17.15f %17.15f\n",i, (i*360)/N,cos((double)(i*2*M_PI)/N), sin((double)(i*2*M_PI)/N));
}

int main(void)
{
  do_it(360);
  return 0;
}
