/* C Example */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
 /*
mpicc mpitest.c -o hello
mpirun -np 4 ./hello
 */
int main (int argc, char* argv[])
{
  int rank, size, buff, receive;
  MPI_Status status;
 
 // This will return an error and the MPI_COOM_WORLD will be instantiated
  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */

  if(rank == 2)
  {
  	/*#define BUFFSIZE 10000 
	char *buff; 
	MPI_Buffer_attach( malloc(BUFFSIZE), BUFFSIZE); */
	/* a buffer of 10000 bytes can now be used by MPI_Bsend */ 

  	//MPI_Send(a, 0, 1, MPI_DOUBLE,1,19,MPI_COMM_WORLD);
  	//MPI_Recv(a, 0, 1, MPI_DOUBLE,1,19,MPI_COMM_WORLD, &status);
  	buff=333;
  	MPI_Send(&buff, 1, MPI_INT, 3, 123, MPI_COMM_WORLD);
  	buff=111;
  	MPI_Send(&buff, 1, MPI_INT, 1, 123, MPI_COMM_WORLD);
  	buff = 222;
  }
  else if(rank == 3)
  {
  	MPI_Recv(&receive, 1, MPI_INT, 2, 123, MPI_COMM_WORLD, &status);
  	MPI_Send(&receive, 1, MPI_INT, 1, 123, MPI_COMM_WORLD);
  }
  else if(rank == 1)
  {
  	MPI_Recv(&receive, 1, MPI_INT, 2, 123, MPI_COMM_WORLD, &status);
  }
  int send_count = 1;
  MPI_Bcast(&buff, send_count, MPI_INT,2,MPI_COMM_WORLD);

  printf( "Hello world from process %d of %d and buffer is %d received is %d\n", rank, size ,buff, receive);
  //MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm)

  

  MPI_Finalize();
  return 0;
}