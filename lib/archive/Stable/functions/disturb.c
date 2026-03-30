/*------------------------------------------------------------------------------- +-
CREATE RANDOM DISTURBANCE PATTERNS IN THE FIELD GRID

This function creates randomly placed square disturbances in the field grid of specified size (distPatchSize).
The disturbances consist of removing all individual species within the disturbed patch.


ENTRY:  1.  The size of the grid:  'gridSize'
        2.  The field grid:  'gridIn'
        3.  The disturbance shape: 1 = square, 2 = round.
        4.  The number of disturbance patches to create: 'distPatchNumber'
        5.  The size of the disturbed patches:  'distPatchSize'
        
EXIT:   1.  The field grid with the randomly generated disturbances: 'gridIn'
*/

#include <math.h>  

void disturb(
  int           gridSize,
  int           n_species,
  unsigned char **gridIn,
  int           distType,
  int           distPatchNumber,
  int           distPatchSize,
  int           *distCensusAll,               // Count of the total number of disturbed cells.
  long int      *distCensusKilled // Count of the number of killed cells.
  ){
  
  int i, j, p, l;
  int x[distPatchNumber];                   // Vector of x-coordinates of the disturbance patches
  int y[distPatchNumber];                   // Vector of y-coordinates of the disturbance patches
    
  unsigned char distGrid[gridSize][gridSize];         // Disturbance pattern.

  for(i = 0; i < gridSize; i++)
  for(j = 0; j < gridSize; j++)
    distGrid[i][j] = 0;
  
  *distCensusAll = 0;                       // Reset the total disturbed cells tally.
  
  // generate the random coordinates for the centers of the disturbance patches:
  for(i = 0; i < distPatchNumber; i++){
    //x[i] = rand()%gridSize;                 // Generate the x-coordinates. 
    x[i] = dsfmt_genrand_uint32(&dsfmt)%gridSize;                 // Generate the x-coordinates. 
    //y[i] = rand()%gridSize;                 // Generate the y-coordinates.
    y[i] = dsfmt_genrand_uint32(&dsfmt)%gridSize;                // Generate the y-coordinates.
    }
    
  // create the disturbance patches:
  // Square patches:
  if(distType == 1){
    printf("dist = square\n");
    for(p = 0; p < distPatchNumber; p++){     // Loop through each disturbance patch.
      for(i = x[p] - distPatchSize/2 ; i < x[p] + distPatchSize/2; i++)   // Go from half the disturbance patch size to the left
      // why the plus and minus 1?                                                                           // to half the patch size to the right.
      for(j = y[p] - distPatchSize/2; j < y[p] + distPatchSize/2; j++){  // Go from half the disturbance patch size up to
                                                                                  // half the disturbance patch size down.
        // If the coordinate for the focal cell is within the field boundaries:
        if(i >= 0 && j >=0 && i<gridSize && j <gridSize){
          if(gridIn[i][j]){                            // If the focal cell is occupied:
            ++distCensusKilled[gridIn[i][j] - 1];                        // Increment the census of killed cells
            gridIn[i][j] = 0;                          // Clear the cell.
            }
          distGrid[i][j] = 1;
          } // if()
        } // for(j)
      } // for(p)
    } // if(distType == 1)      
  
  // Round patches:        
  if(distType == 2){
    printf("dist = circle\n");
    for(p = 0; p < distPatchNumber; p++){     // Loop through each disturbance patch.
      for(i = x[p] - distPatchSize/2; i < x[p] + distPatchSize/2; i++)   // Go from half the disturbance patch size to the left
      // why the plus and minus 1?                                                                           // to half the patch size to the right.
      for(j = y[p] - distPatchSize/2; j < y[p] + distPatchSize/2; j++){  // Go from half the disturbance patch size up to
                                                                                  // half the disturbance patch size down.
        // If the coordinate for the focal cell is within the field boundaries, and the distance from the center of the disturbance is 
        // equal to or less than the radius of the distPatchSize:
        if(i >= 0 && j >=0 && i<gridSize && j <gridSize && sqrt(pow(i - x[p], 2.0) + pow(j - y[p], 2.0)) < distPatchSize/2 - 1){
          if(gridIn[i][j]){                            // If the focal cell is occupied:
            ++distCensusKilled[gridIn[i][j] - 1];                        // Increment the census of killed cells
            gridIn[i][j] = 0;                          // Clear the cell.
            }
          distGrid[i][j] = 1;
          } // if()
        } // for(j)
      } // for(p)  
    } // if(distType == 2)
    
  for(i = 0; i < gridSize; i++)              // Count the number of disturbed cells (occupied or not).
  for(j = 0; j < gridSize; j++)
    if(distGrid[i][j])
      ++*distCensusAll;                             // Increment the counter
  
} // disturb


