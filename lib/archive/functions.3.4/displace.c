/*------------------------------------------------------------------------------
COMPETITION BETWEEN AN OCCUPIED CELL AND ITS NEIGHBORS

This function returns the result of a competition between an already occupied
cell and its neighbors.

ENTRY:    1.  'cell' is the identity of the species in the cell.
          2.  'habitat' is the habitat number associated with the cell.
          3.  'neighbors' is a 1D array of neighbor counts, indexed by neighbor species number - 1.
          4.  'n_species' is the number of species/genotypes in the simulation.
          5.  'displaceVector' is a 1D array containing all of the pairwise displacement proabilities for each species/habitat combination.
          
EXIT:     1.  The result of the competition (i.e. the ID of the species in the focal cell
              for the next time step)
*/


int displace(
  unsigned char cell,
  unsigned char habitat,
  unsigned char *neighbors,
  int           n_species,
  int           n_habitat,
  float         *displaceVector

  ){

  int i, j;                                 //  Loop variables.
  
  int over = n_species + 1;
  
  float densityEach[n_species];             //  Stores the probability density associated with each species in the interaction.
  float densitySum = 0;                     //  Stores the sum of all probability density, for normalization.
  float densityNone = 1;
  float densityCumul[n_species + 1];        //  Stores the cumulative colonization probability for each species.
  float rand_float = dsfmt_genrand_close_open(&dsfmt);    // A random float for determining which species wins.
 
  for(i = 0; i < n_species; i++){           //  Calculate the probability density associated with each neighbor:
    densityNone *= pow((1 - displaceVector[(habitat) * n_species * n_species + i * n_species + cell - 1]), neighbors[i]);
    densitySum += displaceVector[(habitat) * n_species * n_species + i * n_species + cell - 1] * neighbors[i];
                                            //  Sum of probability times number of neighbors of each type.
    densityEach[i] = displaceVector[(habitat) * n_species * n_species + i * n_species + cell - 1] * neighbors[i];
    }                                       //  Each element is the sum of the density times the number of neighbors of that type.
  
  densityCumul[0] = densityNone;            //  Set the first element of the cumulative density to the probability of the null event.
  
  for(i = 1; i <= n_species; i++)           //  Set the other cumulative density quantities, normalized by densitySum.
    densityCumul[i] = densityCumul[i - 1] + densityEach[i - 1] * (1 - densityNone) / densitySum;
    
  // if rand_float = 1, chose another rand_float:
  if(rand_float == 1) rand_float = dsfmt_genrand_close_open(&dsfmt);
  
  
  
  // Uncomment for troubleshooting
  /*
  printf("displace,\n");
  printf("cell = %d, habitat = %d, n_species = %d, n_habitat = %d\n", cell, habitat, n_species, n_habitat);
  for(i = 0; i < n_species * n_species; i++){
    printf("i = %d, i + n_species * n_species * habitat = %d\n", i, i + n_species * n_species * habitat);
    }
  printf("densitySum       = %f\n", densitySum);
  printf("densityNone      = %0.9f\n", densityNone);
  for(i = 0; i < n_species; i++){
    printf("neighbors[%d]     = %d\n", i + 1, neighbors[i]);
    }
  for(i = 0; i < n_species; i++){
    printf("densityEach[%i]   = %f\n", i, densityEach[i]);
    }
  for(i = 0; i <= n_species; i++){
    printf("densityCumul[%i]  = %f\n", i, densityCumul[i]);
    }  
  printf("rand_float        = %f\n", rand_float);
  */
  // Uncomment for troubleshooting
  
  


  for(i = 0; i <= n_species; i++)
    if(rand_float <= densityCumul[i]) break;   //  Compare random_float to the cumulative density array to determine winner.

  if(i == 0)
    return(cell);                          //  Return the original species in the case of the null event.
   
  if(i > n_species){
    printf("disturb returned cell = %d\n", i);
    printf("cell = %d, habitat = %d, n_species = %d, n_habitat = %d\n", cell, habitat, n_species, n_habitat);
    for(i = 0; i < n_species * n_species; i++){
      printf("i = %d, i + n_species * n_species * habitat = %d\n", i, i + n_species * n_species * habitat);
      }
    printf("densitySum       = %f\n", densitySum);
    printf("densityNone      = %0.9f\n", densityNone);
    for(i = 0; i < n_species; i++){
      printf("neighbors[%d]     = %d\n", i + 1, neighbors[i]);
      }
    for(i = 0; i < n_species; i++){
      printf("densityEach[%i]   = %f\n", i, densityEach[i]);
      }
    for(i = 0; i <= n_species; i++){
      printf("densityCumul[%i]  = %f\n", i, densityCumul[i]);
      }  
    printf("rand_float        = %f\n", rand_float);
    }
  else return(i);                             //  Return the winning species.
}
