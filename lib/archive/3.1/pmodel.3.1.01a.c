/* ============================================================================
Description of program:

This cellular automation program models the spatial growth of one or more species, each having different 
growth characteristics in different environments.

A square playing field (fieldGrid) is used for habitat, each cell of which can be unoccupied or occupied
by one individual of one of the species.  The environments are specified by another square 
grid (resourceGrid).  

Growth is inspired by the vegetative growth of a clonal plant; each unoccupied cell adjacent to an occupied
cell has a probability of becoming occupied in the subsequent time step.  Growth parameters are specified 
for each species in each environment by the resource probability file (fileResEqn).

Each occupied cell has a probability of becoming empty in the subsequent time step according to the values
specified for each species/environment combination in the death probability file (fileDeathEqn).

Periodic habitat disruption is implemented by clearing a specified number (distPatchNumber) of randomly 
positioned squares of a specified size (distPatchSize) of all individuals at specified intervals (distInterval).

A census of each species is taken at each time step and output to a census file (censusOut).

A snapshot of the playing field can be saved so that simulations can be re-started later.




Description of updates:

April 2013:  Changed 'genotype' to 'species'.  This change does not affect the how the program
runs in any way, but using the word species makes more intuitive sense.

April 2013:  Added code to make program stop if all but one of the species goes extinct.

===============================================================================*/



#define ELEMENT_SIZE 8                                // size (in bytes) of each grid element
#define LINE_LENGTH 1000                              // maximum line length for an input file 
#define FILE_LENGTH 256                               // maximum number of lines in the configuration file
#define DSFMT_MEXP 19937                              // period length of the dSFMT random number generator
          
#include <stdio.h>
#include <time.h>                                     // to generate a random seed
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int compete(
  unsigned char cell,
  unsigned char *neighbors
  );

int colonize(
  unsigned char resValue,
  unsigned char *neighbor_count,
  unsigned char neighbor_total,
  int           n_species,
  float         **colonizeProbs
  );
  
  
int deathPanel(
  unsigned char cell,
  unsigned char resValue,
  float         **deathProbs
  );
  
void disturbance(
  int           gridSize,
  unsigned char **grid,
  int           distInterval,
  int           distPatchNumber,
  int           distPatchSize
  );
  


// Function files:
#include "../dSFMT-src-2.2.2/dSFMT.c"                 // dSFMT random number generator
#include "../dSFMT-src-2.2.2/dSFMT.h"                 // dSFMT random number generator
#include "../functions/randomFloat.c" 
#include "../functions/readCfg.c" 
#include "../functions/iterate.c"                                                   
                                                      // function prototypes
//char* readCfg(char *cfgFile, char *arg);

/*void iterate(
  int           gridSize,
  unsigned char **gridIn,
  unsigned char **gridOut,
  unsigned char **resGrid,
  float         **colonizeProbs,
  float         **deathProbs,
  int           n_species,
  int           i,
  int           j);
*/




//float randomFloat();

void printArray(int gridSize, unsigned char **grid);

void saveArray1(int gridSize, unsigned char **gridIn, char *directory, char *prefix, int gen, int rep);

