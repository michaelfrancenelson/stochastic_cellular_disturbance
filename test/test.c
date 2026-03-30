#define _POSIX_C_SOURCE 200809L // For POSIX.1-2008 (getline/getdelim)

#include "../src/model.c"

void testRNG()
{
   initializeRS(0);

   for (int i = 0; i < 1; i++)
   {
      printf("random normal: %f\n", random_normal(0, 1));
      printf("random binomial: %d\n", rbinom(1000, 0.75));
   }

   double weights[] = {0.2, 0.3, 0.5};
   int n = 3;

   int n0 = 0, n1 = 0, n2 = 0;

   int tests = 10;

   for (int i = 0; i < tests; i++)
   {
      int index = weightedRandomSample(n, weights, randomFloat());

      if (index == 0)
         n0++;
      if (index == 1)
         n1++;
      if (index == 2)
         n2++;
   }

   printf("1: %d, 2: %d, 3: %d\n", n0, n1, n2);

   int min = -5;
   int max = 5;
   double mu = 0.2;

   tests = 1000;
   printf("c(");
   for (int i = 0; i < tests; i++)
   {
      // printf("random uniform (%d, %d): %d\n", min, max, random_uniform(min, max));
      // printf("Random Poisson (mu = %f): %d\n", mu, random_poisson(mu));
      //  for import into R

      printf("%d, ", random_poisson(mu));
   }
   printf("%d)", random_poisson(mu));

   // printf("%d)", random_poisson(mu));

   printf("finished testing RNG");
}

void testFileIO()
{

   printf("Testing file IO...\n");
   int nRows = 10;
   int nCols = 12;

   printIntArray(blankIntArray(nRows, nCols), nRows, nCols);
   printDoubleArray(blankDoubleArray(nRows, nCols), nRows, nCols, "%.1f ");

   printIntArray(readDelimIntArray("./cfg/test1_field.txt", " "), 20, 30);
   printDoubleArray(readDelimDoubleArray("./cfg/test1_field.txt", " "), 20, 30, "%0.2f ");

   // Test file writing:
   // writeIntArray("test1_write_file.txt", mat2, 9, 10, false);

   // Test append mode
   // writeIntArray("test1_write_file2.txt", mat2, 9, 10, false);
   // writeIntArray("test1_write_file2.txt", mat3, 20, 30, true);
   // writeDoubleArray("test1_write_file2.txt", mat2d, 9, 10, true);

   // Test writing a slice:
   // struct Model model = initializeModel("test1.cfg", false);
   // writeIntSlice("test1_write_slice.txt", model.fieldGrid, 0, model.nRows, model.nCols, false);
   // writeIntSlice("test1_write_slice.txt", model.fieldGrid, 1, model.nRows, model.nCols, true);

   getDelimFileDims("./cfg/test1_colonize.txt", " ");
   getDelimFileDims("./cfg/test_data.csv", ",");
   printIntArray(readDelimIntArray("./cfg/test_data.csv", ","), 4, 5);
}

void testModel()
{
   printf("Testing model...\n\n");

   struct Model model = initializeModel("./cfg/test1.cfg", false);

   int nsp = model.nSpecies;
   int nhab = model.nHabitats;

   printf("testing displace probs\n");
   printDoubleArray(model.displaceProbs, nhab * (nsp + 1), nsp + 1, "%.2f ");
   printf("testing death probs\n");
   printDoubleArray(model.deathProbs, nsp + 1, nhab, "%.2f ");
   printf("testing colonize probs\n");
   printDoubleArray(model.colonizeProbs, nsp + 1, nhab, "%.1f ");
   printf("testing habitat grid\n");
   printIntArray(model.habitatGrid, nsp + 1, nhab);
   printf("testing field grid\n");
   printIntSlice(model.fieldGrid, 0, 20, 30);
}

