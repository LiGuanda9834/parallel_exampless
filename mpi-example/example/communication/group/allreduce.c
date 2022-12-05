/**
 * 全归约函数示例. 所有进程拥有归约结果.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 3

int
main (int argc, char *argv[])
{
  int i, myrank, nprocs;
  int send_buffer[N];
  int recv_buffer[N];

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  for (i = 0; i < N; i++)
    {
      send_buffer[i] = myrank;
    }

  MPI_Allreduce (send_buffer, recv_buffer, N, MPI_INT, MPI_MAX,
		 MPI_COMM_WORLD);

  for (i = 0; i < N; i++)
    {
      printf ("myrank = %d, recv_buffer[%d] = %d\n", myrank, i,
	      recv_buffer[i]);

    }

  MPI_Finalize ();
  return 0;
}
