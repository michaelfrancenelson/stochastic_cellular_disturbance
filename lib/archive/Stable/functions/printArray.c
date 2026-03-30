/*-------------------------------------------------------------------------------
PRINT THE CURRENT PLAYING FIELD TO THE SCREEN.

This function prints the input field array to the terminal window.

ENTRY:  A 2-D array, 'grid'

EXIT:   'grid' is printed to the terminal
*/

void printArray(int gridSize, unsigned char **grid)
{
  int i, j;                                      // Loop variables

  for(i = 0; i < gridSize; i++){                 // Loop through the x and y coordinates of the field.
    for(j = 0; j < gridSize; j++)
      printf("%d ", grid[i][j]);                 // Print the species ID in the corresponding space.  If it is unoccpuied, print '0'.
    printf("\n");
    } // for(i)
}  
