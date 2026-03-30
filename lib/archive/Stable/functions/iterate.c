/*-------------------------------------------------------------------------------
EVOLVE THE BOARD BY ONE STEP

COUNT THE NUMBER OF AND NOTE THE ID OF ALL THE NEIGHBORS OF A FOCAL CELL IN THE FIELD fieldIn AND
INVOKE THE COMPETITION AND COLONIZATION FUNCTIONS.

This function counts the total number of neighbors and the number of each species of neighbor in the 3X3 
neighborhood of a focal cell.  If the focal cell is occupied, the competition and death functions are called.
If the focal cell is unoccupied but has at least one neighbor, the neighbor counts are passed to the 
colonization function.

ENTRY:  1.  'gridIn' points to the input field grid.
        2.  'gridOut' points to the output field grid.
        3.  'resGrid' points to the grid of resource values.
        4.  'colonizeProbs' points to the growth probability array.
        5.  'deathEans' points to the death probability array.
        6.  'n_species' is the number of species in the simulation.
        7.  'i' is the horizontal coordinate of the focal cell.
        8.  'j' is the vertical coordinate of the focal cell.

EXIT:   1.  The output field grid cell with updated individuals
*/

void iterate(
  int           gridSize,
  unsigned char **gridIn,
  unsigned char **gridOut,
  unsigned char **resGrid,
  float         **colonizeProbs,
  float         **deathProbs,
  int           displaceYN,
  float         *displaceVector,
  int           n_species,
  int           n_resource,
  int           i,
  int           j)
{
  

  
  int k, l, n, t;                                     // Create variables for loops.
  int n_counter = 0;                                  // Create the total neighbor counter and initialize it to zero.
  int n_others = 0;                                    // Create a counter for non-self neighbors, initialize it to 0.
  unsigned char neighbors[n_species];                 // Create a 1-D array to hold the counts of neighbors of each species.
  
  for(k = 0; k < n_species; k++)                      // Initialize the species-specific neighbor counter to 0.
    neighbors[k] = 0;
                                  
  for(k = i - 1; k <= i + 1; k++){                    // Loop through the 3X3 neighbor window.
  for(l = j - 1; l <= j + 1; l++){  
    if(gridIn[k][l]){                                // If neighboring cell gridIn[k][l] is occupied:
      neighbors[gridIn[k][l] - 1]++;                 // Increment the corresponding element in the species-specific neighbor counter.
      n_counter++;                                   // Increment the total neighbor counter.
      if(gridIn[i][j] != gridIn[k][l])
        n_others++;
      }//if(gridIn[k][l]

   }}// for k and l

  if(gridIn[i][j]){                                   // If the focal cell is occupied:
    neighbors[gridIn[i][j] - 1]--;                    // Subtract the focal cell species from the species-specific neighbor count.
    n_counter--;                                      // Subtract the focal cell from the total neighbor count.
    if(n_others & displaceYN)
      gridOut[i][j] = displace(gridIn[i][j], resGrid[i][j], displaceVector, n_species, n_resource, neighbors);
    else gridOut[i][j] = gridIn[i][j];
    }//if(gridIn[i][j])                               // Invoke the displacement function.
  
  if(n_counter&&!gridIn[i][j]) {                      // colonize if unoccupied and there are any neighbors
    gridOut[i][j] = colonize(resGrid[i][j], neighbors, n_counter, n_species, colonizeProbs);
    }

  if(gridOut[i][j]>0){                                 // if the focal cell is occupied, invoke the death probability function.
    gridOut[i][j] = deathPanel(gridOut[i][j], resGrid[i][j], deathProbs);
    }

}// iterate()

