#include <stdlib.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "../dSFMT-src-2.2.2/dSFMT.c"                 // dSFMT random number generator
#include "../dSFMT-src-2.2.2/dSFMT.h"                 // dSFMT random number generator
dsfmt_t dsfmt;   

 
double drand()   /* uniform distribution, (0..1] */
{
  return (dsfmt_genrand_close_open(&dsfmt));
}
double random_normal()  /* normal distribution, centered on 0, std dev 1 */
{
  return sqrt(-2*log(drand())) * cos(2*M_PI*drand());
}
int main()
{
  int i;
  double rands[1000];
  
  srand((unsigned)time(NULL));                              // Initialize the random number generator for the dSFMT seed.
  int seed = rand(); 
  
  dsfmt_init_gen_rand(&dsfmt, seed); 
  
  for (i=0; i<1000; i++){
    rands[i] = random_normal();
    printf("%f\n", rands[i]);
    }
  return 0;
}