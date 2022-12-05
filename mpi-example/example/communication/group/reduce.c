 
/**
 * 归约函数示例.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 3

int
main (int argc, char *argv[])
{
  int i, myrank, nprocs, root;
  int send_buffer[N];
  int recv_buffer[N];

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  for (i = 0; i < N; i++)
    {
      send_buffer[i] = myrank;
    }

  // 指定根进程.
  root = 1;

  // recv_bufer仅对归约操作的根进程有效, 根进程不一定为0进程.
  MPI_Reduce (send_buffer, recv_buffer, N, MPI_INT, MPI_SUM, root,
	      MPI_COMM_WORLD);

  if (myrank == root)
    {
      for (i = 0; i < N; i++)
	{
	  printf ("myrank = %d, recv_buffer[%d] = %d\n", myrank, i,
		  recv_buffer[i]);

	}
    }


  MPI_Finalize ();
  return 0;
}
