#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include <sys/time.h>

double **matrix;
int maxrow, maxcol, deltat;
double h, td;


int main (int argc, char* argv[])
{
	int maxProcess;
	//Initialisation des paramètres
	if(argc != 6)
	{
		printf("Please insert the right number of parameters. You only have %d params.", argc);
		return 0;
	}
	maxrow = atoi(argv[1]);
	maxcol = atoi(argv[2]);
	deltat = atoi(argv[3]);
	maxProcess = atoi(argv[4]);
	td = atoi(argv[5]);
	h = atoi(argv[6]);

	// Start time
	double timeStart, timeEnd, Texec;
	struct timeval tp;
	gettimeofday (&tp, NULL); // Debut du chronometre
	timeStart = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;





	//End time
	gettimeofday (&tp, NULL); // Fin du chronometre
	timeEnd = (double) (tp.tv_sec) + (double) (tp.tv_usec) / 1e6;
	Texec = timeEnd - timeStart; //Temps d'execution en secondes

	return 0;
}