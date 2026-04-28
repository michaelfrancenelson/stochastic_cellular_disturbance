#define DSFMT_MEXP 19937 // period length of the dSFMT random number generator

// Needed for parameter parsing:
#define LINE_LENGTH 10000
#define PARAM_LENGTH 300
#define FILE_LENGTH 10000
#define MAX_PARAMS 300

#include "random_number_generators.h"
#include "fileIO.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For parsing parameters from a configuration file
#include <time.h>   // to generate a random seed

/***************************************************************************************
 *
 * Model Parameters
 *
 ***************************************************************************************/

/**
 * @struct ParamDict
 * @brief Dictionarly-like struct to hold pairs of keys and values
 */
struct ParamDict
{
  char ***pairs;
  int nKeys;
};

/**
 * @brief Read model parameters from a delimited file.
 * @param filename The name of the file to read from.
 * @param delim The delimiter to use.
 */
struct ParamDict readParamDictionary(char *filename, char *delim)
{
  printf("Attempting to read model parameter key/value pairs from file %s.\n", filename);

  FILE *file = fopen(filename, "r");

  if (!file)
  {
    char *msg;
    sprintf(msg, "Error reading configuration file %s\n", filename);
    perror(msg);
    // return 0;
  }

  // Get the dimensions of the file:
  int row = 0, col = 0;
  char buf[LINE_LENGTH];
  // Get the number of rows and columns
  while (fgets(buf, LINE_LENGTH, file))
  {
    char *field = strtok(buf, delim);
    col = 0;
    while (field)
    {
      field = strtok(NULL, delim);
      col++;
    }
    row++;
  }
  fclose(file);

  printf("Parameter file contains %d rows.\n", row);

  struct ParamDict dict;
  dict.pairs = (char ***)calloc(row, sizeof(int **));
  char ***pairs = (char ***)calloc(row, sizeof(int **));
  dict.nKeys = row;
  for (int i = 0; i < row; i++)
  {
    dict.pairs[i] = (char **)calloc(2, sizeof(int *));
    dict.pairs[i][0] = calloc(LINE_LENGTH, sizeof(char));
    dict.pairs[i][1] = calloc(LINE_LENGTH, sizeof(char));

    pairs[i] = (char **)calloc(2, sizeof(int *));
    pairs[i][0] = calloc(LINE_LENGTH, sizeof(char));
    pairs[i][1] = calloc(LINE_LENGTH, sizeof(char));
  }

  char line1[LINE_LENGTH] = {0}; // Create a string to hold the text in a line of the config file.
  file = fopen(filename, "r");
  int i = 0, j = 0, k = 0;
  for (i = 0; i < row; i++)
  {
    fgets(line1, LINE_LENGTH, file);
    strcpy(pairs[i][0], line1); // Copy 'line1' to 'paramName'.

    // Read the key
    for (j = 0; j < LINE_LENGTH; j++) // Loop through the characters of 'paramName'.
    {
      if (pairs[i][0][j] <= ' ')
      {
        pairs[i][0][j] = '\0'; // Insert a string-ending character at the first white space
        break;
      }
    }
    printf("Read key #%d: %s.\n", i + 1, pairs[i][0]);

    // Read value
    line1[LINE_LENGTH] = '\0';
    for (k = j; k < LINE_LENGTH - 1; k++)
    {
      if (line1[k + 1] <= ' ')
      {
        pairs[i][1][k - j] = '\0';
        break;
      }
      pairs[i][1][k - j] = line1[k + 1];
    }
    printf("Read key/value pair #%d: %s, %s.\n", i + 1, pairs[i][0], pairs[i][1]);
  }
  fclose(file);
  dict.pairs = pairs;
  return dict;
}

/**
 * @brief Get a string representation of a parameter.
 * @param dict The dictionary in which to search.
 * @param key The name of the parameter.
 */
char *getDictValue(struct ParamDict dict, char *key)
{

  // Loop through the keys and look for a match:
  for (int i = 0; i < dict.nKeys; i++)
  {
    if (strcmp(dict.pairs[i][0], key) == 0)
    {
      return dict.pairs[i][1];
    }
  }

  printf("Parameter %s not found in dictionary.\n", key);
  return "NULL";
}

