#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

double *matrix, *newMatrix;
int maxrow, maxcol, deltat;
double h, td;

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

	// Declare and calculate variables for the separation process
	int newCurrRow, newCurrCol, newMaxRow, newMaxCol, alteration, innerMatrixSize, 
	surroundingLength, surroundingStart, 
	limitedRow, limitedCol, colminun,
	distribution, isExtended;

	// Variables for the calculations
	double tdh2 = (td/(h*h));
	double invtdh2 = (1.0-tdh2*4.0);

	// Start time
	double timeStart, timeEnd, Texec;
	struct timeval tp;
	gettimeofday (&tp, NULL); // Debut du chronometre
	timeStart = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;
	
	// Start the alteration for RANK == 0
	for(alteration = 0; alteration < deltat; alteration++)
	{
		// Start calulations
		for(row = 1; row < maxrow-1; row++)
		{
			prevRow = (row-1)*maxcol;
			currentRow = row*maxcol;
			nextRow = (row+1)*maxcol;

			for(col = 1; col < maxcol-1; col++)
			{
				spinWait(50);
				newMatrix[currentRow+col] = invtdh2*matrix[currentRow+col] + tdh2 * (matrix[prevRow+col] + matrix[nextRow+col] + matrix[currentRow+col-1] + matrix[currentRow+col+1]);
			}
		}
		memcpy(matrix, newMatrix, matrixSize * sizeof(double));
	}

	//End time
	gettimeofday (&tp, NULL); // Fin du chronometre
	timeEnd = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;
	Texec = timeEnd - timeStart; //Temps d'execution en secondes
	printf("Process Executed in : %f\n",Texec);
	//printMatrix();

	free(matrix);
	free(newMatrix);
	return 0;
}