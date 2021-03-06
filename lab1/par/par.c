#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

double *matrix;
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
      printf("%.0f\t",matrix[row*maxcol +col]);
    }
    printf("|\n");
  }
  printf("-------------------------------------------------------------------\n");
}

void setInitialVal(int value)
{
  int row, col;
  matrix = (double*)calloc(maxrow*maxcol,sizeof(double));
  for(row = 0; row < maxrow; row++)
  {
    for(col = 0; col < maxcol; col++)
    {
      matrix[row*maxcol +col] = (double)value;
    }
  }
}

int main (int argc, char* argv[])
{
  int rank, size, alteration;
  double timestart, timeend;

 // This will return an error and the MPI_COOM_WORLD will be instantiated
  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
  operation = atoi(argv[1]);
  setInitialVal(atoi(argv[2]));

  // Creation de la fenetre
  if(rank == 0)
  { 
      printf("Matrix size of %d\n",(int)(maxcol*maxrow));
      MPI_Win_create(matrix,maxcol*maxrow,sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
  }
  else
  {
    MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    MPI_Win_fence(0, win); 
  }
  
  MPI_Win_fence(MPI_MODE_NOPRECEDE,win);
  // Debut des alterations
  timestart = MPI_Wtime();
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
          usleep(1000);
          if(currentProcessor == rank && rank == 0)
          {
            matrix[row*maxcol +col] = matrix[row*maxcol +col] + (row + col) * alteration;
          }
          else if(currentProcessor == rank)
          {
            matrix[row*maxcol +col] = matrix[row*maxcol +col] + (row + col) * alteration;
            MPI_Win_lock(MPI_LOCK_SHARED,0,0,win);
            MPI_Put(&(matrix[row*maxcol +col]), 1, MPI_DOUBLE, 0, col+row*maxcol, 1, MPI_DOUBLE, win); 
            MPI_Win_unlock(0,win);
          }
          
          currentProcessor = (currentProcessor < size-1) ? currentProcessor+1 : 0;
        }
      }
    } 
    else //Second operation
    {
      for(row = 0; row < maxrow; row++)
      {
        for(col = 0; col < maxcol; col++)
        {
          usleep(1000);
          if(currentProcessor == rank && rank == 0)
          {
            matrix[row*maxcol +col] = (col == 0) ? matrix[row*maxcol +col] + row*alteration : matrix[row*maxcol +col] + matrix[row*maxcol +col-1]*alteration;
          }
          else if(currentProcessor == rank)
          {
            matrix[row*maxcol +col] = (col == 0) ? matrix[row*maxcol +col] + row*alteration : matrix[row*maxcol +col] + matrix[row*maxcol +col-1]*alteration;
            MPI_Win_lock(MPI_LOCK_SHARED,0,0,win);
            MPI_Put(&(matrix[row*maxcol +col]), 1, MPI_DOUBLE, 0, col+row*maxcol, 1, MPI_DOUBLE, win); 
            MPI_Win_unlock(0,win);
          }
        }
        currentProcessor = (currentProcessor < size-1) ? currentProcessor+1 : 0;
      }

    }

  }
  timeend = MPI_Wtime();
  printf("Process %d finished the work in %f ms\n", rank, (timeend-timestart)*1000);

  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == 0)
  {
    //sleep(1);
    printMatrix();
  }

  MPI_Free_mem(matrix);
  MPI_Win_free(&win);
  MPI_Finalize();
  return 0;
}



