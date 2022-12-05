/**
 * 多个进程向同一进程发送消息时，不同的消息到达目的进程的顺序不确定。
 * 此时，可以通过不同的标签来识别来自不同进程的消息.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1

int
main (int argc, char *argv[])
{
  int myrank, dest;
  int my_int[N], int_from_1[N], int_from_2[N];
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  my_int[0] = myrank * 111;

  if (myrank == 1)
    {
      dest = 0;
      MPI_Send (my_int, N, MPI_INT, dest, 99, MPI_COMM_WORLD);
    }
  else if (myrank == 2)
    {
      dest = 0;
      MPI_Send (my_int, N, MPI_INT, dest, 99, MPI_COMM_WORLD);
    }
  else
    {
      MPI_Recv (int_from_1, N, MPI_INT, 
                MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);
      MPI_Recv (int_from_2, N, MPI_INT, 
                MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);
      printf ("myrank %d: int_from_1 = %d, \tint_from_2 = %d\n", myrank,
	      int_from_1[0], int_from_2[0]);
    }

  MPI_Finalize ();
  return 0;
}
