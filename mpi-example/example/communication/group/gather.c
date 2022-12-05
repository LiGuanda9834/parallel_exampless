/**
 * 收集函数示例, 根进程从所有进程获取相同数目的数据.
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 0

int
main (int argc, char *argv[])
{
  int i, myrank, nprocs, root;
  int *send_buffer;
  int *recv_buffer;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  // 所有进程都提供send_buffer.
  send_buffer = (int *) calloc (N, sizeof (int));

  for (i = 0; i < N; i++)
    send_buffer[i] = myrank + i;
 
  // 指定进程0为根进程.
  root = 0;

  if (myrank == root)
    {
      // 只有根进程需要recv_buffer.
      recv_buffer = (int *) calloc (nprocs * N, sizeof (int));
      for (i = 0; i < nprocs * N; i++)
	printf ("before gathering, recv_buffer[%d] = %d\n", i, recv_buffer[i]);
    }
  printf ("\n");

  MPI_Gather (send_buffer, N, MPI_INT,
	      recv_buffer, N, MPI_INT, root, MPI_COMM_WORLD);

  if (myrank == root)
    {
      for (i = 0; i < nprocs * N; i++)
	printf ("after gathering, recv_buffer[%d] = %d\n", i, recv_buffer[i]);
      free (recv_buffer);
    }
  free (send_buffer);
  MPI_Finalize ();
  return 0;
}