int main(int argc, char *argv[])
{
  FILE *configFile;
  FILE *fileField;
  FILE *fileRes;
  FILE *fileResEqn;
  FILE *fileDeathEqn;
  FILE *censusOut;
  
  char *cfgFile = argv[1];
  
  int i, j, k, t, r;
  
  int gridSize;                 // The length of one side of the square playing field
  int n_species;                // Number of species or species in the simulation
  int n_resource;               // Number of resource levels in the simulation
  int save;                     // Save the field grid periodically? (0/1)
  int saveInterval;             // Interval at which to save the field grid
  int print;                    // Print the playing field to the console window each time step? (0/1)
  int timeStart;                // What time step should the simulation start on?
  int timeMax;                  // Ending time step for the simulation
  int count;                    // Should a census of species be taken? (0/1)
  int countInterval;            // At what interval should the census occur? 
  int disturb;                  // Invoke the disturbance algorithm? (0/1)
  int distInterval;             // At what interval should the disturbances occur?
  int distPatchNumber;          // How many (square) disturbance patches should be created?
  int distPatchSize;            // How large should one side of the square disturbance patches be?
  int reps;                     // How many replicate simulations should be run?
  int messages;                 // Print program messages to the console window? (0/1)
  int resume;                   // Resume the simulation from a saved field? (0/1)
  int resumeStart;              // Which rep should the resume begin with?
  int resumeEnd;                // Which rep should the resume end with?
  
  int nExtinct = 0;            // 'nExtinct' keeps track of how many species have gone extinct. 

  
  char filePrefix1[100] = "m.field.in.g";       // The prefixes for the saved field files.
  char filePrefix2[100] = "m.field.out.g";
  
  char censusFile[LINE_LENGTH] = "";            // character array for census file name
  
  char fieldFileName[LINE_LENGTH];              // The path to the initial playing field file (new simulation)
  char resFileName[LINE_LENGTH];                // The path to the resource field file
  char resEqnFileName[LINE_LENGTH];             // The path to the growth equations file
  char directoryOut[LINE_LENGTH];               // The path in which to output save files
  char resumeFileName[LINE_LENGTH];             // The path to the initial playing field file (resumed simulation)

  float         **colonizeProbs;                // A 2D array to hold the growth probabilities
  float         **deathProbs;                   // A 2D array to hold the death probabilities
  unsigned char **resourceGrid;                 // A 2D array to hold the resource field
  unsigned char ***fieldGrid;                   // A 3D array to hold the playing field
  
  long int *speciesCounts;

  // read in paramaters from config file
  gridSize        = atoi(readCfg(cfgFile, "gridSize"));
  n_species       = atoi(readCfg(cfgFile, "n_species"));
  n_resource      = atoi(readCfg(cfgFile, "n_resource"));
  save            = atoi(readCfg(cfgFile, "save"));
  print           = atoi(readCfg(cfgFile, "print"));
  timeStart       = atoi(readCfg(cfgFile, "timeStart"));
  timeMax         = atoi(readCfg(cfgFile, "timeMax"));
  saveInterval    = atoi(readCfg(cfgFile, "saveInterval"));
  count           = atoi(readCfg(cfgFile, "count"));
  countInterval   = atoi(readCfg(cfgFile, "countInterval"));
  disturb         = atoi(readCfg(cfgFile, "disturb"));
  distInterval    = atoi(readCfg(cfgFile, "distInterval"));
  distPatchNumber = atoi(readCfg(cfgFile, "distPatchNumber"));
  distPatchSize   = atoi(readCfg(cfgFile, "distPatchSize"));
  reps            = atoi(readCfg(cfgFile, "reps"));
  messages        = atoi(readCfg(cfgFile, "messages"));
  resume          = atoi(readCfg(cfgFile, "resume"));  
  resumeStart     = atoi(readCfg(cfgFile, "resumeStart")); 
  resumeEnd       = atoi(readCfg(cfgFile, "resumeEnd")); 
       
  if(messages){                                                 // print out the parameters (if messages = 1):
    printf("gridSize                    = %d\n", gridSize);
    printf("number of species           = %d\n", n_species);
    printf("number of resource levels   = %d\n", n_resource);
    printf("number of repetitions       = %d\n", reps);
    printf("field grid filename         = %s\n", readCfg(cfgFile, "fieldFileName"));
    printf("resource grid filename      = %s\n", readCfg(cfgFile, "resFileName"));
    printf("resource equations filename = %s\n", readCfg(cfgFile, "resEqnFileName"));
    printf("death equations filename    = %s\n", readCfg(cfgFile, "deathEqnFileName"));
    if(count) 
      printf("count                       = yes\n"); 
      else printf("count                       = no\n");
    if(count) 
      printf("count interval              = %d\n", countInterval);
    if(disturb){
      printf("disturb                     = yes\n");
      printf("disturbance interval        = %d\n", distInterval);
      printf("disturbance patch number    = %d\n", distPatchNumber);
      printf("disturbance patch size      = %d\n", distPatchSize);}
      else printf("disturb                     = no\n");
    } // if(messages)
  
  
  colonizeProbs = calloc(n_species, sizeof(long int));         // Allocate memory for the growth and death probability arrays.
  deathProbs = calloc(n_species, sizeof(long int));
  
  for(i = 0; i < n_species + 1; i++){
    colonizeProbs[i] = calloc(n_resource + 1, sizeof(float));
    deathProbs[i] = calloc(n_resource + 1, sizeof(float));
    }
  
  resourceGrid = calloc(gridSize, ELEMENT_SIZE);        // Allocate memory for the reource and field grid arrays.
  fieldGrid    = calloc(3, ELEMENT_SIZE);
    
  for(i = 0; i < gridSize; i++)                         // Create the 2D resource grid.
    resourceGrid[i] = calloc(gridSize, ELEMENT_SIZE);  

  for(i = 0; i < 3; i++){                               // Create the 3D field grid.
    fieldGrid[i] = calloc(gridSize, ELEMENT_SIZE);
    for(j = 0; j < gridSize; j++)
      fieldGrid[i][j] = calloc(gridSize, ELEMENT_SIZE);
    } // for(i)

  speciesCounts = calloc(n_species + 1, sizeof(long int));      // Allocate memory for the species cell counter.
  
  fileResEqn = fopen(readCfg(cfgFile, "colonizeProbsFileName"), "r");     // Read in the colonization and death probabilities
  fileDeathEqn = fopen(readCfg(cfgFile, "deathProbsFileName"), "r");      // from the corresponding files.
  
  for(i = 0; i < n_species; i++){                                
    for(j = 0; j < n_resource; j++){
      if(messages)  printf(" species = %d, resource = %d,", i + 1, j + 1);  // Print the species and resource combination.      
      fscanf(fileResEqn, "%f", &colonizeProbs[i][j]);                       // Read the corresponding colonization probability.
      fscanf(fileDeathEqn, "%f", &deathProbs[i][j]);                        // Read the corresponding death probability.
      if(messages) printf(" colonization prob. = %0.3f", colonizeProbs[i][j]);    // Print the colonization probability.
      if(messages) printf(" death prob. = %0.3f", deathProbs[i][j]);        // Print the death probability.
      }
    printf("\n");
    }

 // for(i = 0; i < n_species; i++){                                
   // for(j = 0; j < n_resource; j++){
   //   if(messages)  printf(" species = %d, resource = %d,", i + 1, j + 1);  // Print the species and resource combination. 
   //   fscanf(fileDeathEqn, "%f", &deathProbs[i][j]);                        // Read the corresponding death probability.
   //   if(messages) printf(" death prob. = %0.3f", deathProbs[i][j]);        // Print the death probability.
   // }
   // printf("\n");
   // }

  
  unsigned char *neighborCounts;                            // Create a vector to hold the neighbor counts.
  neighborCounts = calloc(n_species, ELEMENT_SIZE);

  srand((unsigned)time(NULL));                              // Initialize the random number generator.
  randomFloat();

  mkdir(readCfg(cfgFile, "directoryOut"), 07770);           // Create the output directory.
  
  for(i = 0; i<LINE_LENGTH; i++)                            // Create the output directory string.
    directoryOut[i] = readCfg(cfgFile, "directoryOut")[i];
  
  if(messages)
    printf("Output directory ... %s\n", directoryOut);      // Print the output directory.
  
  if(!resume){
    resumeStart = 1;                                        // If not resuming a previous simulation, set the start time to t=1.
    resumeEnd = reps;
    }
  
  for(r = resumeStart; r <= resumeEnd; r++){                //  Loop through the reps of the simulation.

    if(resume){
      for(i = 0; i < LINE_LENGTH; i++)                       // Set the initial resume file field path string.
        resumeFileName[i] = readCfg(cfgFile, "directoryOut")[i];
      
      char timeResume[100];                                  // Create character vectors for the time step and rep number
      char repNumber[100];                                   // of the output files.
      
      sprintf(timeResume, "%d", timeStart - 1);              // Set 'timeResume' to the starting time step.
      sprintf(repNumber, "%d", r);                           // Set 'repNumber' to the current rep.

      strcat(resumeFileName, filePrefix2);                   //  Set the resume grid filename.
      strcat(resumeFileName, timeResume);
      strcat(resumeFileName, ".r");
      strcat(resumeFileName, repNumber);
      strcat(resumeFileName, ".txt");

      if(messages)
        printf("Restart file ... %s\n", resumeFileName);     // Print the resume start filename.
      
    } // if(resume)  
      
  
    char tempChar;                                  // A temporary character variable for reading the starting field
                                                             // and resource grids.
  
    if(resume) fileField = fopen(resumeFileName, "r");
    else fileField = fopen(readCfg(cfgFile, "fieldFileName"), "r");  // Open the resource and initial field files for reading.
    fileRes   = fopen(readCfg(cfgFile, "resFileName"), "r");
  
    if(!fopen(readCfg(cfgFile, "fieldFileName"), "r")) printf("can't find field file\n");
    for(i = 0; i < gridSize; i++){                              // Read in the resource and initial
      for(j = 0; j < gridSize; j++){                            // field grids.
        
        fscanf(fileField, "%s", &tempChar);
        fieldGrid[1][i][j] = fieldGrid[0][i][j] = atoi(&tempChar);
        fscanf(fileRes, "%s", &tempChar);
        resourceGrid[i][j] = atoi(&tempChar);
        }
    }  
    fclose(fileField);
    fclose(fileRes);

    
    
    if(count){                                                  // Write the census file.
    

    for(i = 0; i < LINE_LENGTH; i++)                            // Set the census filename.
      censusFile[i] = directoryOut[i];
      
    char censusPrefix[100] = "census.";
    char censusNum[100];
    sprintf(censusNum, "%d", r);
    strcat(censusFile, censusPrefix);
    strcat(censusFile, censusNum);
    strcat(censusFile, ".txt");
    
    if(timeStart == 0){
      censusOut = fopen(censusFile, "w");
      
      if(messages) printf("census file name:  %s\n", censusFile);  // Print the census filename.
    
      fprintf(censusOut, "%s", "Time\t");                      // Write the column headings
      for(i = 1; i < n_species + 1; i++){
        fprintf(censusOut, "%s", "Pop_");
        fprintf(censusOut, "%d\t", i);
        }

      fprintf(censusOut, "\n");
      fclose(censusOut);
      } // if(timeStart == 0)
      } // if(count)

    for(t = timeStart; t <= timeMax; t++){                     // main program loop:
      
      if(messages) printf("t = %d\n", t);                      // Print the time step.
      
      if(print) printArray(gridSize, fieldGrid[t%2]);          // Print gridIn.  'gridIn' is the first layer of the 3D field
                                                               // array if the time step is an even number.  'gridIn' is the
                                                               // second layer when the time step is odd.
      
      for(i = 1; i < gridSize - 1; i++)                        // Clear gridOut:
      for(j = 1; j < gridSize - 1; j++){  
        fieldGrid[!(t%2)][i][j] = 0;                           // If the time step is odd, 'fieldGridOut' is the second layer of 
                                                               // the 3D field array.  When the time step is even, 'gridOut' is 
                                                               // the first layer.
        }   
      
      //save the incoming field image
      if(save) if(t%saveInterval == 0) saveArray1(gridSize, fieldGrid[(t%2)], directoryOut, filePrefix1 , t, r);
      
      if(disturb){
        if(t%distInterval == 0) if(t != 0) disturbance(        // Call the habitat disturbance function.  The disturbance function
          gridSize,                                            // cannot be used in the first time step.
          fieldGrid[t%2],                                      // Disturb fieldGridIn
          distInterval,
          distPatchNumber,
          distPatchSize
          );
        }
    
      for(i = 1; i < gridSize - 1; i++)                        // Iterate the algorithm by one time step.
      for(j = 1; j < gridSize - 1; j++){
        iterate(                                    
          gridSize, 
          fieldGrid[t%2],                                      // grid in:  second layer if t is even, first layer if t is odd.
          fieldGrid[!(t%2)],                                   // grid out: second layer if t is odd, first layer if t is even. 
          resourceGrid,
          colonizeProbs,
          deathProbs,
          n_species, 
          i, 
          j);
        } // for(i and j)
      
                                                                // Save the outgoing field image to a file.
      if(save) if(t%saveInterval == 0) saveArray1(gridSize, fieldGrid[(!t%2)], directoryOut, filePrefix2 , t, r);

      if(count){                                  // Create the census:

        for(i = 0; i < n_species + 1; i ++){
          speciesCounts[i]=0;                     // Reset the species counter.
          }
      
        for(i = 1; i < gridSize - 1; i++)
        for(j = 1; j < gridSize - 1; j++){
          fieldGrid[2][i][j] = 0;                  // clear the tally layer
          speciesCounts[fieldGrid[t%2][i][j]]++;   // tally species populations
          }
  
        // print the results of the cell census
        if(messages) printf("\ncell census results:\n");
        for(i = 1; i < n_species + 1; i++)
        if(messages)  printf("species %d population: %ld\n", i, speciesCounts[i]);
        if(messages) printf("\n");    




        // write the census results to a file
        char censusPrefix[100] = "census.";
        char censusNum[100];
        sprintf(censusNum, "%d", r);
        censusOut = fopen(censusFile, "a+");
        fprintf(censusOut, "%d\t", t);
      
        for(i = 1; i < n_species + 1; i++)
          fprintf(censusOut, "%ld\t", speciesCounts[i]);
          
        fprintf(censusOut, "\n");
        fclose(censusOut);
        
        // Calculate the number of extinct species:
        nExtinct = 0;                                // Initialize to 0.
      
        for(i = 0; i < n_species + 1; i++)           // Loop through the different species.
          if(speciesCounts[i] == 0) nExtinct++;      // Increment 'nExtinct' if the census for a given species is 0.
        
        printf("number extinct = %d\n", nExtinct);   // Print the number extinct.
      
        if(nExtinct >= n_species - 1) break;         // End the program if all but one species have gone extinct.
      
        } //if(count)    
    } // for(t)
  }  // for(r)
 
  /*
  free(colonizeProbs);
  free(deathProbs);
  free(resourceGrid);
  free(fieldGrid);
  free(speciesCounts);
  free(neighborCounts);
  */
} // main


