/*------------------------------------------------------------------------------
COMPETITION BETWEEN AN OCCUPIED CELL AND ITS NEIGHBORS

This function returns the result of a competition between an already occupied
cell and its neighbors.

ENTRY:    1.  'cell' is the identity of the species in the cell.
          2.  'neighbors' is the vector of neighbor counts.
          
EXIT:     1.  The result of the competition (i.e. the ID of the species in the focal cell
              for the next time step)
*/


int displace(
  unsigned char cell,
  unsigned char resource,
  float *displaceVector,
  int n_species,
  int n_resource,
  unsigned char *neighbors
  ){

  int i, j;
  
  float densityEach[n_species];
  float densitySum = 0;
  float spProbSum = 0;
  float densityCumul[n_species];
  float rand_float = dsfmt_genrand_close_open(&dsfmt);    // A random float for determining which species wins.
  

  
  for(i = 0; i < n_species; i++){
    densitySum += displaceVector[(resource) * n_species * n_species + i * n_species + cell - 1] * neighbors[i];
    densityEach[i] = displaceVector[(resource) * n_species * n_species + i * n_species + cell - 1] * neighbors[i];
    }
  
  densityCumul[0] = densityEach[0] / densitySum;
  
  for(i = 1; i < n_species; i++){
    densityCumul[i] = densityCumul[i-1] + densityEach[i] / densitySum;
  }

  
  
  /*
  printf("n_species = %d, n_resource = %d, cell = %d, resource = %d\n", n_species, n_resource, cell, resource);
  
  for(i = 0; i < n_species; i++)
    printf("neighbors[%d] = %d\n", i, neighbors[i]);
  
  for(i = 0; i < n_species; i++){
    printf("displaceVector[%d] = %f\n", (resource) * n_species * n_species + i * n_species + cell - 1, displaceVector[resource * n_species * n_species + i * n_species + cell - 1]);
  }
  
  for(i = 0; i < n_species; i++)
    printf("densityEach[%d] = %f\t", i, densityEach[i]);
  printf("\n");
  
  
  for(i = 0; i < n_species; i++)
    printf("densityCumul[%d] = %f\t", i, densityCumul[i]);
  printf("\n");
  
  
  //printf("hi\n");
  printf("i = %d\n", i);
  
  
  */
  
  for(i = 0; i < n_species; i++){
    if(rand_float <= densityCumul[i])
      break;
  }
  
  
   return(i + 1);
  
  //return(cell);
}
