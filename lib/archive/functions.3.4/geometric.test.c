
#define DSFMT_MEXP 19937 

#include <stdio.h>
#include <time.h>                                     // to generate a random seed
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../dSFMT-src-2.2.2/dSFMT.c"                 // dSFMT random number generator
#include "../dSFMT-src-2.2.2/dSFMT.h"                 // dSFMT random number generator
dsfmt_t dsfmt;   

#include "Bernoulli.c"
//#include "geometric.c"

                                     // Required for dSFMT.


int main()
{
  float p = 0.001;
  
  int i = 0;
  
  srand((unsigned)time(NULL));                              // Initialize the random number generator for the dSFMT seed.
  int seed = rand(); 
  
  dsfmt_init_gen_rand(&dsfmt, seed);                        // Initialize dSFMT.
  
  
  while(dsfmt_genrand_close_open(&dsfmt) >= p)
    i++;
  
  printf("p = %f, ", p);
  printf("i = %d\n", i);

} //Bernoulli()