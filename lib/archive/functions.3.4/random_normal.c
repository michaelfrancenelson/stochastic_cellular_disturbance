#include <math.h>


/* Generate a random Normal number using Box-Muller transform.*/
double random_normal(float mu, float sd)  /* normal distribution, centered on 0, std dev 1 */
{ 
  double randU = dsfmt_genrand_close_open(&dsfmt);
  return mu + sd * (sqrt(-2*log(randU)) * cos(2*M_PI*randU));
}
