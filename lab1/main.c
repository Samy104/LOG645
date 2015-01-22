#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
int **matrix;
int operation = 1;
int maxrow = 8,
    maxcol = 8;
MPI_Win win;

void printMatrix()
{
  int row, col;
  printf("------------------\n");
  for(row = 0; row < maxrow; row++)
  {
    printf("|");
    for(col = 0; col < maxcol; col++)
    {
      printf("%d ",matrix[row][col]);
    }
    printf("|\n");
  }
  printf("\n------------------\n");
}

void setInitialVal(int value)
{
  int row, col;
  matrix = (int**)calloc(maxrow,sizeof(int*));
  for(row = 0; row < maxrow; row++)
  {
    matrix[row] = (int*)calloc(maxcol,sizeof(int));
    for(col = 0; col < maxcol; col++)
    {
      matrix[row][col] = value;
    }
  }
}

int main (int argc, char* argv[])
{
  int rank, size;
 
 // This will return an error and the MPI_COOM_WORLD will be instantiated
  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
  operation = atoi(argv[1]);

  //printf("Rank %d",rank);
  setInitialVal(atoi(argv[2]));

  MPI_Bcast(matrix, maxrow*maxcol, MPI_INT, 0, MPI_COMM_WORLD);
  int alteration;
  // Execution du parallel
  if(rank == 0)
  { 
    /*MPI_Alloc_mem(sizeof(matrix) * sizeof(int), MPI_INFO_NULL, matrix);
    MPI_Win_create(matrix, sizeof(matrix) * sizeof(int), sizeof(int),
                 MPI_INFO_NULL, MPI_COMM_WORLD, &win);*/
      MPI_Win_allocate(sizeof(matrix)*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, matrix, &win);
  }
  else
  {
    MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win); 
  }
  
  if(rank == 1)
  {
    int wut = 999;
    MPI_Put(&(matrix[0][0]), 1, MPI_INT, 0, rank, 1, MPI_INT, win);
  }

  for(alteration = 1;alteration < atoi(argv[3]); alteration++)
  {
    int currentProcessor = 0;
    int row, col;

    //First operation
    if(operation == 1)
    {
      for(row = 0; row < maxrow; row++)
      {
        for(col = 0; col < maxcol; col++)
        {
          if(currentProcessor == rank)
          {
            //int retrieved = MPI_Get();

            int val = matrix[row][col] + (row + col) * alteration;
            matrix[row][col] = val; 
          }
          currentProcessor = (currentProcessor < size-1) ? currentProcessor+1 : 0;
        }
      }
    } 
    else //Second operation
    {
      //Suck mah balls mr garisson
    }
    

  }
  if(rank == 0)
  {

    printMatrix();
  }

  //Fin du programme afficher le temps

  MPI_Finalize();
  return 0;
}