/**
 * @brief Get the value of an integer parameter.
 * @param dict The dictionary in which to search.
 * @param key The name of the parameter.
 */
int getIntDictValue(struct ParamDict dict, char *key)
{
  return atoi(getDictValue(dict, key));
}

/**
 * @brief Get the value of an integer parameter.
 * @param dict The dictionary in which to search.
 * @param key The name of the parameter.
 */
double getDoubleDictValue(struct ParamDict dict, char *key)
{
  return atof(getDictValue(dict, key));
}

/**
 * @struct struct that holds all the needed parameters and state variables to run the model.
 */
struct Model
{
  struct ParamDict params;
  double **speciesParams;
  int **habitatGrid;
  int ***fieldGrid;
  int *neighbors;
  double *weights;
  int *popCensus;
  int burnin;
  int **disturbanceFootprint;
  int nDisturbCells;
  double meanDistPatchCount;
  double distPatchRadius;
  int nRows;
  int nCols;
  int nSpecies;
  int nHabitats;
  int **colorMap;
};

/**
 * @brief Build a model struct reading the parameters from the input configuration file.
 * @param cfgFile The fine containing all the configuration options for the model.
 * @param messages Whether or not to print status update messages as the model is built.
 * @return A model struct that can be used for simulation.
 */
struct Model initializeModel(char *cfgFile, bool messages)
{
  // Create model and read in params:
  struct Model model;
  model.params = readParamDictionary(cfgFile, " ");

  // Initialize the RNG
  initializeRS(getIntDictValue(model.params, "seed"));

  // Convenience Variables
  model.nSpecies = getIntDictValue(model.params, "n_species") + 1; // The +1 accounts for the null species which occupies the empty cells.
  model.nHabitats = getIntDictValue(model.params, "n_habitat");
  model.distPatchRadius = getDoubleDictValue(model.params, "distPatchRadius");
  model.burnin = getIntDictValue(model.params, "burnin");

  //  Read in submodel parameters for the species from the configuration file:
  model.speciesParams = readDelimDoubleArray(getDictValue(model.params, "speciesParamsFile"), ",", 1);

  // Check if color map file is provided, if not generate random color map
  // Rows are species, columns are colors (RGB values).
  if (strcmp(getDictValue(model.params, "colorMapFile"), "NULL") == 0)
  {
    printf("No colormap file provided, generating %d random colors.\n", model.nSpecies);
    model.colorMap = (int **)calloc(model.nSpecies, sizeof(int *));
    // The empty cells should be black:
    model.colorMap[0] = calloc(3, sizeof(int));
    model.colorMap[0][0] = 0;
    model.colorMap[0][0] = 0;
    model.colorMap[0][0] = 0;
    for (int i = 1; i < model.nSpecies; i++)
    {
      model.colorMap[i] = calloc(3, sizeof(int));
      for (int j = 0; j < 3; j++)
      {
        model.colorMap[i][j] = (int)(255.0 * randomFloat());
      }
    }
  }
  else
  {
    model.colorMap = readDelimIntArray(getDictValue(model.params, "colorMapFile"), ",", 1);
  }

  // Get the size of the field from the input file:
  int *fieldDims = getDelimFileDims(getDictValue(model.params, "fieldFileName"), " ", 0);
  model.nRows = fieldDims[0];
  model.nCols = fieldDims[1];
  free(fieldDims);

  // Create the 3D field grid and 2D habitat grid.
  model.fieldGrid = (int ***)calloc(2, sizeof(int *));
  for (int i = 0; i < 2; i++)
  {
    model.fieldGrid[i] = (int **)calloc(model.nRows, sizeof(int *));
    for (int j = 0; j < model.nRows; j++)
    {
      model.fieldGrid[i][j] = (int *)calloc(model.nCols, sizeof(int));
    }
  }