/*-------------------------------------------------------------------------------
READ PARAMETER VALUES.

Parameters for a given run of the program are stored in a text file of two columns. 
The first column contains parameter names, the second column containss the corresponding
values.  The first column must not be indented. Columns are separated by white space
(one or more tabs or spaces). 

Here is an excerpt of a sample file:

    gridSize	  10
    n_resource	 1
    n_species	   2

This function parses the configuration file and returns the value of specified parameter.

ENTRY:  'cfgFile' points to the configuration file.
        'arg' points the name of the parameter to retrieve.

EXIT:   'readCfg()' returns the value of the parameter specified on entry.

        


char* readCfg(char *cfgFile, char *arg)
{
  int i = 0, j = 0;                              // Initialize loop variables.
  int argLength = strlen(arg);                   // Get the number of characters in the parameter name.
  char line1[LINE_LENGTH] = {0};                 // Create a string to hold the text in a line of the config file.
  char paramName[LINE_LENGTH] = {0};             // Create a string to hold the name of the parameter in 'line1'.
  static char paramValue[LINE_LENGTH] = {0};     // I get a compiler error if I don't make this static!
  
  FILE *configFile;

  configFile = fopen(cfgFile, "r");              // Open the configuration file for reading.

  for(i = 0; i < FILE_LENGTH; i++){              // Loop through each line of cfgFile:
    fgets(line1, LINE_LENGTH, configFile);       // Read line i, store it in 'line1'.
    strcpy(paramName, line1);                    // Copy 'line1' to 'paramName'.
    for(j = 0; j < LINE_LENGTH; j++)             // Loop through the characters of 'paramName'.
      if(paramName[j] <= ' ') paramName[j] = '\0';  // Insert a string-ending character at the first white space
    if(!strcmp(paramName, arg)) break;           // If 'paramName' and 'arg' match, stop the loop.
    } //for(i)

  fclose(configFile);                            // Close cfgFile.
    
  for(i = argLength; line1[i] <= ' '; i++);      // Bypass the white space between columns 1 and 2.
  
  j = 0;                                         // Reset j.
  
  for(i = i; i < LINE_LENGTH; i++){              // Loop through the second column of 'line1'.
    paramValue[j] = line1[i];                    // Copy the parameter value into 'paramValue'.
    if(paramValue[j] <= ' '|paramValue[j] <= '\n'){   // If whitespace, or the line-ending character is reached:
      paramValue[j] = '\0';                      // Substitute a string-ending character for the end-of-line character
      break;                                     // and end the loop.
      }
    j++;                                         // Incrememt j.
    } //for(i)
  
  return(paramValue);                            // Return the value of the parameter.

} // readCfg()

*/

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


