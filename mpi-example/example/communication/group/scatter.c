 
/**
 * 散发函数示例.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1

int
main (int argc, char *argv[])
{
  int i, myrank, nprocs, root;
  int *send_buffer;
  int *recv_buffer;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
 
  recv_buffer = (int *) calloc (N, sizeof (int));

  // 指定进程0为根进程.
  root = 0;

  if (myrank == root)
    {
      // 根进程提供待散发的缓冲区.
      send_buffer = (int *) calloc (nprocs * N, sizeof (int));
      for (i = 0; i < nprocs * N; i++)
	    {
	        send_buffer[i] = i + 10;
	        printf ("myrank = %d, send_buffer[%d] = %d\n", myrank, i, send_buffer[i]);
	    }
      printf ("\n");
    }

  MPI_Scatter (send_buffer, N, MPI_INT,
	       recv_buffer, N, MPI_INT, root, MPI_COMM_WORLD);

  for (i = 0; i < N; i++)
    {
      printf ("myrank = %d, recv_buffer[%d] = %d\n", myrank, i, recv_buffer[i]);
    }

  free (recv_buffer);
  if (myrank == root)
    free (send_buffer);

  MPI_Finalize ();
  return 0;
}
