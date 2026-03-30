/* ============================================================================
Description of program:

This cellular automation program models the spatial growth of one or more species, each having different 
growth characteristics in different environments.

A square playing field (fieldGrid) is used for habitat, each cell of which can be unoccupied or occupied
by one individual of one of the species.  The environments are specified by another square 
grid (habitatGrid).  

Growth is inspired by the vegetative growth of a clonal plant; each unoccupied cell adjacent to an occupied
cell has a probability of becoming occupied in the subsequent time step.  Growth parameters are specified 
for each species in each environment by the habitat probability file (fileHabitatEqn).

Each occupied cell has a probability of becoming empty in the subsequent time step according to the values
specified for each species/environment combination in the death probability file (fileDeathEqn).

Periodic habitat disruption is implemented by clearing a specified number (YPatchNumber) of randomly
positioned squares of a specified size (distPatchSize) of all individuals at specified intervals (distInterval).

A census of each species is taken at each time step and output to a census file (censusOut).

A snapshot of the playing field can be saved so that simulations can be re-started later.




Description of updates:

April 2013:  Changed 'genotype' to 'species'.  This change does not affect the how the program
             runs in any way, but using the word species makes more intuitive sense.

April 2013:  Added code to make program stop if all but one of the species goes extinct.

September 2013:  Moved functions to individual source files to un clutter the main source file.

September 2013:  Changed random number generator function to dSFMT.  This should provide better random
                 numbers without adding much time to the routine.

September 2013:  Created an option for wrapping boundaries (boundaryType = 1).

September 2013:  Added a shape option for the disturbance function.  (distShape:  1 = square, 2 = round)

September 2013:  Added disturbance losses columns for each species to the census file.

October 2013:  Added a disturbance delay parameter (the first disturbance happens after this time step).

October 2013:  Added a displacement function.  This function mediates competition for cells that
               are already occupied.

October 2013:  Flattened colonizeProbs and deathProbs to 1D arrays.

October 2013:  Allowed rectangular grids.
 
October 2013:  Updated census algorithm to count individuals in each habitat separately.

October 2013:  Added parameter 'extinctThreshold'.

October 2013:  Added option of stochastic disturbance.

===============================================================================*/


#define ELEMENT_SIZE 8                                // size of elements in the field arrays
#define LINE_LENGTH 1000                              // maximum line length for an input file 
#define FILE_LENGTH 256                               // maximum number of lines in the configuration file
#define DSFMT_MEXP 19937                              // period length of the dSFMT random number generator

#include <stdio.h>
#include <time.h>                                     // to generate a random seed
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Function files:
#include "../dSFMT-src-2.2.2/dSFMT.c"                 // dSFMT random number generator
#include "../dSFMT-src-2.2.2/dSFMT.h"                 // dSFMT random number generator
dsfmt_t dsfmt;                                        // Required for dSFMT.
// #include "../functions/randomFloat.c"              // Random number generator using rand().  
                                                      // Uncomment in disturb.c, colonize.c, deathPanel.c to use.
#include "../functions.3.4/readCfg.c" 
#include "../functions.3.4/colonize.c"
#include "../functions.3.4/displace.c"
#include "../functions.3.4/deathPanel.c"
#include "../functions.3.4/disturb.c"
#include "../functions.3.4/disturbR.c"
#include "../functions.3.4/printArray.c"
#include "../functions.3.4/saveArray.c"
#include "../functions.3.4/iterate.w.c" 
#include "../functions.3.4/Bernoulli.c" 
#include "../functions.3.4/iterate.c"