void iterate(
  int           gridSize,
  unsigned char **gridIn,
  unsigned char **gridOut,
  unsigned char **resGrid,
  float         **colonizeProbs,
  float         **deathProbs,
  int           n_species,
  int           i,
  int           j)
{

  int k, l, n, t;                                     // Create variables for loops.
  int n_counter = 0;                                  // Create the total neighbor counter and initialize it to zero.
  unsigned char neighbors[n_species];                 // Create a 1-D array to hold the counts of neighbors of each species.
  
  for(k = 0; k < n_species; k++)                      // Initialize the species-specific neighbor counter to 0.
    neighbors[k] = 0;
                                  
  for(k = i - 1; k <= i + 1; k++){                    // Loop through the 3X3 neighbor window.
  for(l = j - 1; l <= j + 1; l++){  
     if(gridIn[k][l]){                                // If neighboring cell gridIn[k][l] is occupied:
       neighbors[gridIn[k][l] - 1]++;                 // Increment the corresponding element in the species-specific neighbor counter.
       n_counter++;                                   // Increment the total neighbor counter.
     }//if(gridIn[k][l]
   }}// for k and l

  if(gridIn[i][j]){                                   // If the focal cell is occupied:
    neighbors[gridIn[i][j] - 1]--;                    // Subtract the focal cell species from the species-specific neighbor count.
    n_counter--;                                      // Subtract the focal cell from the total neighbor count.
    gridOut[i][j] = compete(gridIn[i][j], neighbors); // Invoke the competition function. 
    }//if(gridIn[i][j])
  
  if(n_counter&&!gridIn[i][j]) {                      // colonize if unoccupied and there are any neighbors
    gridOut[i][j] = colonize(resGrid[i][j], neighbors, n_counter, n_species, colonizeProbs);
    }

  if(gridOut[i][j]>0){                                 // if the focal cell is occupied, invoke the death probability function.
    gridOut[i][j] = deathPanel(gridOut[i][j], resGrid[i][j], deathProbs);
    }

}// iterate()

