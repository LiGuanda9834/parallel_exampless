 
/**
 * 归约散发函数示例.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int i, N, myrank, nprocs;
  int *send_buffer;
  int *recv_buffer;
  int *recv_count;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  N = 3;

  // 每个进程都提供send_buffer.
  send_buffer = (int *) calloc (N * nprocs, sizeof (int));

  // 进程i保存的"收集结果"的一部分，个数为recv_count[i].
  recv_count = (int *) calloc (nprocs, sizeof (int));
  for (i = 0; i < nprocs; i++)
    {
      recv_count[i] = N;
      recv_buffer = (int *) calloc (recv_count[i], sizeof (int));
    }

  for (i = 0; i < N * nprocs; i++)
    {
      send_buffer[i] = myrank;
    }

  MPI_Reduce_scatter (send_buffer, recv_buffer, recv_count,
		      MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  for (i = 0; i < recv_count[myrank]; i++)
    printf ("myrank = %d, recv_buffer[%d] = %d\n", myrank, i, recv_buffer[i]);

  free (send_buffer);
  free (recv_buffer);
  free (recv_count);  

  MPI_Finalize ();
  return 0;
}
