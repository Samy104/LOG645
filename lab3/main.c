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
      printf("%.2f\t",newMatrix[row*maxcol +col]);
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
	}
	MPI_Win_fence(MPI_MODE_NOPRECEDE,win);
	MPI_Win_fence(MPI_MODE_NOPRECEDE,newwin);

	// Declare and calculate variables for the separation process
	int newCurrRow, newCurrCol, newMaxRow, newMaxCol, alteration, innerMatrixSize, 
	surroundingLength, surroundingStart, 
	limitedRow, limitedCol, colminun,
	distribution, isExtended;

	// Variables réutilisés
	limitedRow = maxrow-2;
	limitedCol = maxcol-2;
	innerMatrixSize = limitedCol*limitedRow;
	colminun = maxcol-1;
	double calculatedMax = innerMatrixSize/size;
	// Calculations des limites
	newCurrRow = (rank) *(calculatedMax / limitedCol);
	newCurrCol = rank*calculatedMax-newCurrRow*limitedCol;
	newMaxRow = (rank+1) *(calculatedMax / limitedCol);
	newMaxCol = (rank+1) *calculatedMax-newMaxRow*limitedCol;
	// Distribution des pixels/elements qui ne sont pas couverts
	distribution = (innerMatrixSize%size != 0) ? rank/(size/(innerMatrixSize%size)) +1 : 1;//(rank+1)/(size/(innerMatrixSize%size)) + 1;
	newCurrCol+= distribution;
	newMaxCol+= distribution;
	newCurrRow++;
	newMaxRow++;
	if(newCurrCol > colminun)
	{
		newCurrRow++;
		newCurrCol = newCurrCol-colminun+1;
	}
	if(newMaxCol > colminun)
	{
		newMaxRow++;
		newMaxCol = newMaxCol-colminun+1;
	}
	if(rank == 3)
	printf("From Row: %d Col: %d To Row: %d Col: %d\n", newCurrRow, newCurrCol, newMaxRow, newMaxCol);

	// Conversion from row/col to 1D Matrix
	int startMatrix, endMatrix, elementMatrix, lastElement, sizeExpand, elementsToPush;
	startMatrix = newCurrRow*maxcol+newCurrCol;
	endMatrix = newMaxRow*maxcol+newMaxCol;
	lastElement = (maxrow-1)*maxcol+maxcol-1;
	if(endMatrix > lastElement)
	{
		endMatrix = lastElement;
	}
	// Put variables
	sizeExpand = newMaxRow - newCurrRow;
	isExtended = 0;
	if(innerMatrixSize%size != 0)
	{
		isExtended = (rank/(size/(innerMatrixSize%size)) != (rank+1)/(size/(innerMatrixSize%size))) ? 1 : 0;
	}
	elementsToPush = calculatedMax + sizeExpand*2 + isExtended;

	// Surrounding length and max for the MPI_Get
	surroundingStart = newCurrRow-1;
	surroundingLength = (newMaxRow-newCurrRow+3)*maxcol;

	// Variables for the calculations
	double tdh2 = (td/(h*h));
	double invtdh2 = (1.0-tdh2*4.0);

	// Start time
	double timeStart, timeEnd, Texec;
	struct timeval tp;
	gettimeofday (&tp, NULL); // Debut du chronometre
	timeStart = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;
	
	// Start the alteration for RANK == 0
	if(rank == 0)
	{
		for(alteration = 1; alteration < deltat; alteration++)
		{
			// Initialize elements for alteration
			elementMatrix = startMatrix;
			col = newCurrCol;
			// Start calulations
			for(row = newCurrRow; row <= newMaxRow; row++)
			{
				prevRow = (row-1)*maxcol;
				currentRow = row*maxcol;
				nextRow = (row+1)*maxcol;

				while(col < colminun && elementMatrix <= endMatrix)
				{
					//spinWait(50);
					newMatrix[elementMatrix] = invtdh2*matrix[currentRow+col] + tdh2 * (matrix[prevRow+col] + matrix[nextRow+col] + matrix[currentRow+col-1] + matrix[currentRow+col+1]);
					elementMatrix++;
					col++;
				}
				elementMatrix+=2;
				col=1;
			}
			MPI_Barrier(MPI_COMM_WORLD);
			memcpy(matrix, newMatrix, matrixSize * sizeof(double));
			MPI_Barrier(MPI_COMM_WORLD);
		}
	}
	else
	{	// Start the alteration for RANK != 0
		for(alteration = 1; alteration < deltat; alteration++)
		{
			MPI_Win_lock(MPI_LOCK_SHARED,0,0,win);
			//MPI_Get(&(matrix[surroundingStart]), surroundingLength, MPI_DOUBLE, 0, surroundingStart, surroundingLength,MPI_DOUBLE, win);
			MPI_Get(&(matrix[0]), matrixSize, MPI_DOUBLE, 0, 0, matrixSize,MPI_DOUBLE, win);
			MPI_Win_unlock(0,win);
			// Initialize elements for alteration
			elementMatrix = startMatrix;
			col = newCurrCol;
			// Start calulations
			for(row = newCurrRow; row <= newMaxRow; row++)
			{
				prevRow = (row-1)*maxcol;
				currentRow = row*maxcol;
				nextRow = (row+1)*maxcol;
				while(col < colminun && elementMatrix <= endMatrix)
				{
					//spinWait(50);
					newMatrix[elementMatrix] = invtdh2*matrix[currentRow+col] + tdh2 * (matrix[prevRow+col] + matrix[nextRow+col] + matrix[currentRow+col-1] + matrix[currentRow+col+1]);
					elementMatrix++;
					col++;
				}
				elementMatrix+=2;
				col=1;
			}
			MPI_Win_lock(MPI_LOCK_SHARED,0,0,newwin);
			MPI_Put(&(newMatrix[startMatrix]), elementsToPush, MPI_DOUBLE, 0, startMatrix, elementsToPush, MPI_DOUBLE, newwin);
			MPI_Win_unlock(0,newwin);
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);
		}
	}

	//End time
	gettimeofday (&tp, NULL); // Fin du chronometre
	timeEnd = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;
	Texec = timeEnd - timeStart; //Temps d'execution en secondes
	printf("Process %d Executed in : %f\n",rank,Texec);
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0)
	{
		printMatrix();
	}
	MPI_Barrier(MPI_COMM_WORLD);

	free(matrix);
	free(newMatrix);
	//MPI_Free_mem(matrix);
	//MPI_Free_mem(newMatrix);
  	//MPI_Win_free(&win);
  	//MPI_Win_free(&newwin);
	MPI_Finalize();
	return 0;
}