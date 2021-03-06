#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
int matrix[8][8] = {{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3},{3,3,3,3,3,3,3,3}};
int operation = 1;
int maxrow = sizeof(matrix)/sizeof(matrix[0]),
    maxcol = sizeof(matrix[0])/sizeof(int);

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
  printf("------------------\n");
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
	operation = atoi(argv[1]);
	setInitialVal(atoi(argv[2]));
  	int alteration;
  	clock_t timestart, timeend;
  	timestart = clock();
	// Execution du séquentiel
    for(alteration = 1;alteration < atoi(argv[3]); alteration++)
    {
      int row, col;
  		//First operation
		if(operation == 1)
		{
			for(row = 0; row < maxrow; row++)
			{
				for(col = 0; col < maxcol; col++)
				{
          usleep(1000);
					matrix[row][col] = matrix[row][col] + (row + col) * alteration;
				}
			}
		} //Second operation
		else
		{
			for(row = 0; row < maxrow; row++)
			{
				for(col = 0; col < maxcol; col++)
				{ 
          usleep(1000);
					matrix[row][col] = (col == 0) ? matrix[row][col] + row*alteration : matrix[row][col] + matrix[row][col-1]*alteration;
				}
			}
		}
      
    } 
    timeend = clock();
    printf("Sequential process finished the work in %f ms\n", (((float)timeend-(float)timestart)/ 1000000.0F)*1000);
    printMatrix();

    return 0;
}