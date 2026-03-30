#define _POSIX_C_SOURCE 200809L // For POSIX.1-2008 (getline/getdelim)

#include "model.c"

int main(int argc, char *argv[])
{
    char *cfgFile = argv[1];
    int nSteps = atoi(argv[2]);
    int quiet = 1;
    if (argv[3] != NULL)
        quiet = atoi(argv[3]);

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

    int nRows = model.nRows;
    int nCols = model.nCols;
    int nSp = model.nSpecies;

    char *colorMapFile = getDictValue(model.params, "colorMapFile");
    char *outputFieldFile = getDictValue(model.params, "outputFieldFile");
    char *outputImageFile = getDictValue(model.params, "outputImageFile");

    census(model.fieldGrid, model.popCensus, 0, nRows, nCols, nSp);
    if (quiet != 1)
        printCensus(model.popCensus, nSp);
    writeCensusHeader(saveCensusFile, nSp, "sp_%04d", ",");
    writeCensusLine(saveCensusFile, model.popCensus, nSp, 0, ",");

    if (strcmp(outputFieldFile, "NULL") == 1)
        writeIntSlice(saveFieldFile, model.fieldGrid, 0, nRows, nCols, " ", false);

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
        if ((i % censusInterval) == 0)
        {
            printf("Time step %d, censusing...", i);
            if (quiet != 1)
                printCensus(model.popCensus, nSp);
            census(model.fieldGrid, model.popCensus, sliceNext, nRows, nCols, nSp);
            writeCensusLine(saveCensusFile, model.popCensus, nSp, i, ",");
        }
        if (strcmp(outputFieldFile, "NULL") != 0)
            writeIntSlice(
                saveFieldFile, model.fieldGrid, sliceNext,
                nRows, nCols, ",", true);
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
            nRows, nCols, " ", false);
    }

    long int end_time = time(NULL);
    double simTime = ((double)(end_time - start_time)) / 60.0;

    if (strcmp(getDictValue(model.params, "simReportFile"), "NULL") != 1)
        saveParams(getDictValue(model.params, "simReportFile"), nSteps, simTime, model);

    return 0;
}