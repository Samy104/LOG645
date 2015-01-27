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
  printf("-------------------------------------------------------------------\n");
  for(row = 0; row < maxrow; row++)
  {
    printf("|");
    for(col = 0; col < maxcol; col++)
    {
      printf("%d\t",matrix[row][col]);
    }
    printf("|\n");
  }
  printf("-------------------------------------------------------------------\n");
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

  //MPI_Bcast(matrix, maxrow*maxcol, MPI_INT, 0, MPI_COMM_WORLD);
  int alteration;
  // Execution du parallel
  
  // Creation de la fenetre
  if(rank == 0)
  { 
    /*MPI_Alloc_mem(sizeof(matrix) * sizeof(int), MPI_INFO_NULL, matrix);
    MPI_Win_create(matrix, sizeof(matrix) * sizeof(int), sizeof(int),
                 MPI_INFO_NULL, MPI_COMM_WORLD, &win);*/
      printf("Matrix size of %d\n",(int)(sizeof(matrix[0])*sizeof(matrix)));
      MPI_Win_allocate(sizeof(matrix)*sizeof(matrix), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, matrix, &win);
      
  }
  else
  {
    MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    MPI_Win_fence(0, win); 
  }
  
  MPI_Win_fence(MPI_MODE_NOPRECEDE,win);

  // Debut des alterations
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
             
            //MPI_Put(&(matrix[row][col]), 1, MPI_INT, 0, col+maxcol*row, 1, MPI_INT, win);
            
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
  
  if(rank == 1)
  {
    int x, y;
    x = 0;
    y = 8;
    //MPI_Get(&(matrix[x][y]), 1, MPI_INT, 0, y+maxcol*x, 1, MPI_INT, win);
    matrix[x][y] = 999;
    printf("What is matrix: %d\n",matrix[x][y]);
    //MPI_Put(7, 1, MPI_INT, 0, rank, 1, MPI_INT, matrix[0][0]);
    
    MPI_Put(&(matrix[x][y]), 1, MPI_INT, 0, y+x*sizeof(matrix), 1, MPI_INT, win);
    
  }
  MPI_Win_fence(0,win);
  MPI_Win_fence(MPI_MODE_NOSUCCEED,win);

  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == 0)
  {
    //MPI_Get(&(matrix[1][0]), 0, MPI_INT, 1, 0, 0, MPI_INT, win);
    printMatrix();
  }

  //Fin du programme afficher le temps
  MPI_Win_free(&win);
  MPI_Free_mem(matrix);
  MPI_Finalize();
  return 0;
}



