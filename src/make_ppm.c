#define _POSIX_C_SOURCE 200809L // For POSIX.1-2008 (getline/getdelim)

#include "model.c"

int main(int argc, char *argv[])
{
    char *arrInFile = argv[1];
        char *ppmOutFile = argv[2];
    char *colorMapFile = argv[3];
    char *delim = argv[4];

    arrInFile = "../model_runs/out/5k_species_resume1.txt";
    ppmOutFile = "../model_runs/out/img/5k_species_1.ppm";
    colorMapFile = "../model_runs/color_maps/color_map_5k_sp_rainbow.txt";
    delim = " ";

    int** colorMap = readDelimIntArray(colorMapFile, " ");
    
    int** arr = readDelimIntArray(arrInFile, delim);

    int* imgDims = getDelimFileDims(arrInFile, delim);

    writeIntArrayPPM(ppmOutFile, arr, colorMap, imgDims[0], imgDims[1]);

    return 0;
}