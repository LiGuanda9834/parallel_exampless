 
/**
 * 根进程向每个进程散发不同数目的数据.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int i, myrank, nprocs, recv_count, send_buffer_size, root;
  int *send_buffer;
  int *recv_buffer;
  int *send_counts;
  int *displacements;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  /**
   * 每个进程都为将要接收的数据开辟内存空间.
   * 编号为myrank的进程从根进程接收的数据个数为myrank+1.
   */
  recv_count = myrank + 1;
  recv_buffer = (int *) calloc (recv_count, sizeof (int));

#if 0
  for (i = 0; i < recv_count; i++)
    {
      recv_buffer[i] = -1;
      fprintf (stderr, "myrank %d: recv_buffer[%d] = %d\n", myrank, i,
	       recv_buffer[i]);
    }
#endif
 
  // 指定根进程.
  root = 0;

  if (myrank == root)
    {
      // 根进程开辟发送缓冲区并初始化.
      // 假定发往每个进程的消息为send_buffer_size个整型数.
      send_buffer_size = nprocs;
      send_buffer = (int *) calloc (nprocs * send_buffer_size, sizeof (int));
      for (i = 0; i < nprocs * send_buffer_size; i++)
	send_buffer[i] = i;

      //根进程创建数组send_counts, send_counts[i]表示向进程i发送的数据数目.
      send_counts = (int *) calloc (nprocs, sizeof (int));

      //根进程创建数组displacements, displacements[i]表示向进程i发送的数据在
      //以send_buffer+displacements[i]为起始地址的内存空间中. 
      displacements = (int *) calloc (nprocs, sizeof (int));
      for (i = 0; i < nprocs; i++)
	    {
	  send_counts[i] = i + 1;
	  displacements[i] = i * nprocs;
	    }
    }

  MPI_Scatterv (send_buffer, send_counts, displacements, MPI_INT,
		recv_buffer, recv_count, MPI_INT, root, MPI_COMM_WORLD);

  fprintf (stderr, "\n\n");
  for (i = 0; i < myrank + 1; i++)
    fprintf (stderr, "myrank: %d, recv_buffer[%d] = %d\n", myrank, i, recv_buffer[i]);

  free (recv_buffer);
  if (myrank == root)
    {
      free (send_buffer);
      free (send_counts);
      free (displacements);
    }

  MPI_Finalize ();
  return 0;
}