int main(int argc, char *argv[])
{
  FILE *configFile;
  FILE *fileField;
  FILE *fileHabitat;
  FILE *fileHabitatEqn;
  FILE *fileDeathEqn;
  FILE *fileDeathVector;
  FILE *fileColonizeVector;
  FILE *fileDisplaceMatrix;
  FILE *censusOutSp;
  FILE *censusOutDist;
  
  char *cfgFile = argv[1];
  
  int i, j, k, n, day, r, m;
  
  // Variables for paramemters set in the cfg file:
  int gridSizeX;                // The number of rows in the playing field
  int gridSizeY;                // The number of columns in the playing field
  int n_species;                // Number of species or species in the simulation
  int n_habitat;                // Number of habitat levels in the simulation
  int save;                     // Save the field grid periodically? (0/1)
  int saveInterval;             // Interval at which to save the field grid
  int saveDelay;                // Number of time steps to wait before the first save.
  int print;                    // Print the playing field to the console window each time step? (0/1)
  int timeStart;                // What time step should the simulation start on?
  int timeMax;                  // Ending time step for the simulation
  int count;                    // Should a census of species be taken? (0/1)
  int countInterval;            // At what interval should the census occur? 
  int disturbYN;                // Invoke the disturbance algorithm? (0/1)
  int disturbModel;             // What is the disturbance model? (0 = regular interval, 1 = random disturbance interval/number)
  int disturbRateR;             // What is the mean number of disturbance patches per time step? (random model, 1/p for a geometric distribution)
  float disturbProbR;           // What is the probability of a disturbance event in a given time step?  (random model)
  float disturbScaleR;            // What is the scaling factor for number of disturbances? (random model, multiplied by the geometric RV to determine total number of patches)
  int distShape;                // What shape for the disturbance patches? (1 = square, 2 = circular)
  int distInterval;             // At what interval should the disturbances occur? (regular interval model)
  int distPatchNumber;          // How many disturbance patches should be created per event? (regular interval model) 
  int distPatchSize;            // How large should one side of the square disturbance patches be?
  int distDelay;                // How long should the simulation wait until the first disturbance event?
  int reps;                     // How many replicate simulations should be run?
  int messages;                 // Print program messages to the console window? (0/1)
  int resume;                   // Resume the simulation from a saved field? (0/1)
  int resumeStart;              // Which rep should the resume begin with?
  int resumeEnd;                // Which rep should the resume end with?
  int stopExtinct;              // Should the simulation stop if all but one species go extinct?
  int extinctThreshold;         // Number below which to consider a species extinct.
  int boundaryType;             // What kind of boundaries?  0 = absorbing, 1 = wrapping.
  int displaceYN;               // Should the displace algorithm be employed (slows down the process)?
  unsigned int seed;            // What should the random seed be?
  
  
  char filePrefix1[100] = "m.field.in.g";       // The prefixes for the saved field files.
  char filePrefix2[100] = "m.field.out.g";
  
  char censusFileSp[LINE_LENGTH] = "";          // character array for species census file name
  char censusFileDist[LINE_LENGTH] = "";        // character array for disturbance census file name
  
  char fieldFileName[LINE_LENGTH];              // The path to the initial playing field file (new simulation)
  char habitatFileName[LINE_LENGTH];                // The path to the habitat field file
  char directoryOut[LINE_LENGTH];               // The path in which to output save files
  char resumeFileName[LINE_LENGTH];             // The path to the initial playing field file (resumed simulation)

  float         **colonizeProbs;                // A 2D array to hold the growth probabilities
  float         **deathProbs;                   // A 2D array to hold the death probabilities
  float         *colonizeVector;
  float         *deathVector;
  float         *displaceVector1;               // A 1D array to hold the displacement probabilities
  float         *displaceVector2;
  unsigned char **habitatGrid;                  // A 2D array to hold the habitat field
  unsigned char ***fieldGrid;                   // A 3D array to hold the playing field
  long int      **speciesCounts;                // A 2D array to hold the census of each species in each habitat
  int           *nExtinct = 0;                  // 'nExtinct' keeps track of how many species have gone extinct in each habitat. 



  // read in paramaters from config file
  gridSizeX       = atoi(readCfg(cfgFile, "gridSizeX"));
  gridSizeY       = atoi(readCfg(cfgFile, "gridSizeY"));
  n_species       = atoi(readCfg(cfgFile, "n_species"));
  n_habitat       = atoi(readCfg(cfgFile, "n_habitat"));
  save            = atoi(readCfg(cfgFile, "save"));
  print           = atoi(readCfg(cfgFile, "print"));
  timeStart       = atoi(readCfg(cfgFile, "timeStart"));
  timeMax         = atoi(readCfg(cfgFile, "timeMax"));
  saveInterval    = atoi(readCfg(cfgFile, "saveInterval"));
  saveDelay       = atoi(readCfg(cfgFile, "saveDelay"));
  count           = atoi(readCfg(cfgFile, "count"));
  countInterval   = atoi(readCfg(cfgFile, "countInterval"));
  disturbYN       = atoi(readCfg(cfgFile, "disturbYN"));
  disturbModel    = atoi(readCfg(cfgFile, "disturbModel"));
  distInterval    = atoi(readCfg(cfgFile, "distInterval"));
  distPatchNumber = atoi(readCfg(cfgFile, "distPatchNumber"));
  distPatchSize   = atoi(readCfg(cfgFile, "distPatchSize"));
  disturbRateR    = atoi(readCfg(cfgFile, "disturbRateR"));
  disturbProbR    = atof(readCfg(cfgFile, "disturbProbR"));
  disturbScaleR   = atof(readCfg(cfgFile, "disturbScaleR"));
  distShape       = atoi(readCfg(cfgFile, "distShape"));
  distDelay       = atoi(readCfg(cfgFile, "distDelay"));
  reps            = atoi(readCfg(cfgFile, "reps"));
  messages        = atoi(readCfg(cfgFile, "messages"));
  resume          = atoi(readCfg(cfgFile, "resume"));  
  resumeStart     = atoi(readCfg(cfgFile, "resumeStart")); 
  resumeEnd       = atoi(readCfg(cfgFile, "resumeEnd")); 
  seed            = atoi(readCfg(cfgFile, "seed"));
  stopExtinct     = atoi(readCfg(cfgFile, "stopExtinct"));
  extinctThreshold= atoi(readCfg(cfgFile, "extinctThreshold"));
  boundaryType    = atoi(readCfg(cfgFile, "boundaryType"));
  displaceYN      = atoi(readCfg(cfgFile, "displaceYN"));
  
  int distCensusAll = 0;                        // The number of disturbed cells (occupied or not) from disturb()
  long int *distCensusKilled;                    // Array to hold the number and species of cells killed by disturb()

       
  if(messages){                                                 // print out the parameters (if messages = 1):
    printf("number of rows               = %d\n", gridSizeX);
    printf("number of columns            = %d\n", gridSizeY);
    printf("number of species            = %d\n", n_species);
    printf("number of habitat levels     = %d\n", n_habitat);
    printf("number of repetitions        = %d\n", reps);
    printf("field grid filename          = %s\n", readCfg(cfgFile, "fieldFileName"));
    printf("habitat grid filename        = %s\n", readCfg(cfgFile, "habitatFileName"));
    printf("colonization vector filename = %s\n", readCfg(cfgFile, "colonizeVectorFile"));
    printf("death vector filename        = %s\n", readCfg(cfgFile, "deathVectorFile"));
    printf("displace matrix filename     = %s\n", readCfg(cfgFile, "displaceMatrixFile"));
    if(count) 
      printf("count                       = yes\n"); 
      else printf("count                       = no\n");
    if(count) 
      printf("count interval              = %d\n", countInterval);
    if(disturbYN){
      printf("disturb                     = yes\n");
      if(!disturbModel){
        printf("disturbance Model           = regular interval\n");
        printf("disturbance interval        = %d\n", distInterval);
        printf("disturbance patch number    = %d\n", distPatchNumber);
        }
      if(disturbModel){
        printf("disturbance Model           = random\n");
        printf("mean disturbances/event     = %d\n", disturbRateR);
        printf("probability of event/day    = %f\n", disturbProbR);
        }
      printf("disturbance patch size      = %d\n", distPatchSize);
      printf("disturbance shape           = %d\n", distShape);}
      else printf("disturb                     = no\n");
    } // if(messages)
  
  colonizeProbs = calloc(n_species, sizeof(long int));         // Allocate memory for the growth and death probability arrays.
  deathProbs = calloc(n_species, sizeof(long int));
  distCensusKilled = calloc(n_species, sizeof(long int));
  displaceVector1 = calloc(n_species * n_species * n_habitat, sizeof(float));   // unused, but program won't run correctly without this.
  displaceVector2 = calloc(n_species * n_species * n_habitat, sizeof(float));
  deathVector = calloc(n_species * n_habitat, sizeof(float));
  colonizeVector = calloc(n_species * n_habitat, sizeof(float));
  
  
  for(i = 0; i < n_species; i++){
    colonizeProbs[i] = calloc(n_habitat + 1, sizeof(float));
    deathProbs[i]    = calloc(n_habitat + 1, sizeof(float));
    }
    
  habitatGrid  = calloc(gridSizeX, ELEMENT_SIZE);        // Allocate memory for the reource and field grid arrays.
  fieldGrid    = calloc(2, ELEMENT_SIZE);
    
  
  for(i = 0; i < 3; i++){                               // Create the 3D field grid and 2D habitat grid.
    fieldGrid[i] = calloc(gridSizeX, ELEMENT_SIZE);
    for(j = 0; j < gridSizeX; j++){
      fieldGrid[i][j] = calloc(gridSizeY, ELEMENT_SIZE);
      }
    } // for(i)

  for(i = 0; i < gridSizeX; i++)
    habitatGrid[i] = calloc(gridSizeY, ELEMENT_SIZE);
          
  //speciesCounts = calloc((n_species + 1) * n_habitat, sizeof(long int));      // Allocate memory for the species cell counter.
  speciesCounts = calloc((n_species + 1), sizeof(long int));      // Allocate memory for the species cell counter.
   for(i = 0; i < n_species + 1; i++)
    speciesCounts[i] = calloc(n_habitat + 1, sizeof(long int));

  nExtinct = calloc(n_habitat, sizeof(int));
  
  //fileDisplaceMatrix = fopen(readCfg(cfgFile, "displaceMatrixFile"), "r");       // Read the displacement probabilities.
  fileColonizeVector = fopen(readCfg(cfgFile, "colonizeVectorFile"), "r");
  fileDeathVector = fopen(readCfg(cfgFile, "deathVectorFile"), "r");
  

  for(i = 0; i < n_species * n_habitat; i++)                 // Read in the colonization and death probabilities
    fscanf(fileDeathVector, "%f", &deathVector[i]);
  
  for(i = 0; i < n_species * n_species * n_habitat; i++)
    fscanf(fileColonizeVector, "%f", &colonizeVector[i]);
  
  
  
  for(i = 0; i < n_habitat; i++)
  for(j = 0; j < n_species; j++){
    if(messages) printf("habitat %d: species %d colonize %f, death %f\n", i, j, colonizeVector[i * n_species + j], deathVector[i * n_species + j]);
      
    }
  
  fileDisplaceMatrix = fopen(readCfg(cfgFile, "displaceMatrixFile"), "r");
  
  for(i = 0; i < n_species * n_species * n_habitat; i++){               // Read the displacement probabilities and flatten them to a 1D array.
    fscanf(fileDisplaceMatrix, "%f", &displaceVector2[i]);
    }
  
  fclose(fileDisplaceMatrix);
  
  
  if(messages)                                              // Print the displacement probabilities.
    for(i = 0; i < n_species * n_habitat; i++)
      for(j = 0; j < n_species; j++)
        printf("habitat %d:  species %d on species %d = %f\n", (i / n_species), i%n_species + 1, j + 1, displaceVector2[n_species * i + j]);
  
  
  unsigned char *neighborCounts;                            // Create a vector to hold the neighbor counts.
  neighborCounts = calloc(n_species, sizeof(char));

  srand((unsigned)time(NULL));                              // Initialize the random number generator for the dSFMT seed.
  if(seed == 0) seed = rand();                              // If seed = 0 in the cfg file, choose a random seed.
  dsfmt_init_gen_rand(&dsfmt, seed);                        // Initialize dSFMT.

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
                                                             // and habitat grids.
    if(resume) fileField = fopen(resumeFileName, "r");
    else fileField = fopen(readCfg(cfgFile, "fieldFileName"), "r");  // Open the habitat and initial field files for reading.
    fileHabitat    = fopen(readCfg(cfgFile, "habitatFileName"), "r");
  
    if(!fopen(readCfg(cfgFile, "fieldFileName"), "r")) printf("can't find field file\n");
    if(!fopen(readCfg(cfgFile, "habitatFileName"), "r")) printf("can't find habitat file\n");
    for(i = 0; i < gridSizeX; i++){                              // Read in the habitat and initial
      for(j = 0; j < gridSizeY; j++){                            // field grids.
        fscanf(fileField, "%s", &tempChar);
        fieldGrid[1][i][j] = fieldGrid[0][i][j] = atoi(&tempChar);
        fscanf(fileHabitat, "%s", &tempChar);
        habitatGrid[i][j] = atoi(&tempChar);
        }
    }  
    fclose(fileField);
    fclose(fileHabitat);

    if(count){                                                  // Write the census file.    
      for(i = 0; i < LINE_LENGTH; i++)                            // Set the census filename.
        censusFileSp[i] = directoryOut[i];
      
      char censusPrefix[100] = "census.sp.";
      char censusNum[100];
      sprintf(censusNum, "%d", r);
      strcat(censusFileSp, censusPrefix);
      strcat(censusFileSp, censusNum);
      strcat(censusFileSp, ".txt");
      
      if(disturbYN){
        for(i = 0; i < LINE_LENGTH; i++)                            // Set the census filename.
          censusFileDist[i] = directoryOut[i];
        
        char censusPrefix[100] = "census.dist.";
        char censusNum[100];
        sprintf(censusNum, "%d", r);
        strcat(censusFileDist, censusPrefix);
        strcat(censusFileDist, censusNum);
        strcat(censusFileDist, ".txt");        
        }
    
      if(timeStart == 0){
        censusOutSp = fopen(censusFileSp, "w");
        fprintf(censusOutSp, "Time\t");  // Write the column headings        
        if(messages) printf("census file name:  %s\n", censusFileSp);  // Print the census filename.
    
        // Write the column headings
        for(i = 0; i <= n_species; i++)
          for(j = 0; j < n_habitat; j++){
          fprintf(censusOutSp, "%s", "sp_");
          fprintf(censusOutSp, "%d", i);
          fprintf(censusOutSp, "%s", "_h_");
          fprintf(censusOutSp, "%d\t", j + 1);
          } // for(i)

        fprintf(censusOutSp, "\n");
        fclose(censusOutSp);
        
        
        
        if(disturbYN){
          censusOutDist = fopen(censusFileDist, "w");
          fprintf(censusOutDist, "Time\t");  // Write the column headings
          
          for(i = 1; i <= n_species; i++)
            for(j = 0; j < n_habitat; j++){
              fprintf(censusOutDist, "%s", "sp_");
              fprintf(censusOutDist, "%d", i);
              fprintf(censusOutDist, "%s", "_h_");
              fprintf(censusOutDist, "%d\t", j + 1);
              } // for(j)
          fprintf(censusOutDist, "%s", "total.dist");
          fprintf(censusOutDist, "\n");
          fclose(censusOutDist);          
          
          } //if(disturbYN)
        
        
        
        } // if(timeStart == 0)
      } // if(count)

    for(day = timeStart; day <= timeMax; day++){                     // main program loop:
      
      if(messages) printf("t = %d\n", day);                      // Print the time step.
      
      if(print) printArray(gridSizeX, gridSizeY, fieldGrid[day%2]);          // Print gridIn.  'gridIn' is the first layer of the 3D field
                                                               // array if the time step is an even number.  'gridIn' is the
                                                               // second layer when the time step is odd.
      
      //save the incoming field image
      if(save) if((day%saveInterval == 0) & (day >= saveDelay) & !disturbModel)
        saveArray(gridSizeX, gridSizeY, fieldGrid[!(day%2)], directoryOut, filePrefix1 , day, r);
      
      if(count){                                  // Create the census:
        //for(i = 0; i <= (n_species + 1) * n_habitat; i ++)
        for(i = 0; i <= n_species; i++)
          for(j = 0; j < n_habitat; j++){
            //printf("i = %d, j = %d, speciesCounts[%d][%d] = %ld\n", i, j, i, j, speciesCounts[i][j]);
            //speciesCounts[i][j] = 0;                     // Reset the species counter.
            }
        
        if(boundaryType)
          for(i = 0; i < gridSizeX; i++)                        // Iterate the algorithm by one time step (.
          for(j = 0; j < gridSizeY; j++){
            fieldGrid[!(day%2)][i][j] = 0;
            iterate_w(                                    
              gridSizeX,
              gridSizeY, 
              fieldGrid[(day%2)],                                      // grid in:  second layer if t is even, first layer if t is odd.
              fieldGrid[!(day%2)],                                   // grid out: second layer if t is odd, first layer if t is even.
              habitatGrid,
              colonizeVector,
              displaceYN,
              displaceVector2,
              deathVector,
              n_species,
              n_habitat,
              i, 
              j);
            //speciesCounts[fieldGrid[!(day%2)][i][j] * n_species + habitatGrid[i][j]]++;   // tally species populations
            speciesCounts[fieldGrid[!(day%2)][i][j]][habitatGrid[i][j]]++;   // tally species populations
            }// for(i and j)
        
       
        else // if(boundaryType)
          for(i = 1; i < gridSizeX-1; i++)                        // Iterate the algorithm by one time step.
          for(j = 1; j < gridSizeY-1; j++){
            fieldGrid[!(day%2)][i][j] = 0;
            iterate(                                    
              fieldGrid[(day%2)],                                      // grid in:  second layer if t is even, first layer if t is odd.
              fieldGrid[!(day%2)],                                   // grid out: second layer if t is odd, first layer if t is even.
              habitatGrid,
              colonizeVector,
              displaceYN,
              displaceVector2,
              deathVector,
              n_species,
              n_habitat,
              i, 
              j);
            //speciesCounts[fieldGrid[!(day%2)][i][j] * n_species + habitatGrid[i][j]]++;   // tally species populations
            speciesCounts[fieldGrid[!(day%2)][i][j]][habitatGrid[i][j]]++;   // tally species populations
            }// for(i and j)
        } // if(count)
        
     else // if(count)
        
        if(boundaryType)
          for(i = 0; i < gridSizeX; i++)                        // Iterate the algorithm by one time step (.
          for(j = 0; j < gridSizeY; j++){
            fieldGrid[!(day%2)][i][j] = 0;
            iterate_w(
              gridSizeX,
              gridSizeY,
              fieldGrid[(day%2)],                                      // grid in:  second layer if t is even, first layer if t is odd.
              fieldGrid[!(day%2)],                                   // grid out: second layer if t is odd, first layer if t is even.
              habitatGrid,
              colonizeVector,
              displaceYN,
              displaceVector2,
              deathVector,
              n_species,
              n_habitat,
              i,
              j);
            }
        else{ // if(boundaryType)
          for(i = 1; i < gridSizeX-1; i++)                        // Iterate the algorithm by one time step.
          for(j = 1; j < gridSizeY-1; j++){
            fieldGrid[!(day%2)][i][j] = 0;
            iterate( 
              fieldGrid[(day%2)],                                      // grid in:  second layer if t is even, first layer if t is odd.
              fieldGrid[!(day%2)],                                   // grid out: second layer if t is odd, first layer if t is even.
              habitatGrid,
              colonizeVector,
              displaceYN,
              displaceVector2,
              deathVector,
              n_species,
              n_habitat,
              i, 
              j);// for(i and j)
            }
          } // else if(count)

      if(disturbYN){                               // Call the habitat disturbance function.
        //printf("disturb\n");
        if(!disturbModel){
          if((day + 0)%distInterval == 0 & day != 0 & day >= distDelay){
            disturb(
              gridSizeX,
              gridSizeY,
              n_species,                                            // cannot be used in the first time step.
              fieldGrid[!(day%2)],                                    // Disturb fieldGridOut
              distShape,
              distPatchNumber,
              distPatchSize,
              &distCensusAll,
              distCensusKilled
              );
            printf("total disturbed cells = %d\n", distCensusAll);
            for(n = 0; n < n_species; n++)
              for(j = 0; j < n_habitat; j++)
                printf("habitat %d total species %d killed cells = %ld\n",j, n + 1, distCensusKilled[n]);
            } // if(day...)
          } // if(!disturbModel)
        if(disturbModel){
          if(dsfmt_genrand_close_open(&dsfmt) <= disturbProbR)   // If a random draw indicates disturbance:
            disturbR(
              gridSizeX,
              gridSizeY,
              n_species,
              fieldGrid[!(day%2)],
              distShape,
              disturbRateR,
              distPatchSize,
              disturbScaleR,
              &distCensusAll,
              distCensusKilled
              );
          } // if(disturbModel)
        } // if(disturbYN)
      
      if(count){
        // print the results of the cell census
        if(messages) printf("\ncell census results:\n");
        for(i = 1; i <= n_species; i++)
          for(j = 0; j < n_habitat; j++)
          //if(messages)  printf("habitat %d species %d population:  %ld\n", j, i, speciesCounts[i * n_species + j]);
          if(messages)  printf("habitat %d species %d population:  %ld\n", j, i, speciesCounts[i][j]);


        if(messages) printf("\n");    

        // write the census results to a file
        censusOutSp = fopen(censusFileSp, "a+");
        fprintf(censusOutSp, "%d\t", day);
      
        for(i = 0; i <= n_species; i++)
          for(j = 0; j < n_habitat; j++)
            //fprintf(censusOutSp, "%ld\t", speciesCounts[i * n_species + j]);
            fprintf(censusOutSp, "%ld\t", speciesCounts[i][j]);
            
        fprintf(censusOutSp, "\n");
        fclose(censusOutSp);
        
        if(disturbYN){
          censusOutSp = fopen(censusFileDist, "a+");
          fprintf(censusOutDist, "%d\t", day);
      
          for(i = 1; i <= n_species; i++)
            for(j = 0; j < n_habitat; j++)
                fprintf(censusOutDist, "%ld\t", distCensusKilled[i * n_species + j - 1]);
            
          fprintf(censusOutDist, "%d", distCensusAll);
          fprintf(censusOutDist, "\n");
          fclose(censusOutDist);
          
          distCensusAll = 0;
          
          }//if(dist)
        
        // Calculate the number of extinct species:
        for(i = 0; i < n_habitat; i++)
          nExtinct[i] = 0;                                // Initialize to 0.
      
        for(i = 1; i <= n_species; i++)           // Loop through the different species.
          for(j = 0; j < n_habitat; j++)
          if(speciesCounts[i][j] <= extinctThreshold) nExtinct[j]++;      // Increment 'nExtinct' if the census for a given species is 0.
        
        if(messages) 
          for(i = 0; i < n_habitat; i++){
            printf("habitat %d number extinct = %d\n", i, nExtinct[i]);   // Print the number extinct.  
            if(nExtinct[j] > n_species && stopExtinct) break;         // End the program if all but one species have gone extinct.
            }
  
        if(day != timeMax)
          for(i = 0; i <= n_species; i ++)
            for(j = 0; j < n_habitat; j++){
              speciesCounts[i][j] = 0;                     // Reset the species counter.
              distCensusKilled[i * n_habitat + j] = 0;                  // Reset the killed cell counter.
              } // for(j)
        } //if(count)
      
      // Save the outgoing field image to a file.
      if(save) if((day%saveInterval == 0) & (day >= saveDelay))
        saveArray(gridSizeX, gridSizeY, fieldGrid[!(day%2)], directoryOut, filePrefix2 , day, r);
      } // for(day)
    }  // for(r)
  for(i = 1; i <= n_species; i++)
    for(j = 0; j < n_habitat; j++)
      printf("%lu\n", speciesCounts[i][j]);
    
  //printf("n_species = %d\n", n_species);

  } // main