  // Read in the habitat and field data:
  model.habitatGrid = readDelimIntArray(getDictValue(model.params, "habitatFileName"), " ", 0);
  if (messages)
    printf("model.c reading field file %s...\n", getDictValue(model.params, "fieldFileName"));

  if (strcmp(getDictValue(model.params, "readResumeFieldFile"), "NULL") == 0)
  {
    printf("Reading intial conditions from field file %s.\n", getDictValue(model.params, "fieldFileName"));
    readDelimIntSlice(getDictValue(model.params, "fieldFileName"), " ", model.fieldGrid, 0, 0);
  }
  else
  {
    printf("Resuming simulation from field file %s.\n", getDictValue(model.params, "readResumeFieldFile"));
    readDelimIntSlice(getDictValue(model.params, "readResumeFieldFile"), " ", model.fieldGrid, 0, 0);
  }
  printf("Successfully read habitat and species data.\n");

  // Allocate memory for model state variables
  model.neighbors = (int *)calloc(8, sizeof(int));
  model.weights = (double *)calloc(8, sizeof(double));
  model.popCensus = calloc(model.nSpecies, sizeof(int));
  printf("Allocated memory for model state variables.\n");

  // Set up disturbance footprint
  if (getDoubleDictValue(model.params, "meanDistPct") > 0)
  {
    double radius = getDoubleDictValue(model.params, "distPatchRadius");
    int footprintSize = (int)pow(2 * radius + 1, 2);
    model.disturbanceFootprint = (int **)calloc(2, sizeof(int));
    for (int i = 0; i < 2; i++)
      model.disturbanceFootprint[i] = calloc(footprintSize, sizeof(int));

    int nCells = 0;
    int radiusInt = ceil(radius);

    // Brute force: check distance to each index in bounding box:
    // Faster option might be to do this for just the first quadrant and then reflect.
    // Probably ok since this only runs once at model initialization.
    for (int i = -radiusInt; i <= radiusInt; i++)
    {
      for (int j = -radiusInt; j <= radiusInt; j++)
      {
        double dist = sqrt(pow(i, 2) + pow(j, 2));
        if (dist <= radius)
        {
          model.disturbanceFootprint[0][nCells] = i;
          model.disturbanceFootprint[1][nCells] = j;
          nCells++;
        }
      }
    }
    model.nDisturbCells = nCells;
    // Set up disturbance footprint
    // Pre-calculate the mean number of disturbance patches.

    double distArea = 0.01 * getDoubleDictValue(model.params, "meanDistPct") * model.nRows * model.nCols;
    double patchArea = (double)nCells;
    // How many patches are needed to approximately cover this area?
    model.meanDistPatchCount = distArea / patchArea;
  }
  else
  {
    model.meanDistPatchCount = 0.0;
  }

  return model;
}

/************************************************************************
 *
 * Define Submodels
 *
 ************************************************************************/

/**
 * @brief Convenience function to calculate the index of a coordinate, adjusted for wrapping around boundaries.
 * @param index The non-adjusted coordinate.
 * @param maxIndex The maximum value that any coordinate coordinate can take.
 */
int wrapIndex(int index, int maxIndex)
{
  return (index + maxIndex) % maxIndex;
}

/**
 * @brief Get the species IDs of the cells in the Moore neighborhood.
 * @param model A model struct.
 * @param row Thr row number of the central cell.
 * @param col Thr column number of the central cell.
 * @param slice Which layer in the model's field to read from.
 * @note
 */
void setMooreNeighborsEdge(
    struct Model model,
    int row, int col, int slice)
{
  int nCol = model.nCols;
  int nRow = model.nRows;
  // pre allocate the search indices
  int x[8] =
      {
          (col - 1 + nCol) % nCol,
          col,
          (col + 1) % nCol,
          (col - 1 + nCol) % nCol,
          (col + 1) % nCol,
          (col - 1 + nCol) % nCol,
          col,
          (col + 1) % nCol};
  int y[8] =
      {
          (row + 1) % nRow,
          (row + 1) % nRow,
          (row + 1) % nRow,
          row,
          row,
          (row - 1 + nRow) % nRow,
          (row - 1 + nRow) % nRow,
          (row - 1 + nRow) % nRow};

  for (int i = 0; i < 8; i++)
  {
    model.neighbors[i] = model.fieldGrid[slice][y[i]][x[i]];
  }
}

