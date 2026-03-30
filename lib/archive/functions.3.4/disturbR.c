/*------------------------------------------------------------------------------- +-
CREATE RANDOM DISTURBANCE PATTERNS IN THE FIELD GRID

This function creates randomly placed square disturbances in the field grid of specified size (distPatchSize).
The disturbances consist of removing all individual species within the disturbed patch.


ENTRY:  1.  The size of the grid:  'gridSize'
        2.  'n_species' is the number of species in the simulation.
        3.  'gridIn' is the 2D array holding the current state of the field.
        4.  'distType' is the disturbance shape: 1 = square, 2 = round.
        5.  'distPatchNumber' is the number of disturbance patches to create.
        6.  'distPatchSize' is the radius/side length of the disturbance patches.
        7.  'distPatchNumber' is the number of disturbance patches to create.
        8.  'distCensusAll'
        
EXIT:   1.  The field grid with the randomly generated disturbances: 'gridIn'
*/


void disturbR(
  int           gridSizeX,
  int           gridSizeY,
  int           n_species,
  unsigned char **gridIn,
  int           distShape,
  int           disturbRateMode,
  int           distPatchSize,
  float         disturbSD,
  int           *distCensusAll,               // Count of the total number of disturbed cells.
  long int      *distCensusKilled // Count of the number of killed cells.
  ){
  
  int i, j, p, l;
  float r_norm;   
  unsigned char distGrid[gridSizeX][gridSizeY];         // Disturbance pattern.

  int distPatchNumber = 0;
  
  // Determine the number of patches based on disturbRateMode
  
  //printf("disturb\n");
  
  //while((r_norm = random_normal(disturbRateMode, disturbSD))){
  //  distPatchNumber = r_norm;
  //  if(r_norm >= 0) break;
  //  }
    
  distPatchNumber = random_normal(disturbRateMode, disturbSD);
  
  if(distPatchNumber < 0) distPatchNumber = 0;
    
  printf("%d disturbance patches.\n", distPatchNumber);

  int x[distPatchNumber];                   // Vector of x-coordinates of the disturbance patches
  int y[distPatchNumber];                   // Vector of y-coordinates of the disturbance patches
  
  for(i = 0; i < gridSizeX; i++)            // Clear the disturbance grid.
  for(j = 0; j < gridSizeY; j++)
    distGrid[i][j] = 0;
    
  *distCensusAll = 0;                       // Reset the total disturbed cells tally.
  
  // generate the random coordinates for the centers of the disturbance patches:
  for(i = 0; i < distPatchNumber; i++){
    //x[i] = rand()%gridSize;                 // Generate the x-coordinates. 
    x[i] = dsfmt_genrand_uint32(&dsfmt)%gridSizeX;                 // Generate the x-coordinates. 
    //y[i] = rand()%gridSize;                 // Generate the y-coordinates.
    y[i] = dsfmt_genrand_uint32(&dsfmt)%gridSizeY;                // Generate the y-coordinates.
    }
    
  // create the disturbance patches:
  // Square patches:
  if(distShape == 1){
    printf("dist = square\n");
    for(p = 0; p < distPatchNumber; p++){     // Loop through each disturbance patch.
      for(i = x[p] - distPatchSize/2 ; i < x[p] + distPatchSize/2; i++)   // Go from half the disturbance patch size to the left                                                                         // to half the patch size to the right.
      for(j = y[p] - distPatchSize/2; j < y[p] + distPatchSize/2; j++){  // Go from half the disturbance patch size up to
        if(gridIn[(i + gridSizeX)%gridSizeX][(j + gridSizeY)%gridSizeY]){                            // If the focal cell is occupied:
          ++distCensusKilled[gridIn[(i + gridSizeX)%gridSizeX][(j + gridSizeY)%gridSizeY] - 1];                        // Increment the census of killed cells
          gridIn[(i + gridSizeX)%gridSizeX][(j + gridSizeY)%gridSizeY] = 0;                          // Clear the cell.
          }
        distGrid[i][j] = 1;
        } // for(j)
      } // for(p)
    } // if(distType == 1)      
  
  // Round patches:        
  if(distShape == 2){
    printf("dist = circle\n");
    for(p = 0; p < distPatchNumber; p++){     // Loop through each disturbance patch.
      for(i = x[p] - distPatchSize/2; i < x[p] + distPatchSize/2; i++)   // Go from half the disturbance patch size to the left
      // why the plus and minus 1?                                                                           // to half the patch size to the right.
      for(j = y[p] - distPatchSize/2; j < y[p] + distPatchSize/2; j++){  // Go from half the disturbance patch size up to
                                                // half the disturbance patch size down.
        // If the coordinate for the focal cell is within the field boundaries, and the distance from the center of the disturbance is 
        // equal to or less than the radius of the distPatchSize:
        if(sqrt(pow(i - x[p], 2.0) + pow(j - y[p], 2.0)) < distPatchSize/2 - 1){
          if(gridIn[(i + gridSizeX)%gridSizeX][(j + gridSizeY)%gridSizeY]){                            // If the focal cell is occupied:
            ++distCensusKilled[gridIn[(i + gridSizeX)%gridSizeX][(j + gridSizeY)%gridSizeY] - 1];                        // Increment the census of killed cells
            gridIn[(i + gridSizeX)%gridSizeX][(j + gridSizeY)%gridSizeY] = 0;                          // Clear the cell.
          }
          gridIn[(i + gridSizeX)%gridSizeX][(j + gridSizeY)%gridSizeY] = 0;
          } // if()
        } // for(j)
      } // for(p)  
    } // if(distType == 2)
    
  for(i = 0; i < gridSizeX; i++)              // Count the number of disturbed cells (occupied or not).
  for(j = 0; j < gridSizeY; j++)
    if(distGrid[i][j])
      ++*distCensusAll;                             // Increment the counter
  
} // disturb


