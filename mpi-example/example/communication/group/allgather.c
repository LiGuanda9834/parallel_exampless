 
/**
 * 全收集函数示例, 每个进程都拥有收集结果,
 * 相当于以每个进程作为根进程对同样的数据执行一次收集操作.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1

int
main (int argc, char *argv[])
{
  int i, myrank, nprocs;
  int *send_buffer;
  int *recv_buffer;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  send_buffer = (int *) calloc (N, sizeof (int));

  for (i = 0; i < N; i++)
    send_buffer[i] = myrank;
 
  // 在每个进程上, recv_buffer的大小为所有进程的send_buffer大小之和.
  recv_buffer = (int *) calloc (nprocs * N, sizeof (int));
 
  MPI_Allgather (send_buffer, N, MPI_INT,
		 recv_buffer, N, MPI_INT, MPI_COMM_WORLD);

  for (i = 0; i < nprocs * N; i++)
    fprintf (stderr, "myrank = %d, recv_buffer[%d] = %d\n", myrank, i, recv_buffer[i]);

  fprintf (stderr, "\n");
  free (recv_buffer);
  free (send_buffer);
  MPI_Finalize ();
  return 0;
}
