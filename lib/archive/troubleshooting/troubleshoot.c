#include <stdio.h>
#include <time.h>                                   
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>


void iterate(
  int   gridSizeX,
  int   gridSizeY,
  int   **gridIn,
  int   **gridOut,
  int   **habitat,
  float *displaceVector1,
  int   n_species,
  int   n_habitat,
  int   i,
  int   j);


int main(int argc, char *argv[])
{
  int n_species = 2;
  int n_habitat = 2;
  
  int gridSizeX = 4;
  int gridSizeY = 4;
  
  int i, j, day, tMax;

  char *displaceFileName;
  char *gridFileName;
  char *habitatFileName;
  
  FILE *displaceFile;
  FILE *gridFile;
  FILE *habitatFile;
  
  displaceFileName = argv[1];
  gridFileName     = argv[2];
  habitatFileName  = argv[3];
  tMax             = atoi(argv[4]);
  
  float *displaceVector1;              
  float *displaceVector2;
  int ***grid;
  int **habitat;    
  
  gridFile    = fopen(gridFileName, "r");
  habitatFile = fopen(habitatFileName, "r");
  
  grid    = calloc(2, sizeof(int));

  
  for(i = 0; i < 2; i++){
    grid[i] = calloc(gridSizeX, sizeof(int));
    for(j = 0; j < gridSizeX + 1; j++)
      grid[i][j] = calloc(gridSizeY, sizeof(int));
  } // for(i)
  
  habitat = calloc(4, sizeof(int));
  for(i = 0; i < gridSizeX; i++)
    habitat[i] = calloc(4, sizeof(int));
 
 
   
  
  displaceVector1 = calloc(n_species * n_species * n_habitat, sizeof(float));   // unused, but program won't run correctly without this.
  displaceVector2 = calloc(n_species * n_species * n_habitat, sizeof(float));

  // populate displaceVector1
  displaceFile = fopen(displaceFileName, "r");
  for(i = 0; i < n_species * n_species * n_habitat; i++){               
    fscanf(displaceFile, "%f", &displaceVector1[i]);
    }
  fclose(displaceFile);
  
  // populate displaceVector2
  displaceFile = fopen(displaceFileName, "r");
  for(i = 0; i < n_species * n_species * n_habitat; i++){               
    fscanf(displaceFile, "%f", &displaceVector2[i]);
    }
  fclose(displaceFile);
  
  printf("displaceVector1:\n");
  for(i = 0; i < n_species * n_species * n_species; i++)
    printf("displaceVector1[%d] = %f\n", i, displaceVector1[i]);
    
  printf("\n");
  
  printf("displaceVector2:\n");
  for(i = 0; i < n_species * n_species * n_species; i++)
    printf("displaceVector2[%d] = %f\n", i, displaceVector2[i]);

  char tempChar;
  gridFile = fopen(gridFileName, "r");
  habitatFile  = fopen(habitatFileName, "r");
  
  for(i = 0; i < gridSizeX; i++)
  for(j = 0; j < gridSizeY; j++){
    fscanf(gridFile, "%s", &tempChar);
    grid[0][i][j] = grid[1][i][j] = atoi(&tempChar);
    fscanf(habitatFile, "%s", &tempChar);
    habitat[i][j] = atoi(&tempChar);
    }

  for(day = 0; day < tMax; day++){ 
    
    printf("day = %d\n", day);


    
    for(i = 0; i < gridSizeX; i++)
    for(j = 0; j < gridSizeY; j++){
      printf("i = %d, j = %d\n", i, j);
      
      printf("grid[%d][%d][%d] = %d\n", !(day%2),i, j, grid[!(day%2)][i][j]);
      
      //grid[!(day%2)][i][j] = 1;
      
      printf("bug\n");
      
      iterate(
        gridSizeX,
        gridSizeY,
        grid[day%2],
        grid[!(day%2)],
        habitat,
        displaceVector1,
        n_species,
        n_habitat,
        i,
        j);
      } // for(i, j)
    
    } // for(day)

  } // main
  
  
void iterate(
  int   gridSizeX,
  int   gridSizeY,
  int   **gridIn,
  int   **gridOut,
  int   **habitat,
  float *displaceVector1,
  int   n_species,
  int   n_habitat,
  int   i,
  int   j)
{
  printf("iterate\n");
  
  
  } // iterate
  
int displace(
  float *displaceVector1,
  int n_species,
  int n_habitat){
  
  
  
  
  }  // displace
  