/**
 * @brief Get the species IDs of the cells in the Moore neighborhood.
 * @param model A model struct.
 * @param row Thr row number of the central cell.
 * @param col Thr column number of the central cell.
 * @param slice Which layer in the model's field to read from.
 * @note
 */
void setMooreNeighborsCenter(
    struct Model model,
    int row, int col, int slice)
{
  int nCol = model.nCols;
  int nRow = model.nRows;
  // pre allocate the search indices
  int x[8] =
      {
          col - 1,
          col,
          col + 1,
          col - 1,
          col + 1,
          col - 1,
          col,
          col + 1};
  int y[8] =
      {
          row + 1,
          row + 1,
          row + 1,
          row,
          row,
          row - 1,
          row - 1,
          row - 1};

  for (int i = 0; i < 8; i++)
  {
    model.neighbors[i] = model.fieldGrid[slice][y[i]][x[i]];
  }
}

/**
 * @brief Get the weights of a set of neighbors for colonization, competition, displacement, etc.
 * @param model A model struct.
 * @param n the number of weights to retrieve.
 * @param hab The habitat code
 * @param paramsColumn The column of the species parameters array in which the desired weights are held.
 */
void *setNeighborWeights(
    struct Model model,
    int n, int habOffset, int paramsColumn)
{
  for (int i = 0; i < n; i++)
  {
    model.weights[i] = model.speciesParams[habOffset + model.neighbors[i]][paramsColumn];
  }
}

/**
 * @brief Colonize a (preferably empty) cell.
 * @param model A model struct
 * @param row The row of the cell to colonize
 * @param col The column of the cell to colonize
 * @param habOffset Precalculate the offset for the species parameters lookup.
 * @param slice The slice (layer) of the field grid
 * @param key1 The random number for deciding whether to colonize or not
 * @param key2 The random number for deciding which neighbor colonizes
 */
int colonizeCell(
    struct Model model,
    // int row, int col,
    int habOffset, int slice,
    double key1, double key2)
{
  // Get the neighbors of the empty cell
  // Colonization probabilities are in column 5 of the species parameters array:
  // setMooreNeighbors(model, row, col, slice);
  setNeighborWeights(model, 8, habOffset, 4);

  // Need the probability that nobody colonizes the cell.
  // That's the product of 1 - weight_i for all weights.
  // Colonization probabilities are in the fifth column of the species parameters array
  double probNoColonize = 1;
  for (int i = 0; i < 8; i++)
    probNoColonize = probNoColonize * (1.0 - model.speciesParams[habOffset + model.neighbors[i]][4]);

  if (key1 < probNoColonize)
    return 0;

  return model.neighbors[weightedRandomSample(8, model.weights, key2)];
}

/**
 * @brief Decide whether the individual in a currently occupied cell should die.
 * @param model a Model struct.
 * @param speciesID Which species is up for death?
 * @param habOffset Precalculate the offset for the species parameters lookup.
 * @param key The random number for deciding whether to die or not
 */
int die(struct Model model, int speciesID, int habOffset, double key)
{
  // Death probabilities are in the 3rd column
  if (key <= model.speciesParams[habOffset + speciesID][2])
    return 0;
  return speciesID;
}

/**
 * @brief Displace the individual in a currently occpied cell
 * @param model A model struct.
 * @param row The row of the cell
 * @param col The column of the cell
 * @param slice The slice (layer) of the field grid
 * @param speciesID The species present in the cell
 * @param habOffset Precalculate the offset for the species parameters lookup.
 * @param key1 The random number for deciding whether to displace or not
 * @param key2 The random number for deciding which neighbor displaces
 */