void testSubmodels()
{

   printf("Testing submodels...\n\n");

   char *cfgFile = "test3cfg";
   char *fieldFileOut = "./out/test3_field.txt";

   struct Model model = initializeModel(cfgFile, false);

   getMooreNeighbors(
       0, 0,
       model.nRows, model.nCols,
       model.fieldGrid,
       model.neighbors,
       0);
   for (int i = 0; i < 8; i++)
      printf("%d ", model.neighbors[i]);

   getNeighborWeights(8, model.neighbors, model.weights, 1, model.colonizeProbs);
   printf("Model weights: \n");
   for (int i = 0; i < 8; i++)
      printf("%f ", model.weights[i]);

   int colonizer;
   colonizer = colonizeCell(
       model.fieldGrid,
       model.habitatGrid,
       model.colonizeProbs,
       model.neighbors,
       model.weights,
       2, 0, 0,
       model.nRows, model.nCols,
       randomFloat(), randomFloat());
   printf("colonizing species: %d\n", colonizer);

   step(0, 0, model);

   int nSteps = 0;

   writeIntSlice(fieldFileOut, model.fieldGrid, 0, model.nRows, model.nCols, false);

   for (int i = 0; i < nSteps; i++)
   {

      step(i % 2, i, model);
      writeIntSlice(fieldFileOut, model.fieldGrid, i % 2, model.nRows, model.nCols, true);
   }

   // disturb(model.fieldGrid, 5.2, 1, 0, model.nRows, model.nCols);
   printIntSlice(model.fieldGrid, 0, model.nRows, model.nCols);
}

void testSimulate(int nSteps)
{

   printf("Testing simulation....\n");

   char *fieldFilenameOut = "./out/test4_field_out.txt";
   char *cfgFile = "test4.cfg";
   bool writePPM = false;

   struct Model model = initializeModel(cfgFile, false);

   int nRows = model.nRows;
   int nCols = model.nCols;
   int nSp = model.nSpecies;
   char ppmFilename[1000];

   writeIntSlice(fieldFilenameOut, model.fieldGrid, 0, nRows, nCols, false);

   // Make a random color map:
   int **colorMap = (int **)calloc(nSp + 1, sizeof(int));
   for (int i = 0; i < nSp + 1; i++)
   {
      colorMap[i] = calloc(3, sizeof(int));
      for (int j = 0; j < 3; j++)
      {
         colorMap[i][j] = (int)(255.0 * randomFloat());
      }
   }

   int i = 0;
   for (i = 0; i < nSteps; i++)
   {
      int slice = i % 2;

      if (writePPM)
      {
         sprintf(ppmFilename, "./out/ppm_test_%03d.ppm", i);
         writeIntSlicePPM(ppmFilename, model.fieldGrid, colorMap, slice, nRows, nCols);
      }

      printf("time step %d of %d.\n", i, nSteps);
      disturb(model.fieldGrid, model.disturbanceFootprint, 3.1, 2, slice, nRows, nCols);
      step(slice, i, model);
      writeIntSlice(fieldFilenameOut, model.fieldGrid, i % 2, nRows, nCols, true);
   }
}

void testPPM()
{

   unsigned long int seed = rand();

   srand((unsigned)time(NULL)); // Initialize the random number generator for the dSFMT seed.
   seed = rand();

   dsfmt_init_gen_rand(&dsfmt, seed);

   int nRows = 1000;
   int nCols = 1000;
   int nSpecies = 10;

   int **colorMap = (int **)calloc(nSpecies + 1, sizeof(int));

   for (int i = 0; i < nSpecies + 1; i++)
      colorMap[i] = calloc(3, sizeof(int));

   int ***field = (int ***)calloc(2, sizeof(int **));
   for (int i = 0; i < 2; i++)
   {
      field[i] = (int **)calloc(nRows, sizeof(int *));
      for (int j = 0; j < nRows; j++)
      {
         field[i][j] = (int *)calloc(nCols, sizeof(int));
         for (int k = 0; k < nCols; k++)
         {
            field[i][j][k] = (int)(nSpecies * randomFloat());
         }
      }
   }

   for (int i = 0; i < nSpecies + 1; i++)
   {
      for (int j = 0; j < 3; j++)
      {
         colorMap[i][j] = (int)(255.0 * randomFloat());
      }
   }

   int nFrames = 100;
   int slice = 0;

   for (int f = 0; f < nFrames; f++)
   {
      for (int j = 0; j < nRows; j++)
      {
         for (int k = 0; k < nCols; k++)
         {
            field[slice][j][k] = (int)(nSpecies * randomFloat());
         }
      }

      char fname[1000];
      sprintf(fname, "./out/img/ppm_test_%03d.ppm", f);

      writeIntSlicePPM(fname, field, colorMap, slice, nRows, nCols);
   }
}

