/*------------------------------------------------------------------------------- 
This function outputs the resuts of one or more Bernoulli trials.

Entry:    1.  'n' is the number of trials to perform.
          2.  'p' is the probability of success.

EXIT:     1.  'berOut' is the number of successes.  


*/

int Bernoulli(int n, float p)
{
  int i, sum = 0;
  
  for(i = 0; i < n; i++)
    if(dsfmt_genrand_close_open(&dsfmt) <= p)    // A random float for determining which species to output
      sum++;
      
  return(sum);

} //Bernoulli()