#define MAX_LINE_LENGTH 1000000

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Print the current counts of each species to the console.
 * @param pops The census pops array from the model struct.
 * @param nSpecies The number of species in the simulation.
 */
void printCensus(int *pops, int nSpecies)
{
    for (int i = 0; i < nSpecies + 1; i++)
        printf("Species %d: %d\n", i, pops[i]);
}

/**
 * @brief Writes a ppm image from an array of unsigned chars.
 * @param filename The name of the file to write to.
 * @param data The 1D array of chars containing the image data.
 * @param width The width in pixels.
 * @param height the height in pixels.
 */
int write_ppm(const char *filename, unsigned char *data, int height, int width)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        char *msg;
        sprintf(msg, "write_ppm(): Error opening file %s for writing.\n", filename);
        perror(msg);
        free(msg);
        return 0;
    }

    // Write PPM header
    // P6 = binary RGB, max color value = 255
    if (fprintf(fp, "P6\n%d %d\n255\n", width, height) < 0)
    {
        char *msg;
        sprintf(msg, "write_ppm(): Error writing PPM header in file %s\n", filename);

        perror(msg);
        free(msg);
        fclose(fp);
        return 0;
    }

    // Write pixel data
    size_t data_size = (size_t)width * height * 3;
    if (fwrite(data, 1, data_size, fp) != data_size)
    {
        char *msg;
        sprintf(msg, "write_ppm(): Error writing pixel data in file %s\n", filename);

        perror(msg);
        free(msg);
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

/**
 * @brief Convenience function to save a layer of a cube of ints to a ppm image.
 * @param field The 3D cube of ints
 * @param colorMap A 2D array containing the mapping of species in the field to RGB values.  Rows are species ID, columns are R G and B values
 * @param slice The layer of the field to save
 * @param nRows The number of rows in the field.  This will become the number of pixels high of the image.
 * @param nCols The number of rows in the field.  This will become the number of pixels wide of the image.
 */
void writeIntSlicePPM(const char *filename, int ***field, int **colorMap, int slice, int nRows, int nCols)
{
    unsigned char *fieldImage = calloc(nRows * nCols * 3, sizeof(unsigned char));

    for (int row = 0; row < nRows; row++)
    {
        for (int col = 0; col < nCols; col++)
        {
            int idx = (row * nCols + col) * 3;
            fieldImage[idx] = colorMap[field[slice][row][col]][0];     // red
            fieldImage[idx + 1] = colorMap[field[slice][row][col]][1]; // green
            fieldImage[idx + 2] = colorMap[field[slice][row][col]][2]; // blue
        }
    }

    if (!write_ppm(filename, fieldImage, nRows, nCols))
    {
        fprintf(stderr, "Failed to write PPM image\n");
        free(fieldImage);
    }
    free(fieldImage);
}

/**
 * @brief Create a 2D array of ints and initialize all values to zero.
 * @param rows The number of rows (first index)
 * @param cols The number of columns (second index)
 * @return A 2D array of ints with the given dimensions.
 */
int **blankIntArray(int rows, int cols)
{
    // Allocate row pointers (VLA size determined by 'rows')
    int **arr = (int **)malloc(rows * sizeof(int *));
    if (arr == NULL)
        return NULL;

    // Allocate each row (VLA size 'cols')
    for (int i = 0; i < rows; i++)
    {
        arr[i] = (int *)malloc(cols * sizeof(int));
        if (arr[i] == NULL)
        {
            // Cleanup on failure
            for (int j = 0; j < i; j++)
                free(arr[j]);
            free(arr);
            return NULL;
        }
        // Initialize row
        for (int j = 0; j < cols; j++)
        {
            arr[i][j] = 0;
        }
    }
    return arr;
}

/**
 * @brief Create a 2D array of doubles and initialize all values to zero.
 * @param rows The number of rows (first index)
 * @param cols The number of columns (second index)
 * @return A 2D array of doubles with the given dimensions.
 */
double **blankDoubleArray(int rows, int cols)
{
    // Allocate row pointers (VLA size determined by 'rows')
    double **arr = (double **)malloc(rows * sizeof(int *));
    if (arr == NULL)
        return NULL;

    // Allocate each row (VLA size 'cols')
    for (int i = 0; i < rows; i++)
    {
        arr[i] = (double *)malloc(cols * sizeof(double));
        if (arr[i] == NULL)
        {
            // Cleanup on failure
            for (int j = 0; j < i; j++)
                free(arr[j]);
            free(arr);
            return NULL;
        }
        // Initialize row
        for (int j = 0; j < cols; j++)
        {
            arr[i][j] = 0;
        }
    }
    return arr;
}

/**
 * @brief Print the contents of a 2D array of ints to the console.
 * @param mat The 2D array
 * @param nRows the number of rows to print.  This is needed because there is no way to tell how many rows are in the 2D array.
 * @param nRows the number of columns to print.  This is needed because there is no way to tell how many columns are in the 2D array.
 */
void printIntArray(int **mat, int nRows, int nCols)
{
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief Print the contents of one layer in a 3D array of ints to the console.
 * @param cube The 3D array
 * @param slice The layer of the 3D array to print.
 * @param nRows the number of rows to print.  This is needed because there is no way to tell how many rows are in the 2D array.
 * @param nRows the number of columns to print.  This is needed because there is no way to tell how many columns are in the 2D array.
 */
void printIntSlice(int ***cube, int slice, int nRows, int nCols)
{
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {
            printf("%d ", cube[slice][i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief Print the contents of a 2D array of doubles to the console.
 * @param mat The 2D array
 * @param nRows the number of rows to print.  This is needed because there is no way to tell how many rows are in the 2D array.
 * @param nRows the number of columns to print.  This is needed because there is no way to tell how many columns are in the 2D array.
 * @param fmt The format for the string representation of the doubles
 */
void printDoubleArray(double **mat, int nRows, int nCols, char *fmt)
{
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {
            printf(fmt, mat[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief Print the contents of a 1D array of ints to the console.
 * @param vec The 1D arrayf
 * @param n the number of elements to print.  This is needed because there is no way to tell how many rows are in the 2D array.
 */
void printIntVector(int *vec, int n)
{
    for (int i = 0; i < n; i++)
        printf("%d ", vec[i]);
    printf("\n");
}

/**
 * @brief Print the contents of a 1D array of ints to the console.
 * @param vec The 1D arrayf
 * @param n the number of elements to print.  This is needed because there is no way to tell how many rows are in the 2D array.
 * @param fmt The format for the string representation of the doubles
 */
void printDoubleVector(double *vec, int n, char *fmt)
{
    for (int i = 0; i < n; i++)
        printf(fmt, vec[i]);
    printf("\n");
}

/**
 * @brief Write the contents of a 2D array of ints to a text file.
 * @param filename The name of the file to write.
 * @param arr The 2D array of ints to write.
 * @param nRows The number of rows of data to write in the array.
 * @param nCols The number of columns of data to write in the array.
 * @param delim The file delimiter, usually a space or a comma.
 * @param append Whether or not to start a new file or append to the existing content in the file.
 */
void writeIntArray(char *filename, int **arr, int nRow, int nCol, char* delim, bool append)
{
    char *mode = "w";
    if (append)
        mode = "a";
    FILE *file;

    file = fopen(filename, mode);

    if (file == NULL | !file)
    {
        char *msg;
        sprintf(msg, "writeIntArray(): Error opening file %s\n", filename);
        perror(msg);
    }

    for (int i = 0; i < nRow; i++)
    {
        for (int j = 0; j < nCol - 1; j++)
        {
            fprintf(file, "%d%s", arr[i][j], delim);
        }
        fprintf(file, "%d", arr[i][nCol - 1]);
        fprintf(file, "\n");
    }

    fclose(file);
}

/**
 * @brief Write the contents of a 2D array of ints to a text file.
 * @param filename The name of the file to write.
 * @param arr The 2D array of ints to write.
 * @param nElements How many elements to write?
 * @param delim The file delimiter, usually a space or a comma.
 * @param append Whether or not to start a new file or append to the existing content in the file.
 */
void writeIntVector(char *filename, int *arr, int nElements, char* delim, bool append)
{
    char *mode = "w";
    if (append)
        mode = "a";
    FILE *file;

    file = fopen(filename, mode);

    if (file == NULL | !file)
    {
        char *msg;
        sprintf(msg, "writeIntVector(): Error opening file %s\n", filename);
        perror(msg);
    }

    for (int i = 0; i < nElements - 1; i++)
    {
        fprintf(file, "%d%s", arr[i], delim);
    }

    fprintf(file, "%d", arr[nElements - 1]);

    fprintf(file, "\n");
    fclose(file);
}


/**
 * @brief Write the header for the census file.
 * @param filename The name of the file to write to
 * @param nSpecies The number of species in the simulation.  An additional column will be added for the 'zero species', i.e. an empty cell.
 * @param fmt The format to use for the species names/numbers.  "sp_%04d" will label species as "sp_0000", "sp_0001", etc.
 * @param delim The delimiter.  Should be a space or a comma.
 */
void writeCensusHeader(char *filename, int nSpecies, char* fmt, char* delim)
{
    FILE *file;

    file = fopen(filename, "w");

    if (file == NULL | !file)
    {
        char *msg;
        sprintf(msg, "writeCensusHeader(): Error opening file %s\n", filename);
        perror(msg);
    }

    fprintf(file, "step");
    fprintf(file, delim);
    for (int i = 0; i < nSpecies; i++)
    {
        fprintf(file, fmt, i);
        fprintf(file, delim);
    }

    fprintf(file, fmt, nSpecies);
    fprintf(file, "\n");
    fclose(file);


}

/**
 * @brief Write the contents of a 2D array of ints to a text file.
 * @param filename The name of the file to write.
 * @param arr The 2D array of ints to write.
 * @param nElements How many elements to write?
 * @param timeStep The current time step of the model.
 * @param delim The file delimiter, usually a space or a comma.
 */
void writeCensusLine(char *filename, int *arr, int nElements, int timeStep, char* delim)
{

    FILE *file;

    file = fopen(filename, "a");

    if (file == NULL | !file)
    {
        char *msg;
        sprintf(msg, "writeCensusLine(): Error opening file %s\n", filename);
        perror(msg);
    }

    fprintf(file, "%d%s", timeStep, delim);
    for (int i = 0; i < nElements; i++)
    {
        fprintf(file, "%d%s", arr[i], delim);
    }
    fprintf(file, "%d\n", arr[nElements]);
    fclose(file);
}

/**
 * @brief Write the contents of a one layer of a3D array of ints to a text file.
 * @param filename The name of the file to write.
 * @param arr The 2D array of ints to write.
 * @param slice The layer of the 3D array wo write.
 * @param nRows The number of rows of data to write in the array.
 * @param nCols The number of columns of data to write in the array.
 * @param delim The file delimiter, usually a space or a comma.
 * @param append Whether or not to start a new file or append to the existing content in the file.
 */
void writeIntSlice(char *filename, int ***arr, int slice, int nRow, int nCol, char* delim, bool append)
{
    char *mode = "w";
    if (append)
        mode = "a";
    FILE *file;

    file = fopen(filename, mode);

    for (int i = 0; i < nRow; i++)
    {
        for (int j = 0; j < nCol - 1; j++)
        {
            int k = arr[slice][i][j];
            fprintf(file, "%d%s", k, delim);
        }
        fprintf(file, "%d", arr[slice][i][nCol - 1]);
        fprintf(file, "\n"); // Save a newline character at the end of each row.
    }
    fclose(file);
}

/**
 * @brief Write the contents of a one layer of a32D array of ints to a text file.
 * @param filename The name of the file to write.
 * @param arr The 2D array of ints to write.
 * @param slice The layer of the 3D array wo write.
 * @param nRows The number of rows of data to write in the array.
 * @param nCols The number of columns of data to write in the array.
 * @param append Whether or not to start a new file or append to the existing content in the file.
 */
void writeIntSliceBinary(char *filename, int ***arr, int slice, int nRow, int nCol, bool append)
{
    char *mode = "wb";
    if (append)
        mode = "ab";
    FILE *file;

    file = fopen(filename, mode);

    for (int i = 0; i < nRow; i++)
    {
        for (int j = 0; j < nCol; j++)
        {
            int k = arr[slice][i][j];
            // fprintf(file, "%d ", k);
            fwrite(&k, sizeof(int), 1, file); // write 10 bytes from our buffer
        }
        fwrite("\n", sizeof(int), 1, file); // write 10 bytes from our buffer
        // fprintf(file, "\n"); // Save a newline character at the end of each row.
    }
    fclose(file);
}

/**
 * @brief Write the contents of a 2D array of doubles to a text file.
 * @param filename The name of the file to write.
 * @param arr The 2D array of doubles to write.
 * @param nRows The number of rows of data to write in the array.
 * @param nCols The number of columns of data to write in the array.
 * @param delim The file delimiter, usually a space or a comma.
 * @param append Whether or not to start a new file or append to the existing content in the file.
 */
void writeDoubleArray(char *filename, double **arr, int nRow, int nCol, char* delim, bool append)
{

    char *mode = "w";
    if (append)
        mode = "a";
    FILE *file;

    file = fopen(filename, mode);

    for (int i = 0; i < nRow; i++)
    { // Loop through the x and y coordinates of the field.
        for (int j = 0; j < nCol - 1; j++)
        {
            fprintf(file, "%f%s", arr[i][j], delim);
        }
        fprintf(file, "%f", arr[i][nCol - 1]);
        fprintf(file, "\n"); // Save a newline character at the end of each row.
    }

    fclose(file);
}

/**
 * @brief Retrieve the dimensions of a 2D data structure in a delimted text file.
 * @param filename The file to read the dimensions of.
 * @param delim The file delimiter.
 */
int *getDelimFileDims(char *filename, char *delim)
{
    FILE *file;

    printf("Attempting to open delimited file %s with delimiter '%s'.\n", filename, delim);

    file = fopen(filename, "r");

    if (file == NULL | !file)
    {
        char *msg = calloc(MAX_LINE_LENGTH, sizeof(char));
        sprintf(msg, "Error opening file %s\n", filename);
        perror(msg);
    }

    int row = 0, col = 0;
    char buf[MAX_LINE_LENGTH];
    // Get the number of rows and columns
    while (fgets(buf, MAX_LINE_LENGTH, file))
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
    // free(file);
    printf("Read dimensions of %s: %d rows and %d columns.\n", filename, row, col);

    int *out = (int *)calloc(2, sizeof(int));
    out[0] = row;
    out[1] = col;
    return out;
}

/**
 * @brief Read a 2D array of doubles from a delimited text file.
 * @param filename The file containing the data.
 * @param delim The file delimiter.
 * @return A pointer to a 2D array of ints the same dimensions as the data in the input file.
 */
double **readDelimDoubleArray(char *filename, char *delim)
{
    // Get the dimensions
    int *dims = getDelimFileDims(filename, delim);

    // Allocate the blank array
    double **mat = blankDoubleArray(dims[0], dims[1]);

    FILE *file;

    printf("Attempting to read %d rows and %d columns of double data from file %s.\n", dims[0], dims[1], filename);
    free(dims);

    file = fopen(filename, "r");
    if (file == NULL | !file)
    {
        char *msg;
        sprintf(msg, "Error opening file %s\n", filename);
        perror(msg);
    }

    int row = 0;
    int col = 0;

    char buf[MAX_LINE_LENGTH];

    while (fgets(buf, MAX_LINE_LENGTH, file))
    {
        char *field = strtok(buf, delim);
        col = 0;
        while (field)
        {

            mat[row][col] = atof(field);
            field = strtok(NULL, delim);
            col++;
        }
        row++;
    }

    fclose(file);
    // free(file);
    return mat;
}

/**
 * @brief Read a 2D array of ints from a delimited text file.
 * @param filename The file containing the data.
 * @param delim The file delimiter.
 * @return A pointer to a 2D array of ints the same dimensions as the data in the input file.
 */
int **readDelimIntArray(char *filename, char *delim)
{
    // Get the dimensions
    int *dims = getDelimFileDims(filename, delim);

    // Allocate the blank array
    int **mat = blankIntArray(dims[0], dims[1]);

    FILE *file;

    printf("Attempting to read %d rows and %d columns of int data from file %s.\n", dims[0], dims[1], filename);
    free(dims);

    file = fopen(filename, "r");
    if (file == NULL | !file)
    {
        char *msg;
        sprintf(msg, "Error opening file %s\n", filename);
        perror(msg);
    }

    int row = 0;
    int col = 0;

    char buf[MAX_LINE_LENGTH];

    while (fgets(buf, MAX_LINE_LENGTH, file))
    {
        char *field = strtok(buf, delim);
        col = 0;
        while (field)
        {

            mat[row][col] = atoi(field);
            field = strtok(NULL, delim);
            col++;
        }
        row++;
    }

    fclose(file);
    // free(file);
    return mat;
}

/**
 * @brief Read a 2D array of ints from a delimited file into a slice of a 3D array
 * @param filaname The file to read from
 * @param delim The delimiter
 * @param field The 3D array to write to
 * @param slice The slice into which to read
 */
void readDelimIntSlice(char *filename, char *delim, int ***field, int slice)
{
    // Get the dimensions
    int *dims = getDelimFileDims(filename, delim);

    // Read the input file
    int **mat = readDelimIntArray(filename, delim);

    for (int row = 0; row < dims[0]; row++)
    {
        for (int col = 0; col < dims[1]; col++)
        {
            field[slice][row][col] = mat[row][col];
        }
    }

    free(dims);
    free(mat);
}
