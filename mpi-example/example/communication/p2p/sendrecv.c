/**
 * 函数MPI_Sendrecv可解除死锁.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 65536

int
main (int argc, char *argv[])
{
  int myrank, dest;
  int my_int[N], get_int[N];
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  my_int[0] = (myrank + 1) * (myrank + 1);
  dest = (myrank == 0) ? (1) : (0);

#if 0
  if (myrank == 0)
    {
      MPI_Send (my_int, N, MPI_INT, dest, 99, MPI_COMM_WORLD);
      MPI_Recv (get_int, N, MPI_INT, dest, 99, MPI_COMM_WORLD, &status);
    }
  else
    {
      MPI_Recv (get_int, N, MPI_INT, dest, 99, MPI_COMM_WORLD, &status);
      MPI_Send (my_int, N, MPI_INT, dest, 99, MPI_COMM_WORLD);
    }
#endif
  MPI_Sendrecv (my_int, N, MPI_INT, dest, 99,
		get_int, N, MPI_INT, dest, 99, MPI_COMM_WORLD, &status);

  printf ("myrank %d: my_int = %d, \tget_int = %d\n", myrank,
	  my_int[0], get_int[0]);

  MPI_Finalize ();
  return 0;
}
