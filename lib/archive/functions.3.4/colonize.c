/*------------------------------------------------------------------------------ +-
COLONIZE AN EMPTY CELL

This function uses the habitat value of the focal cell, as well as the tally of neighbors of each distinct
species to colonize an empty cell.  

ENTRY:      1.  'habitat' is the value of the habitat in the empty cell.
            2.  'neighbors' is a vector containing the frequency of each 
                species that occurs in the neighborhood of the 
                empty cell, indexed by the species number.  Does this start at 1?
            3.  'n_species' is the number of distinct species in the model.
            4.  'colonizeVector is a 1D array containing the probability of colonization
                for each habitat/species combination.


EXIT:       1.  Colonize returns the identity of the colonizing species, or 0 if no species colonized.

FUNCTIONS:  1.  dsfmt_genrand_close_open()
*/

#include <math.h>

int colonize(
  unsigned char habitat,
  unsigned char *neighbors,
  int           n_species,
  float         *colonizeVector
  )
{ 
  int i, j;                                   // Variables for loops.

  float densityEach[n_species];           // Vector to hold the probability density of each species colonizing.
  float densitySum = 0;
  float densityNone = 1;                          // Product of the probabilities of each neighbor species not colonizing.
  float densityCumul[n_species + 1];
  float rand_float = dsfmt_genrand_close_open(&dsfmt);

  for(i = 0; i <n_species; i++){
    densityNone *= pow((1 - colonizeVector[i + n_species * habitat]), neighbors[i]);  // Density of having no colonization.
    densitySum += colonizeVector[i + n_species * habitat] * neighbors[i];             // Sum of all the neighbor colonization densities.
    densityEach[i] = colonizeVector[i + n_species * habitat] * neighbors[i];        // Sum of all the neighbor colonization densities.
    }

  // Set the first element of densityCumul to the probability of no colonization:
  densityCumul[0] = densityNone;

  for(i = 1; i <= n_species; i++){
    densityCumul[i] = densityCumul[i - 1] + densityEach[i - 1] * (1 - densityNone) / densitySum; 
    }                                           // set the cumulative density to the corresponding element in densityEach normalized by the probability of no colonization and the total sum of all densities.

    
  /*
  //Uncomment for troubleshooting:
  
  printf("\nhabitat = %d\n", habitat);
  for(i = 0; i < n_species; i++){
    printf("i = %d, i + n_species * habitat = %d\n", i, i + n_species * habitat);
    printf("colonizeVector[%d] = %f\n", i + n_species * habitat, colonizeVector[i + n_species * habitat]);
    } 
    
  printf("densitySum       = %f\n", densitySum);
  printf("densityNone      = %f\n", densityNone);
  for(i = 0; i < n_species; i++){
    printf("neighbors[%d]     = %d\n", i, neighbors[i]);
    }
  for(i = 0; i < n_species; i++){
    printf("densityEach[%i]   = %f\n", i, densityEach[i]);
    }
  for(i = 0; i <= n_species; i++){
    printf("densityCumul[%i]  = %f\n", i, densityCumul[i]);
    }  
  printf("rand_float        = %f\n", rand_float);
  
  //Uncomment for troubleshooting.
  */
  
  for(i = 0; i <= n_species; i++)
    if(rand_float < densityCumul[i]) break;
  
  if(i > n_species) return(0);
  
  else return(i);
  
 } // colonize