int compete(
    struct Model model,
    // int row, int col, int slice,
    int speciesID, int habOffset,
    double key1, double key2)
{
  // setMooreNeighbors(model, row, col, slice);

  // The colonization probabilties are in the fourth column of the species parameters array.
  for (int i = 0; i < 8; i++)
  {
    model.weights[i] = model.speciesParams[habOffset + model.neighbors[i]][3];
  }

  // Need the probability of no displacement:
  double noDisplace = 1;
  for (int i = 0; i < 8; i++)
  {
    noDisplace = noDisplace * (1.0 - model.weights[i]);
  }

  if (key1 < noDisplace)
    return speciesID;

  return model.neighbors[weightedRandomSample(8, model.weights, key2)];
}

/**
 * @brief Loops through the current field and counts the number of individals of each species
 * @param field The 3D cube of species
 * @param pops The census array stored in the model
 * @param the current slice of the field
 * @param nRows how many rows are in the field?
 * @param nCols how many columns are in the field?
 * @param nSpecies how many species are in the simulation?
 */
void census(int ***field, int *pops, int slice, int nRows, int nCols, int nSpecies)
{
  for (int i = 0; i < nSpecies; i++)
    pops[i] = 0;
  for (int row = 0; row < nRows; row++)
  {
    for (int col = 0; col < nCols; col++)
    {
      pops[field[slice][row][col]]++;
    }
  }
}

/**
 * @brief Apply the disturbance submodel.
 * @param field The simulation field cube
 * @param footprintIndices A pointer to the array for holding the footprint coordinates.
 * @param nPatches How many disturbance patches to create?
 * @param sliceNext The slice of the field in which to apply the disturbances.
 * @param nRows How many rows in the field?
 * @param nCols How many columns in the field?
 * @param nCells How many cells are there in the disturbance patch?
 */
void disturb(
    int ***field, int **footprintIndices,
    int nPatches, int sliceNext,
    int nRows, int nCols, int nCells)
{

  for (int patch = 0; patch < nPatches; patch++)
  {
    int row = random_uniform(0, nRows);
    int col = random_uniform(0, nCols);
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < nCells; j++)
      {
        field[sliceNext][wrapIndex(row + footprintIndices[0][j], nRows)][wrapIndex(col + footprintIndices[1][j], nCols)] = 0;
      }
    }
  }
}

/*****************************************************************************
 *
 * Run the model
 *
 *****************************************************************************/

void stepEdge(int sliceCurrent, int sliceNext, struct Model model, int rowMin, int rowMax, int colMin, int colMax)
{
  int displacingSpecies = 0;
  for (int row = rowMin; row <= rowMax; row++)
  {
    for (int col = colMin; col <= colMax; col++)
    {
      int speciesID = model.fieldGrid[sliceCurrent][row][col];
      int hab = model.habitatGrid[row][col];
      int habOffset = hab * model.nSpecies;

      setMooreNeighborsEdge(model, row, col, sliceCurrent);

      // If the cell is unoccupied, apply colonize submodel
      if (speciesID == 0)
      {
        model.fieldGrid[sliceNext][row][col] = colonizeCell(
            model,
            habOffset,
            sliceCurrent,
            randomFloat(), randomFloat());
      }
      else
      {
        // Check if any neighbors displace the current cell
        displacingSpecies = compete(model, speciesID, habOffset, randomFloat(), randomFloat());

        if (displacingSpecies != speciesID)
        {
          model.fieldGrid[sliceNext][row][col] = displacingSpecies;
        }
        // Otherwise set up a death trial:
        else
        {
          model.fieldGrid[sliceNext][row][col] = die(model, speciesID, habOffset, randomFloat());
        }
      }
    }
  }
}

/**
 * @brief Run one step of the model simulation
 * @param slice The current layer of the field grid.  The other layer will be populated with the outcome of the step.
 * @param model A Model struct containing all the parameters and elements.
 */