*/

/*------------------------------------------------------------------------------
COMPETITION BETWEEN AN OCCUPIED CELL AND ITS NEIGHBORS

This function returns the result of a competition between an already occupied
cell and its neighbors.

ENTRY:    1.  'cell' is the identity of the species in the cell.
          2.  'neighbors' is the vector of neighbor counts.
          
EXIT:     1.  The result of the competition (i.e. the ID of the species in the focal cell
              for the next time step)
*/

int compete(
  unsigned char cell,
  unsigned char *neighbors
  ){
  return cell;                                        //  stub 
} // compete


/*------------------------------------------------------------------------------ +-
PROBABILISTIC DEATH OF AN OCCUPIED CELL

This function fetches the likelihood of death for a given species in a given environment from the corresponding element in
'deathProbs' and performs a Bernoulli trial to determine if the inhabitant of the focal cell survives to the next time step.

ENTRY:      1.  'cell' is the species value in the cell before the death trial.  'cell' is numbered from 1 to 'n_species'.
            2.  'resValue' is the value of the resource in the cell.
            3.  Each row of the 'deathProbs' matrix corresponds to the identity of 
                a species (beginning with 1), while columns represent the possible values of the resource (beginning with 0).  
                Each entry is the probability that the given species occupying the 
                focal cell will die in a time step given the corresponding resource level.
                                
EXIT:       1.  'deathPanel' returns the species value for the cell after the death trial. Possible values are 'cell' or 0. 

FUNCTIONS:  1.  randomFloat()
*/

