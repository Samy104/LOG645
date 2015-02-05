#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <sys/time.h>

double matrix[10][10];

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
	int maxrow = sizeof(matrix[0])/sizeof(matrix[0][0]),
		maxcol = sizeof(matrix[0])/sizeof(double);

	printf("---------------------------------------------------------------------------------\n");
	for(row = 0; row < maxrow; row++)
	{
		printf("|");
		for(col = 0; col < maxcol; col++)
		{
		  printf("%.0f\t",matrix[row][col]);
		}
		printf("|\n");
	}
	printf("---------------------------------------------------------------------------------\n");
}

int main (int argc, char* argv[])
{
	int thread_count = 10;
	int operation = atoi(argv[1]);
	int valeurInit = atoi(argv[2]);
	int alteration = atoi(argv[3]);

	int maxrow = sizeof(matrix[0])/sizeof(matrix[0][0]),
		maxcol = sizeof(matrix[0])/sizeof(double);
	
	int r,c;
	// Initialise the matrix
	for(r = 0; r < maxrow; r++)
	{
		for(c = 0; c < maxcol; c++)
		{
			matrix[r][c] = valeurInit;
		}
	}
	//TIME
	double timeStart, timeEnd, Texec;
	struct timeval tp;
	gettimeofday (&tp, NULL); // Debut du chronometre
	timeStart = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;

	int row, col, alt;
	// Beggining of calculations
	if(operation == 1)
	{
		#pragma omp parallel for private(col, alt)
		for(row = 0; row < 10; row++)
		{
			for(alt = 0; alt < alteration; alt++)
			{
				for(col = 9; col >= 0; col--)
				{
					spinWait(50);
					matrix[row][col] = matrix[row][col] +row + col;
				}			
			}
		}	
	}
	else if(operation == 2)
	{
		#pragma omp parallel for private(col, alt)
		for(row = 0; row < 10; row++)
		{
			for(alt = 0; alt < alteration; alt++)
			{
				for(col = 9; col >= 0; col--)
				{
					spinWait(50);
					if(col < 9)
					{
						matrix[row][col] = matrix[row][col] + matrix[row][col+1];
					}
					else if(col == 9)
					{
						matrix[row][col] = matrix[row][col] + row;
					}
				}			
			}
		}
	}
	gettimeofday (&tp, NULL); // Fin du chronometre
	timeEnd = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;
	Texec = timeEnd - timeStart; //Temps d'execution en secondes
	printf("The parallel is done in %f ms.\n", Texec*1000);
	printMatrix();
	return 0;
}