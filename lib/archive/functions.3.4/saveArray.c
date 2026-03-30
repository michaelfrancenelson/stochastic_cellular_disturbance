/*-------------------------------------------------------------------------------
SAVE THE FIELD GRID TO A FILE.

This function saves the field array to a file.

ENTRY:  A 2-D array, 'gridIn'

EXIT:   'gridIn' is saved to a text file
*/

void saveArray(int gridSizeX, int gridSizeY, unsigned char **gridIn, char *directory, char *prefix, int gen, int rep)
{
  int i, j;
  char dir[LINE_LENGTH];
  char filePrefix[LINE_LENGTH];
  char fileSuffix1[LINE_LENGTH];
  char fileSuffix2[LINE_LENGTH];
  char sep[100] = ".";
  char txt[100] = ".txt";
  char repp[100] = "r";  
  
  sprintf(fileSuffix1, "%d", gen);              // Assign the generation number to 'fileSuffix1'.
  sprintf(fileSuffix2, "%d", rep);              // Assign the rep number to 'fileSuffix2'.
  
  strcpy(dir, directory);
  strcpy(filePrefix, prefix);

  FILE *outFile;
  
  strcat(dir, filePrefix);              // Concatenate the directory and the file prefix.
  strcat(dir, fileSuffix1);             // Add the generation number.
  strcat(dir, ".");                     // Add a dot.
  strcat(dir, repp);                    // Add an 'r'.
  strcat(dir, fileSuffix2);             // Add the rep number.
  strcat(dir, ".txt");                  // Add the '.txt' extension

  outFile = fopen(dir, "w");            // Open the file for writing.

  for(i = 0; i < gridSizeX; i++){        // Loop through the x and y coordinates of the field.
    for(j = 0; j < gridSizeY; j++){
      fprintf(outFile, "%d ", gridIn[i][j]);  // Save the genotype in the corresponding cell, separated by a space.
      }
    fprintf(outFile, "\n");             // Save a newline character at the end of each row.
    }   
     
  fclose(outFile);                     // Close the file.
  }
     
