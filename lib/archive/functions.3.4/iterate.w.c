/*-------------------------------------------------------------------------------
EVOLVE THE BOARD BY ONE STEP (wrapping boundaries)

COUNT THE NUMBER OF AND NOTE THE ID OF ALL THE NEIGHBORS OF A FOCAL CELL IN THE FIELD fieldIn AND
INVOKE THE COMPETITION AND COLONIZATION FUNCTIONS.

This function counts the total number of neighbors and the number of each species of neighbor in the 3X3 
neighborhood of a focal cell.  If the focal cell is occupied, the competition and death functions are called.
If the focal cell is unoccupied but has at least one neighbor, the neighbor counts are passed to the 
colonization function.

ENTRY:  1.  'gridIn' points to the input field grid.
        2.  'gridOut' points to the output field grid.
        3.  'habitatGrid' points to the grid of habitat values.
        4.  'colonizeProbs' points to the growth probability array.
        5.  'deathEans' points to the death probability array.
        6.  'n_species' is the number of species in the simulation.
        7.  'i' is the row coordinate of the focal cell.
        8.  'j' is the horizontal coordinate of the focal cell.

EXIT:   1.  The output field grid cell with updated individuals
*/

void iterate_w(
  int           gridSizeX,
  int           gridSizeY,
  unsigned char **gridIn,
  unsigned char **gridOut,
  unsigned char **habitatGrid,
  float         *colonizeVector,
  int           displaceYN,
  float         *displaceVector,
  float         *deathVector,
  int           n_species,
  int           n_habitat,
  int           i,
  int           j)
{



  
  int k, l, m;                                      // Create variables for loops.
  int n_counter = 0;                                // Create the total neighbor counter and initialize it to zero.
  int n_others = 0;
  unsigned char neighbors[n_species];           // Create a 1-D array to hold the counts of neighbors of each species.
  

  for(k = 0; k < n_species; k++)                   // Initialize the species-specific neighbor counter to 0.
    neighbors[k] = 0;

  for(k = i - 1; k <= i + 1; k++)                   // Loop through the 3X3 neighbor window.
  for(l = j - 1; l <= j + 1; l++)  
    if(gridIn[(k + gridSizeX)%gridSizeX][(l + gridSizeY)%gridSizeY]){     // If neighboring cell gridIn[k][l] is occupied:
      neighbors[gridIn[(k + gridSizeX)%gridSizeX][(l + gridSizeY)%gridSizeY] - 1]++;    
      n_counter++;                                  // Increment the corresponding element in the species-specific neighbor counter.
      if(gridIn[i][j] != gridIn[(k + gridSizeX)%gridSizeX][(l + gridSizeY)%gridSizeY])
        n_others++;                                 // Increment the total neighbor counter.
      }//if(gridIn[k...]

  if(n_counter&&!gridIn[i][j])                      // colonize if unoccupied and there are any neighbors
    gridOut[i][j] = colonize(habitatGrid[i][j], neighbors, n_species, colonizeVector);

  if(gridIn[i][j]){                                 // If the focal cell is occupied:
    neighbors[gridIn[i][j] - 1]--;                      // Subtract the focal cell species from the species-specific neighbor count.
    n_counter--;                                    // Subtract the focal cell from the total neighbor count.
    if(n_others * displaceYN > 0){
      gridOut[i][j] = displace(gridIn[i][j], habitatGrid[i][j], neighbors, n_species, n_habitat, displaceVector);
      }
    else gridOut[i][j] = gridIn[i][j];
    }//if(gridIn[i][j])

  if(gridOut[i][j]>0)                               // if the focal cell is occupied, invoke the death probability function.
    gridOut[i][j] = deathPanel(n_species, gridOut[i][j], habitatGrid[i][j], deathVector);

  // Uncomment for troubleshooting
  /*
  
  printf("\n\ntroubleshooting iterate.w.c\n");
  printf("gridSizeX = %d, gridSizeY = %d\n", gridSizeX, gridSizeY);
  printf("gridIn[%d][%d] = %d\n", i, j, gridIn[i][j]);
  for(k = i - 1; k <= i + 1; k++){                   // Loop through the 3X3 neighbor window.
    for(l = j - 1; l <= j + 1; l++){
      printf(" %d ", gridIn[(k + gridSizeX)%gridSizeX][(l + gridSizeY)%gridSizeY]);
    }
    printf("\n");
  }
  printf("habitatGrid[%d][%d] = %d\n", i, j, habitatGrid[i][j]);
  for(i = 0; i < n_habitat * n_species; i++)
    printf("colonizeVector[%d] = %f\n", i, colonizeVector[i]);
  for(i = 0; i < n_habitat * n_species; i++)
    printf("deathVector[%d] = %f\n", i, deathVector[i]);
  printf("n_species = %d, n_habitat = %d, i = %d, j = %d\n", n_species, n_habitat, i, j);
  for(i = 0; i < n_species; i++)
    printf("neighbors[%d] = %d\n", i + 1, neighbors[i]);
  printf("n_counter = %d\n", n_counter);
  printf("n_others = %d\n", n_others);
  printf("n_others & displaceYN =%d\n", n_others * displaceYN > 0);
  printf("displaceYN = %d", displaceYN);
  printf("\n\ntroubleshooting iterate.w.c\n\n");
  */
  
  
}// iterate()