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

September 2013:  Moved functions to individual source files to un clutter the main source file.

September 2013:  Changed random number generator function to dSFMT.  This should provide better random
                 numbers without adding much time to the routine.

September 2013:  Created an option for wrapping boundaries (boundaryType = 1).

September 2013:  Added a shape option for the disturbance function.  (distShape:  1 = square, 2 = round)

September 2013:  Added disturbance losses columns for each species to the census file.

October 2013:  Added a disturbance delay parameter (the first disturbance happens after this time step).


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

// Function files:
#include "../dSFMT-src-2.2.2/dSFMT.c"                 // dSFMT random number generator
#include "../dSFMT-src-2.2.2/dSFMT.h"                 // dSFMT random number generator
dsfmt_t dsfmt;                                        // Required for dSFMT.
// #include "../functions/randomFloat.c"              // Random number generator using rand().  
                                                      // Uncomment in disturb.c, colonize.c, deathPanel.c to use.
#include "../functions/readCfg.c" 
#include "../functions/colonize.c"
#include "../functions/compete.c" 
#include "../functions/deathPanel.c"
#include "../functions/disturb.c"
#include "../functions/printArray.c"
#include "../functions/saveArray.c"
#include "../functions/iterate.w.c" 
#include "../functions/Bernoulli.c" 
#include "../functions/iterate.c" 


