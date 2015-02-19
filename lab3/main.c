#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include <sys/time.h>

double *matrix, *newMatrix;
int maxrow, maxcol, deltat;
double h, td;
MPI_Win win, newwin;

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
      printf("%.2f\t",matrix[row*maxcol +col]);
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
	  	MPI_Win_create(newMatrix,maxcol*maxrow,sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &newwin);
	}
	else
	{
		MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
		MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &newwin);
		MPI_Win_fence(0, win); 
	}
	MPI_Win_fence(MPI_MODE_NOPRECEDE,win);

	// Declare and calculate variables for process separation
	int newCurrRow, newCurrCol, newMaxRow, newMaxCol, alteration, innerMatrixSize;
	innerMatrixSize = (maxrow-2)*(maxcol-2);
	double calculatedMax = innerMatrixSize/size;
	newCurrRow = (rank) *(calculatedMax / (maxcol-2));
	newCurrCol = rank*calculatedMax-newCurrRow*(maxcol-2)+1;
	newMaxRow = (rank+1) *(calculatedMax / (maxcol-2));
	newMaxCol = (rank+1) *calculatedMax-newMaxRow*(maxcol-2)+1;
	newCurrRow++;
	newMaxRow++;
	printf("From Row: %d Col: %d To Row: %d Col: %d\n", newCurrRow, newCurrCol, newMaxRow, newMaxCol);

	// Start time
	double timeStart, timeEnd, Texec;
	struct timeval tp;
	gettimeofday (&tp, NULL); // Debut du chronometre
	timeStart = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;

	/*
		We will have two windows for each matrix. The first will be for the MPI_Get(old) and the second MPI_Put(new).
		Once the calculations for a single frame is done we insert a barrier and copy the informations of the newMatrix to the old in RANK 0.
		Refresh until done the alterations
	*/
	
	double tdh2 = (double)(td/(h*h));
	// Start the alteration for RANK == 0
	if(rank == 0)
	{
		for(alteration = 0; alteration < deltat; alteration++)
		{
			for(row = newCurrRow; row < newMaxRow-1; row++)
			{
				prevRow = (row-1)*maxcol;
				currentRow = row*maxcol;
				nextRow = (row+1)*maxcol;
				for(col = newCurrCol; col < newMaxCol; col++)
				{
					//spinWait(50);
					newMatrix[currentRow+col] = (1.0-tdh2*0.25)*matrix[currentRow+col] + tdh2 * (matrix[prevRow+col] + matrix[nextRow+col] + matrix[currentRow+col-1] + matrix[currentRow+col+1]);
				}
			}
			MPI_Barrier(MPI_COMM_WORLD);
			memcpy(matrix, newMatrix, matrixSize * sizeof(double));
			MPI_Barrier(MPI_COMM_WORLD);
		}
	}
	else
	{	// Start the alteration for RANK != 0
		for(alteration = 0; alteration < deltat; alteration++)
		{
			for(row = newCurrRow; row < newMaxRow-1; row++)
			{
				prevRow = (row-1)*maxcol;
				currentRow = row*maxcol;
				nextRow = (row+1)*maxcol;
				for(col = newCurrCol; col < newMaxCol-1; col++)
				{
					//spinWait(50);
					newMatrix[currentRow+col] = (1.0-tdh2*0.25)*matrix[currentRow+col] + tdh2 * (matrix[prevRow+col] + matrix[nextRow+col] + matrix[currentRow+col-1] + matrix[currentRow+col+1]);
					//MPI_Put(&(newMatrix[currentRow +col]), 1, MPI_DOUBLE, 0, col+row*maxcol, 1, MPI_DOUBLE, newwin);
				}
			}
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);
		}
	}


	//End time
	gettimeofday (&tp, NULL); // Fin du chronometre
	timeEnd = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;
	Texec = timeEnd - timeStart; //Temps d'execution en secondes
	printf("Time Executed : %f\n",Texec);
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