void step(int sliceCurrent, int step, struct Model model)
{
  // Index of the opposite slice:
  int sliceNext = 1;
  if (sliceCurrent == 1)
    sliceNext = 0;

  int displacingSpecies = 0;

  // Loop through the edges first.
  // Top row:
  stepEdge(sliceCurrent, sliceNext, model, 0, 0, 0, model.nCols - 1);
  // Bottom row:
  stepEdge(sliceCurrent, sliceNext, model, model.nRows - 1, model.nRows - 1, 0, model.nCols - 1);
  // Left col:
  stepEdge(sliceCurrent, sliceNext, model, 0, model.nRows - 1, 0, 0);
  // Left col:
  stepEdge(sliceCurrent, sliceNext, model, 0, model.nRows - 1, model.nCols - 1, model.nCols - 1);

  // Now loop through the center
  for (int row = 1; row < model.nRows - 1; row++)
  {
    for (int col = 1; col < model.nCols - 1; col++)
    {
      int speciesID = model.fieldGrid[sliceCurrent][row][col];
      int hab = model.habitatGrid[row][col];
      int habOffset = hab * model.nSpecies;

      setMooreNeighborsCenter(model, row, col, sliceCurrent);

      // If the cell is unoccupied, apply colonize submodel
      if (speciesID == 0)
      {
        model.fieldGrid[sliceNext][row][col] = colonizeCell(
            model,
            // row, col,
            habOffset,
            sliceCurrent,
            randomFloat(), randomFloat());
      }
      else
      {
        // Check if any neighbors displace the current cell
        displacingSpecies = compete(model, speciesID, habOffset, randomFloat(), randomFloat());

        if (displacingSpecies != speciesID)
        {
          model.fieldGrid[sliceNext][row][col] = displacingSpecies;
        }
        // Otherwise set up a death trial:
        else
        {
          model.fieldGrid[sliceNext][row][col] = die(model, speciesID, habOffset, randomFloat());
        }
      }
    }
  }
  // Run the disturbance model last
  if ((model.meanDistPatchCount > 0.0) && (step >= model.burnin))
  {

    int n_patches = random_poisson(model.meanDistPatchCount);
    if (n_patches > 0)
      disturb(
          model.fieldGrid,
          model.disturbanceFootprint,
          n_patches, sliceNext,
          model.nRows, model.nCols, model.nDisturbCells);
  }
}

void saveParams(char *filename, int nSteps, double simTime, struct Model model)
{
  FILE *file;

  printf("Attempting to save simulation report to file '%s'.\n", filename);

  file = fopen(filename, "w");

  if (file == NULL | !file)
  {
    char *msg = calloc(MAX_LINE_LENGTH, sizeof(char));
    sprintf(msg, "Error writing to file %s\n", filename);
    perror(msg);
  }

  fprintf(file, "nRows: %d\n", model.nRows);
  fprintf(file, "nCols: %d\n", model.nCols);
  fprintf(file, "n_species: %s\n", getDictValue(model.params, "n_species"));
  fprintf(file, "n_habitat: %s\n", getDictValue(model.params, "n_habitat"));

  fprintf(file, "meanDistPct: %s\n", getDictValue(model.params, "meanDistPct"));
  fprintf(file, "distPatchRadius: %s\n", getDictValue(model.params, "distPatchRadius"));

  fprintf(file, "seed: %s\n", getDictValue(model.params, "seed"));

  fprintf(file, "speciesParamsFile: %s\n", getDictValue(model.params, "speciesParamsFile"));
  fprintf(file, "habitatFileName: %s\n", getDictValue(model.params, "habitatFileName"));
  fprintf(file, "outputFieldFile: %s\n", getDictValue(model.params, "outputFieldFile"));

  fprintf(file, "saveResumeFieldFile: %s\n", getDictValue(model.params, "saveResumeFieldFile"));
  fprintf(file, "outputCensusFile: %s\n", getDictValue(model.params, "outputCensusFile"));
  fprintf(file, "outputImageFile: %s\n", getDictValue(model.params, "outputImageFile"));
  fprintf(file, "colorMapFile: %s\n", getDictValue(model.params, "colorMapFile"));

  fprintf(file, "nSteps: %d\n", nSteps);
  fprintf(file, "simTimeMinutes: %f\n", simTime);
}
