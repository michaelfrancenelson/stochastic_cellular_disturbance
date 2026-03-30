/*------------------------------------------------------------------------------ +-
COLONIZE AN EMPTY CELL

This function uses the resource value of the focal cell, as well as the tally of neighbors of each distinct
species to colonize an empty cell.  

ENTRY:      1.  'resValue' is the value of the resource in the empty cell.
            2.  'neighbor_count' is a vector containing the frequency of each 
                species that occurs in the neighborhood of the 
                empty cell, indexed by the species number.  Does this start at 1?
            3.  'neighbor_total' is the total number of occupied neighboring cells.
            4.  Each row of the 'colonizeProbs' matrix corresponds to the identity of 
                a species, while columns represent the possible values of the resource.  
                Each entry is the probability that the given species will colonize the empty
                cell as a function of the given resource level. 
            5.  'n_species' is the number of distinct species in the model.


EXIT:       1.  Colonize returns the identity of the colonizing species, or 0 if no species colonized.

FUNCTIONS:  1.  dsfmt_genrand_close_open()
*/

#include <math.h>

int colonize(
  unsigned char resValue,
  unsigned char *neighbor_count,
  unsigned char neighbor_total,
  int           n_species,
  float         **colonizeProbs
  )
{ 
  int i, j, x;                                   // Variables for loops and counters

  float densityEach[n_species];           // Vector to hold the probability density of each species colonizing.
  float densitySum = 0;
  float spProbSum = 0;                           // Sum of probabilty density for each genotype.
  float densityNone = 1;                          // Product of the probabilities of each neighbor species not colonizing.
  float densityCumul[n_species + 1];
  float rand_float = dsfmt_genrand_close_open(&dsfmt);
  
  for(i = 0; i <n_species; i++){
    densityNone *= pow((1 - colonizeProbs[i][resValue]), neighbor_count[i]);  // Density of having no colonization.
    densitySum += colonizeProbs[i][resValue] * neighbor_count[i];             // Sum of all the neighbor colonization densities.
    densityEach[i] = colonizeProbs[i][resValue] * neighbor_count[i];
    }
  
    // Set the first element of densityEach to the probability of no colonization:
  
  densityCumul[0] = densityNone;
  
    // normalize the densities to sum to (1 - densityNone);
  for(i = 1; i <= n_species; i++){
    densityEach[i] *= (1 - densityNone) / densitySum;
    densityCumul[i] = densityCumul[i - 1] + densityEach[i - 1];
    }
  
  for(i = 0; i <= n_species; i++){
    if(rand_float <= densityCumul[i])
      break;
  }
  
  return(i);
 } // colonize