int main(int argc, char *argv[])
{
  FILE *configFile;
  FILE *fileField;
  FILE *fileRes;
  FILE *fileResEqn;
  FILE *fileDeathEqn;
  FILE *censusOutSp;
  FILE *censusOutDist;
    
  char *cfgFile = argv[1];
  
  int i, j, k, n, t, r;
  
  // Variables for paramemters set in the cfg file:
  int gridSize;                 // The length of one side of the square playing field
  int n_species;                // Number of species or species in the simulation
  int n_resource;               // Number of resource levels in the simulation
  int save;                     // Save the field grid periodically? (0/1)
  int saveInterval;             // Interval at which to save the field grid
  int saveDelay;                // Number of time steps to wait before the first save.
  int print;                    // Print the playing field to the console window each time step? (0/1)
  int timeStart;                // What time step should the simulation start on?
  int timeMax;                  // Ending time step for the simulation
  int count;                    // Should a census of species be taken? (0/1)
  int countInterval;            // At what interval should the census occur? 
  int dist;                     // Invoke the disturbance algorithm? (0/1)
  int distShape;                // What shape for the disturbance patches? (1 = square, 2 = circular)
  int distInterval;             // At what interval should the disturbances occur?
  int distPatchNumber;          // How many (square) disturbance patches should be created?
  int distPatchSize;            // How large should one side of the square disturbance patches be?
  int distDelay;                // How long should the simulation wait until the first disturbance event?
  int reps;                     // How many replicate simulations should be run?
  int messages;                 // Print program messages to the console window? (0/1)
  int resume;                   // Resume the simulation from a saved field? (0/1)
  int resumeStart;              // Which rep should the resume begin with?
  int resumeEnd;                // Which rep should the resume end with?
  int stopExtinct;              // Should the simulation stop if all but one species go extinct?
  int boundaryType;             // What kind of boundaries?  0 = absorbing, 1 = wrapping.
  unsigned int seed;            // What should the random seed be?
  
  
  int nExtinct = 0;            // 'nExtinct' keeps track of how many species have gone extinct. 
  
  char filePrefix1[100] = "m.field.in.g";       // The prefixes for the saved field files.
  char filePrefix2[100] = "m.field.out.g";
  
  char censusFileSp[LINE_LENGTH] = "";          // character array for species census file name
  char censusFileDist[LINE_LENGTH] = "";        // character array for disturbance census file name
  
  char fieldFileName[LINE_LENGTH];              // The path to the initial playing field file (new simulation)
  char resFileName[LINE_LENGTH];                // The path to the resource field file
  char resEqnFileName[LINE_LENGTH];             // The path to the growth equations file
  char directoryOut[LINE_LENGTH];               // The path in which to output save files
  char resumeFileName[LINE_LENGTH];             // The path to the initial playing field file (resumed simulation)

  float         **colonizeProbs;                // A 2D array to hold the growth probabilities
  float         **deathProbs;                   // A 2D array to hold the death probabilities
  unsigned char **resourceGrid;                 // A 2D array to hold the resource field
  unsigned char ***fieldGrid;                   // A 3D array to hold the playing field
  long int *speciesCounts;                      // A 1D array to hold the census of each species
  long int *speciesCounts2;

  // read in paramaters from config file
  gridSize        = atoi(readCfg(cfgFile, "gridSize"));
  n_species       = atoi(readCfg(cfgFile, "n_species"));
  n_resource      = atoi(readCfg(cfgFile, "n_resource"));
  save            = atoi(readCfg(cfgFile, "save"));
  print           = atoi(readCfg(cfgFile, "print"));
  timeStart       = atoi(readCfg(cfgFile, "timeStart"));
  timeMax         = atoi(readCfg(cfgFile, "timeMax"));
  saveInterval    = atoi(readCfg(cfgFile, "saveInterval"));
  saveDelay       = atoi(readCfg(cfgFile, "saveDelay"));
  count           = atoi(readCfg(cfgFile, "count"));
  countInterval   = atoi(readCfg(cfgFile, "countInterval"));
  dist            = atoi(readCfg(cfgFile, "dist"));
  distInterval    = atoi(readCfg(cfgFile, "distInterval"));
  distPatchNumber = atoi(readCfg(cfgFile, "distPatchNumber"));
  distPatchSize   = atoi(readCfg(cfgFile, "distPatchSize"));
  distShape       = atoi(readCfg(cfgFile, "distShape"));
  distDelay       = atoi(readCfg(cfgFile, "distDelay"));
  reps            = atoi(readCfg(cfgFile, "reps"));
  messages        = atoi(readCfg(cfgFile, "messages"));
  resume          = atoi(readCfg(cfgFile, "resume"));  
  resumeStart     = atoi(readCfg(cfgFile, "resumeStart")); 
  resumeEnd       = atoi(readCfg(cfgFile, "resumeEnd")); 
  seed            = atoi(readCfg(cfgFile, "seed"));
  stopExtinct     = atoi(readCfg(cfgFile, "stopExtinct"));
  boundaryType    = atoi(readCfg(cfgFile, "boundaryType"));


  int distCensusAll = 0;                        // The number of disturbed cells (occupied or not) from disturb()
  long int *distCensusKilled;                    // Array to hold the number and species of cells killed by disturb()
  
       
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
    if(dist){
      printf("disturb                     = yes\n");
      printf("disturbance interval        = %d\n", distInterval);
      printf("disturbance patch number    = %d\n", distPatchNumber);
      printf("disturbance patch size      = %d\n", distPatchSize);
      printf("disturbance shape           = %d\n", distShape);}
      else printf("disturb                     = no\n");
    } // if(messages)
  
  colonizeProbs = calloc(n_species, sizeof(long int));         // Allocate memory for the growth and death probability arrays.
  deathProbs = calloc(n_species, sizeof(long int));
  distCensusKilled = calloc(n_species, sizeof(long int));
  
  for(i = 0; i < n_species + 1; i++){
    colonizeProbs[i] = calloc(n_resource + 1, sizeof(float));
    deathProbs[i] = calloc(n_resource + 1, sizeof(float));
    }
  
  resourceGrid = calloc(gridSize, ELEMENT_SIZE);        // Allocate memory for the reource and field grid arrays.
  fieldGrid    = calloc(2, ELEMENT_SIZE);
    
  for(i = 0; i < gridSize; i++)                         // Create the 2D resource grid.
    resourceGrid[i] = calloc(gridSize, ELEMENT_SIZE);  

  for(i = 0; i < 3; i++){                               // Create the 3D field grid.
    fieldGrid[i] = calloc(gridSize, ELEMENT_SIZE);
    for(j = 0; j < gridSize; j++)
      fieldGrid[i][j] = calloc(gridSize, ELEMENT_SIZE);
    } // for(i)

  speciesCounts = calloc(n_species + 1, sizeof(long int));      // Allocate memory for the species cell counter.
  speciesCounts2 = calloc(n_species + 1, sizeof(long int));
  
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
    if(messages) printf("\n");
    }
  
  
  
  unsigned char *neighborCounts;                            // Create a vector to hold the neighbor counts.
  neighborCounts = calloc(n_species, ELEMENT_SIZE);

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
        censusFileSp[i] = directoryOut[i];
      
      char censusPrefix[100] = "census.sp.";
      char censusNum[100];
      sprintf(censusNum, "%d", r);
      strcat(censusFileSp, censusPrefix);
      strcat(censusFileSp, censusNum);
      strcat(censusFileSp, ".txt");
      
      
      
      if(dist){
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
        for(i = 1; i <= n_species; i++){
          fprintf(censusOutSp, "%s", "sp_");
          fprintf(censusOutSp, "%d\t", i);
          } // for(i)

        fprintf(censusOutSp, "\n");
        fclose(censusOutSp);
        
        
        
        if(dist){
          censusOutDist = fopen(censusFileDist, "w");
          fprintf(censusOutDist, "Time\t");  // Write the column headings
          
          for(i = 1; i <= n_species; i++){
            fprintf(censusOutDist, "%s", "sp_");
            fprintf(censusOutDist, "%d", i);
            fprintf(censusOutDist, "%s\t", "_d");
            } // for(i)
          fprintf(censusOutDist, "%s", "total.dist");
          fprintf(censusOutDist, "\n");
          fclose(censusOutDist);          
          
          } //if(dist)
        
        
        
        } // if(timeStart == 0)
      } // if(count)

    for(t = timeStart; t <= timeMax; t++){                     // main program loop:
      
      if(messages) printf("t = %d\n", t);                      // Print the time step.
      
      if(print) printArray(gridSize, fieldGrid[t%2]);          // Print gridIn.  'gridIn' is the first layer of the 3D field
                                                               // array if the time step is an even number.  'gridIn' is the
                                                               // second layer when the time step is odd.
      
      //save the incoming field image
      if(save) if((t%saveInterval == 0) & (t >= saveDelay)) 
        saveArray1(gridSize, fieldGrid[!(t%2)], directoryOut, filePrefix1 , t, r);

      if(dist){                               // Call the habitat disturbance function.
        if((t + 0)%distInterval == 0 & t != 0 & t >= distDelay){
          disturb(
                  gridSize,
                  n_species,                                            // cannot be used in the first time step.
                  fieldGrid[(t%2)],                                    // Disturb fieldGridOut
                  distShape,
                  distPatchNumber,
                  distPatchSize,
                  &distCensusAll,
                  distCensusKilled
                  );
          printf("total disturbed cells = %d\n", distCensusAll);
          for(n = 0; n < n_species; n++)
            printf("total species %d killed cells = %ld\n", n + 1, distCensusKilled[n]);
        }
      } // if(dist)
      
      if(count){                                  // Create the census:
        for(i = 0; i < n_species + 1; i ++){
          speciesCounts[i]=0;                     // Reset the species counter.
          }
        
        if(boundaryType)
          for(i = 0; i < gridSize; i++)                        // Iterate the algorithm by one time step (.
          for(j = 0; j < gridSize; j++){
            fieldGrid[!(t%2)][i][j] = 0;
            iterate_w(                                    
              gridSize, 
              fieldGrid[(t%2)],                                      // grid in:  second layer if t is even, first layer if t is odd.
              fieldGrid[!(t%2)],                                   // grid out: second layer if t is odd, first layer if t is even. 
              resourceGrid,
              colonizeProbs,
              deathProbs,
              n_species, 
              i, 
              j);
            speciesCounts[fieldGrid[!(t%2)][i][j]]++;   // tally species populations
            }// for(i and j)
        
       
        else // if(boundaryType)
          for(i = 1; i < gridSize-1; i++)                        // Iterate the algorithm by one time step.
          for(j = 1; j < gridSize-1; j++){
            fieldGrid[!(t%2)][i][j] = 0;
            iterate(                                    
              gridSize, 
              fieldGrid[(t%2)],                                      // grid in:  second layer if t is even, first layer if t is odd.
              fieldGrid[!(t%2)],                                   // grid out: second layer if t is odd, first layer if t is even. 
              resourceGrid,
              colonizeProbs,
              deathProbs,
              n_species, 
              i, 
              j);
            speciesCounts[fieldGrid[!(t%2)][i][j]]++;   // tally species populations
            }// for(i and j)
        } // if(count)
        
      else // if(count)
        
        if(boundaryType)
          for(i = 0; i < gridSize; i++)                        // Iterate the algorithm by one time step (.
          for(j = 0; j < gridSize; j++){
            fieldGrid[!(t%2)][i][j] = 0;
            iterate_w(                                    
              gridSize, 
              fieldGrid[(t%2)],                                      // grid in:  second layer if t is even, first layer if t is odd.
              fieldGrid[!(t%2)],                                   // grid out: second layer if t is odd, first layer if t is even. 
              resourceGrid,
              colonizeProbs,
              deathProbs,
              n_species, 
              i, 
              j);// for(i and j)
            }
        else{ // if(boundaryType)
          for(i = 1; i < gridSize-1; i++)                        // Iterate the algorithm by one time step.
          for(j = 1; j < gridSize-1; j++){
            fieldGrid[!(t%2)][i][j] = 0;
            iterate(                                    
              gridSize, 
              fieldGrid[(t%2)],                                      // grid in:  second layer if t is even, first layer if t is odd.
              fieldGrid[!(t%2)],                                   // grid out: second layer if t is odd, first layer if t is even. 
              resourceGrid,
              colonizeProbs,
              deathProbs,
              n_species, 
              i, 
              j);// for(i and j)
            }
          } // else if(count)
      
      if(count){ 
      
        // print the results of the cell census
        if(messages) printf("\ncell census results:\n");
        for(i = 1; i <= n_species; i++)
          if(messages)  printf("species %d population in:  %ld\n", i, speciesCounts[i]);

        if(messages) printf("\n");    

        // write the census results to a file
        censusOutSp = fopen(censusFileSp, "a+");
        fprintf(censusOutSp, "%d\t", t);
      
        for(i = 1; i <= n_species; i++){
          fprintf(censusOutSp, "%ld\t", speciesCounts[i]);
          }
        fprintf(censusOutSp, "\n");
        fclose(censusOutSp);
        
        if(dist){
          censusOutSp = fopen(censusFileDist, "a+");
          fprintf(censusOutDist, "%d\t", t);
      
          for(i = 1; i <= n_species; i++){
            fprintf(censusOutDist, "%ld\t", distCensusKilled[i - 1]);
            }
          fprintf(censusOutDist, "%d", distCensusAll);
          fprintf(censusOutDist, "\n");
          fclose(censusOutDist);
          
          distCensusAll = 0;
          
          }//if(dist)
        
        // Calculate the number of extinct species:
        nExtinct = 0;                                // Initialize to 0.
      
        for(i = 0; i <= n_species; i++)           // Loop through the different species.
          if(speciesCounts[i] == 0) nExtinct++;      // Increment 'nExtinct' if the census for a given species is 0.
        
        if(messages) printf("number extinct = %d\n", nExtinct);   // Print the number extinct.
      
        if(nExtinct >= n_species - 1 && stopExtinct) break;         // End the program if all but one species have gone extinct.

        if(t != timeMax)
          for(i = 0; i <= n_species; i ++){
            speciesCounts[i] = 0;                     // Reset the species counter.
            speciesCounts2[i] = 0;                     // Reset the species counter.

            distCensusKilled[i] = 0;                  // Reset the killed cell counter.
            }
        } //if(count)
      
      // Save the outgoing field image to a file.
      if(save) if((t%saveInterval == 0) & (t >= saveDelay))
        saveArray1(gridSize, fieldGrid[!(t%2)], directoryOut, filePrefix2 , t, r);
      } // for(t)
    }  // for(r)
  for(i = 1; i <= n_species; i++) 
    printf("%lu\n", speciesCounts[i]);

  } // main