int deathPanel(
  unsigned char cell,
  unsigned char resValue,
  float         **deathProbs
  )
{
  if(randomFloat() < deathProbs[cell-1][resValue])    // If a randomly drawn float (between 0 and 1) is less than
                                                      // the value of the corresponding death probability
                                                      // then the cell is cleared.
    return(0);
  else
    return(cell);                                     // Otherwise the value of the cell is returned.
}  

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

FUNCTIONS:  1.  randomFloat()
*/
int colonize(
  unsigned char resValue,
  unsigned char *neighbor_count,
  unsigned char neighbor_total,
  int           n_species,
  float         **colonizeProbs
  )
{ 
  int i, j, x;                                   // Variables for loops and counters
  float p_none = 1;                              // Variable to hold Pr(no colonization)
  float genoColonize[neighbor_total + 1][3];     // Create a 3 by neighbor_total + 1 array to hold species id,
                                                 // colonization likelihoods, and cumulative probs.
                                                 // row 1 = species ID
                                                 // row 2 = colonization probability
                                                 // row 3 = cumulative prob

  float p_total = 0;                             // A float that stores the sum of all probabilities of colonization
  float rand_float = randomFloat();              // A random float for determining which species to output
  int returnGenotypeIndex = 0;                   // The index of the first row 'genoColonize' to output 
  genoColonize[0][0] = 0;                        // The first element in genoColonize is the null species ID.
                                                 // This species is returned if there is no colonization.
  
  // Set the species ID and corresponding colonization likelihood in the genoColonize array:
  x = 1;                                         // Reset the counter.  Counter starts at 1 (the second index)
                                                 // because the first element is already set to the null species.
            
  for(i = 0; i < n_species; i++){                // Loop through each species.
    for(j = 0; j < neighbor_count[i]; j++){      // Loop once for each individual of species i
      genoColonize[x][0] = i + 1;                // Set the species ID column (species ID starts at 1).
      genoColonize[x][1] = colonizeProbs[i][resValue]; // Get the colonization likelihood for species i
                                                 // from the colonizeProbs table.                                                   
      x++;                                       // Increment the counter.
      p_none *= 1-colonizeProbs[i][resValue];    // Update the null colonization probability.
      p_total += colonizeProbs[i][resValue];     // Update the total probability of colonization (can be > 1).
      }                                          // This is used to normalize the probabilities later.
    } // for(i)
  p_total += p_none;                             // Add the probability of no colonization to the total prob.
  
  // Set the likelihood that no species colonizes the focal cell.
  genoColonize[0][1] = p_none;                   // Set the null colonization likelihood.
  genoColonize[0][2] = p_none;                   // Set the null colonization cumulative probability (same as
                                                 // the null colonization likelihood).

  
  // Set the cumulative probabilities for each neighbor in 'genoColonize'.
  for(i = 1; i < neighbor_total + 1; i++){       // Loop through the rest of the neighbors.
    genoColonize[i][2] =                         // Calculate the cumulative likehihood for each neighbor:
      genoColonize[i-1][2] +                     // = previous species's cumulative likelihood + current
      genoColonize[i][1] *                       // species's total likelihood * scaled total colonization 
      (1 - p_none) / p_total;                    // likelihood.
    } // for(i)

  // Determine which neighbor will colonize (including the possiblilty of the null species).
  for(i = 0; i < neighbor_total + 1; i++){       // Loop through the cumulative probability of all neighbors 
      if(rand_float <= genoColonize[i][2]){      // (including the null) to determine which neighbor will colonize.
        returnGenotypeIndex = i;                 // The index of the winning species
        break;                                   // Break once the winner is found.
      }
    }

  return(genoColonize[returnGenotypeIndex][0]);  // Return the winning species. 

} // colonize


