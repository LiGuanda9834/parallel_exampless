 
/**
 * 简单的"发送", "接收"匹配示例. 可导致死锁.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 32

int
main (int argc, char *argv[])
{
  int myrank, dest;
  //int my_int[N], get_int[N];
  //int my_aa[N], get_aa[N];
  int *my_int = (int *) calloc (N, sizeof (int));
  int *get_int = (int *) calloc (N, sizeof (int));
  //int *my_aa = (int *) calloc (N, sizeof (int));
  //int *get_aa = (int *) calloc (N, sizeof (int));
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  my_int[0] = (myrank + 1) * (myrank + 1);
  dest = (myrank == 0) ? (1) : (0);

  if (myrank == 0)
    {
  //    MPI_Send (my_aa, N, MPI_INT, dest, 99, MPI_COMM_WORLD);
      MPI_Send (my_int, N, MPI_INT, dest, 9, MPI_COMM_WORLD);
      MPI_Recv (get_int, N, MPI_INT, dest, 99, MPI_COMM_WORLD, &status);
    }
  else
    {
      MPI_Send (my_int, N, MPI_INT, dest, 99, MPI_COMM_WORLD);
      MPI_Recv (get_int, N, MPI_INT, dest, 9, MPI_COMM_WORLD, &status);
  //    MPI_Recv (get_aa, N, MPI_INT, dest, 99, MPI_COMM_WORLD, &status);
    }

  fprintf (stderr, "myrank %d: my_int = %d, \tget_int = %d\n", myrank,
	  my_int[0], get_int[0]);

  MPI_Finalize ();
  return 0;
}
