#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include <sys/time.h>

double *matrix, *newMatrix;
int maxrow, maxcol, deltat;
double h, td;
MPI_Win win;

void spinWait(int milliseconds) 
{ 
 struct timeval startTime; 
 struct timeval endTime; 
 
 gettimeofday(&startTime, NULL); 
 
 do 
 { 
 gettimeofday(&endTime, NULL); 
 } while ((endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_usec - startTime.tv_usec) 
< milliseconds * 1000); 
 
 return; 
} 

void printMatrix()
{
  int row, col;
  printf("-------------------------------------------------------------------\n");
  for(row = maxrow-1; row >= 0; row--)
  {
    printf("|");
    for(col = 0; col < maxcol; col++)
    {
      printf("%.3f\t",matrix[row*maxcol +col]);
    }
    printf("|\n");
  }
  printf("-------------------------------------------------------------------\n");
}

// For performance reasons we will use no functions except the print
int main (int argc, char* argv[])
{
	int rank,size, row, col, 
		currentRow, prevRow, nextRow,
		matrixSize;
	MPI_Init (&argc, &argv);      /* starts MPI */
  	MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  	MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */								
	//Initialisation des paramètres
	if(argc != 6)
	{
		printf("Please insert the right number of parameters. You only have %d params.\n", argc);
		return 0;
	}
	maxrow = atoi(argv[1]);
	maxcol = atoi(argv[2]);
	deltat = atoi(argv[3]);
	td = atof(argv[4]);
	h = atof(argv[5]);
	matrixSize = maxrow*maxcol;

	// Initialise the matrix
	matrix = (double*)calloc(matrixSize,sizeof(double));
	for(row = 0; row < maxrow; row++)
	{
		currentRow = row*maxcol;
		for(col = 0; col < maxcol; col++)
		{
			matrix[currentRow+col] = row*(maxrow - row-1) * col*(maxcol - col-1);
		}
	}
	newMatrix = (double*)calloc(matrixSize,sizeof(double));
	memcpy(newMatrix, matrix, matrixSize * sizeof(double));

	// Create window
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

	// Start time
	double timeStart, timeEnd, Texec;
	struct timeval tp;
	gettimeofday (&tp, NULL); // Debut du chronometre
	timeStart = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;

	// Start the alteration
	double tdh2 = (double)(td/(h*h));
	for(row = 1; row < maxrow-1; row++)
	{
		prevRow = (row-1)*maxcol;
		currentRow = row*maxcol;
		nextRow = (row+1)*maxcol;
		for(col = 1; col < maxcol-1; col++)
		{
			newMatrix[currentRow+col] = (1.0-tdh2/4.0)*matrix[currentRow+col] + tdh2 * (matrix[prevRow+col] + matrix[nextRow+col] + matrix[currentRow+col-1] + matrix[currentRow+col+1]);
		}
	}
	memcpy(matrix, newMatrix, matrixSize * sizeof(double));

	//spinWait(50);

	//End time
	gettimeofday (&tp, NULL); // Fin du chronometre
	timeEnd = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;
	Texec = timeEnd - timeStart; //Temps d'execution en secondes
	//printf("Time Executed : %f\n",Texec);
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0)
	{
		printMatrix();
	}

	free(matrix);
	free(newMatrix);
	//MPI_Free_mem(matrix);
	//MPI_Free_mem(newMatrix);
  	MPI_Win_free(&win);
	
	return 0;
}