void testModelRunner()
{
   char *cfgFile = "./cfg/test1.cfg";
   int nSteps = 100;

   int quiet = 1;

   long int start_time = time(NULL);

   printf("Running model with configuration file %s\n", cfgFile);
   printf("Simulation length: %d steps.\n", nSteps);

   struct Model model = initializeModel(cfgFile, false);
   printf("Reading configuration file %s\n", cfgFile);

   char *saveCensusFile = getDictValue(model.params, "outputCensusFile");
   char *saveFieldFile = getDictValue(model.params, "outputFieldFile");

   int censusInterval = 1;
   if (strcmp(getDictValue(model.params, "censusInterval"), "NULL") != 0)
      censusInterval = getIntDictValue(model.params, "censusInterval");

   if (getIntDictValue(model.params, "seed") == 0)
      initializeRS(0);
   printf("Saving output to %s\n", saveFieldFile);

   int nRows = model.nRows;
   int nCols = model.nCols;
   int nSp = model.nSpecies;

   char *colorMapFile = getDictValue(model.params, "colorMapFile");
   char *outputFieldFile = getDictValue(model.params, "outputFieldFile");
   char *outputImageFile = getDictValue(model.params, "outputImageFile");

   census(model.fieldGrid, model.popCensus, 0, nRows, nCols, nSp);
   if (quiet != 1)
      printCensus(model.popCensus, nSp);
   writeCensusLine(saveCensusFile, model.popCensus, nSp + 1, 0, true);

   if (strcmp(outputFieldFile, "NULL") == 1)
      writeIntSlice(saveFieldFile, model.fieldGrid, 0, nRows, nCols, false);

   char imgFileName[1000];
   if (strcmp(outputImageFile, "NULL") != 0)
   {
      sprintf(imgFileName, "%s_%05d.ppm", outputImageFile, 0);
      writeIntSlicePPM(imgFileName, model.fieldGrid, model.colorMap, 0, nRows, nCols);
      printf("Image output filename: %s\n", imgFileName);
   }

   int slice = 0;
   int sliceNext;
   for (int i = 1; i <= nSteps; i++)
   {
      slice = (i + 1) % 2;
      sliceNext = (slice + 1) % 2;
      printf("time step %d of %d.\n", i, nSteps);
      step(slice, i, model);
      printf("census interval %d\n", censusInterval);
      if ((i % censusInterval) == 0)
      {
         printf("Time step %d, censusing...", i);
         if (quiet != 1)
            printCensus(model.popCensus, nSp);
         census(model.fieldGrid, model.popCensus, sliceNext, nRows, nCols, nSp);
         writeCensusLine(saveCensusFile, model.popCensus, nSp + 1, i, true);
      }
      if (strcmp(outputFieldFile, "NULL") != 0)
         writeIntSlice(
             saveFieldFile, model.fieldGrid, sliceNext,
             nRows, nCols, true);
      if (strcmp(outputImageFile, "NULL") != 0)
      {
         sprintf(imgFileName, "%s_%05d.ppm", outputImageFile, i);
         writeIntSlicePPM(imgFileName, model.fieldGrid, model.colorMap, sliceNext, nRows, nCols);
      }
   }
   if (strcmp(getDictValue(model.params, "saveResumeFieldFile"), "NULL") != 0)
   {
      writeIntSlice(
          getDictValue(model.params, "saveResumeFieldFile"),
          model.fieldGrid, slice,
          nRows, nCols, false);
   }

   long int end_time = time(NULL);
   double simTime = ((double)(end_time - start_time)) / 60.0;

   if (strcmp(getDictValue(model.params, "simReportFile"), "NULL") != 1)
      saveParams(getDictValue(model.params, "simReportFile"), nSteps, simTime, model);
}

void testReadParams()
{
   char *filename = "./cfg/test6.cfg";

   struct ParamDict dict = readParamDictionary(filename, " ");

   char *param = "seed";
   printf("parameter: %s, value: %s\n", param, getDictValue(dict, param));

   param = "NA";
   printf("parameter: %s, value: %s\n", param, getDictValue(dict, param));

   printf(" ");
}

int main()
{

   // testReadParams();
   testModelRunner();
   // testPPM();
   // testSimulate(200);
   // testSubmodels();
   // testRNG();
   //    testFileIO();
   //   testModel();

   // return 0;
}