/*------------------------------------------------------------------------------- +-
CREATE RANDOM DISTURBANCE PATTERNS IN THE FIELD GRID

This function creates randomly placed square disturbances in the field grid of specified size (distPatchSize).
The disturbances consist of removing all individual species within the disturbed patch.


ENTRY:  1.  The size of the grid:  'gridSize'
        2.  The field grid:  'gridIn'
        3.  The disturbance interval: 'distInterval'
        4.  The number of disturbance patches to create: 'distPatchNumber'
        5.  The size of the disturbed patches:  'distPatchSize'
        
EXIT:   1.  The field grid with the randomly generated disturbances: 'gridIn'
*/

void disturbance(
  int           gridSize,
  unsigned char **gridIn,
  int           distInterval,
  int           distPatchNumber,
  int           distPatchSize
  ){
  
  int i, j, p, l;
  int x[distPatchNumber];                   // Vector of x-coordinates of the disturbance patches
  int y[distPatchNumber];                   // Vector of y-coordinates of the disturbance patches
  
  // generate the random coordinates for the disturbance patches:
  for(i = 0; i < distPatchNumber; i++){
    x[i] = rand()%gridSize;                 // Generate the x-coordinates.  
    // generate a float between 0 and 1, multiply by 1000
    y[i] = rand()%gridSize;                 // Generate the y-coordinates.
  }
  
  // create the disturbance patches:
  for(p = 0; p < distPatchNumber; p++){     // Loop through each disturbance patch.
    for(i = x[p] - distPatchSize/2 + 1 ; i < x[p] + distPatchSize/2 - 1; i++)   // Go from half the disturbance patch size to the left
    // why the plus and minus 1?                                                                           // to half the patch size to the right.
    for(j = y[p] - distPatchSize/2 + 1; j < y[p] + distPatchSize/2 - 1; j++){  // Go from half the disturbance patch size up to
                                                                               // half the disturbance patch size down.
      if(i >= 0 &&j >=0 && i<gridSize && j <gridSize) // If the coordinate for the focal cell is within the field boundaries:
        gridIn[i][j] = 0;                            // Clear the cell.
      }
    } // for(p)
}



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


/*-------------------------------------------------------------------------------
SAVE THE FIELD GRID TO A FILE.

This function saves the field array to a file.

ENTRY:  A 2-D array, 'gridIn'

EXIT:   'gridIn' is saved to a text file
*/

void saveArray1(int gridSize, unsigned char **gridIn, char *directory, char *prefix, int gen, int rep)
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

  for(i = 0; i < gridSize; i++){        // Loop through the x and y coordinates of the field.
    for(j = 0; j < gridSize; j++){
      fprintf(outFile, "%d ", gridIn[i][j]);  // Save the genotype in the corresponding cell, separated by a space.
      }
    fprintf(outFile, "\n");             // Save a newline character at the end of each row.
    }   
     
  fclose(outFile);                     // Close the file.
  }
     

/*------------------------------------------------------------------------------- +-
This function generates a random float between 0 and 1

ENTRY:   1. The random number generator has been initialized (in main loop).

EXIT:    1. 'randomFloat' returns a uniformly distributed random number greater or
            equal to 0 and less than 1.


float randomFloat()
{
      return (float)rand()/(float)(RAND_MAX);    // Divide a randomly chosen integer by the maximum
                                                     // integer size.      
                                                     
                                                     // consider switching to double
                                                     
                                                     
}
*/