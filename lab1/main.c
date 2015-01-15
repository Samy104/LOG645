#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
int matrix[8][8] = {{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3}};
int operation = 1;
int maxrow = sizeof(matrix)/sizeof(matrix[0]),
    maxcol = sizeof(matrix[0])/sizeof(int);

void printMatrix()
{
  int row, col;
  for(row = 0; row < maxrow; row++)
  {
    for(col = 0; col < maxcol; col++)
    {
      printf("%d ",matrix[row][col]);
    }
    printf("\n");
  }
}

void sectionSequentielle(int k)
{
  int row, col;
  //First operation
  if(operation == 1)
  {
    for(row = 0; row < maxrow; row++)
    {
      for(col = 0; col < maxcol; col++)
      {
        matrix[row][col] = matrix[row][col] + (row + col) * k;
      }
    }
  } //Second operation
  else
  {
    for(row = 0; row < maxrow; row++)
    {
      for(col = 0; col < maxcol; col++)
      { 
        matrix[row][col] = (col == 0) ? matrix[row][col] + row*k : matrix[row][col] + matrix[row][col-1]*k;
      }
    }
  }
}

void sectionParallele(int k) 
{
  int currentProcessor = 0;
  int row, col;
  // Create a communcation per pixel

  // Attach three processes

  //First operation
  if(operation == 1)
  {
    for(row = 0; row < maxrow; row++)
    {
      for(col = 0; col < maxcol; col++)
      {
        matrix[row][col] = matrix[row][col] + (row + col) * k;
        currentProcessor = (currentProcessor >= 24) ? 0 : currentProcessor++;
      }
    }
  }
  //Second operation
  printMatrix();

}

void setInitialVal(int value)
{
  int row, col;

  for(row = 0; row < maxrow; row++)
  {
    for(col = 0; col < maxcol; col++)
    {
      matrix[row][col] = value;
    }
  }
}

int main (int argc, char* argv[])
{
  int rank, size, buff, receive;
 
 // This will return an error and the MPI_COOM_WORLD will be instantiated
  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
  operation = atoi(argv[1]);

  setInitialVal(atoi(argv[2]));

  if(size == 1)
  {
    int alteration = 1; 
    for(;alteration < atoi(argv[3]); alteration++)
    {
      sectionSequentielle(alteration);
      printMatrix();
    } 
  }
  else
  {
    sectionParallele(1);
  }
  //printf("The arg is %d\n", operation);
  MPI_Finalize();
  return 0;
}



