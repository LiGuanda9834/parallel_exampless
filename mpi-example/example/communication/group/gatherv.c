/**
 * 收集函数示例, 根进程从所有进程收集不同数目的数据.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int i, myrank, nprocs, root, send_count, recv_buffer_size;
  int *send_buffer;
  int *recv_buffer;
  int *recv_counts;
  int *displacements;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  /**
   * 进程号为myrank的进程提供的数据个数为myrank+1.
   */
  send_count = myrank + 1;
  send_buffer = (int *) calloc (send_count, sizeof (int));
  for (i = 0; i < send_count; i++)
    {
      send_buffer[i] = myrank;
      fprintf (stderr, "before gatherv, myrank %d: send_buffer[%d] = %d\n",
               myrank, i, send_buffer[i]);
    }

  // 指定根进程.
  root = 2;

  if (myrank == root)
    {
      //根进程开辟接收缓冲区并初始化, 大于实际要接收的数据数目.
      recv_buffer_size = nprocs;
      recv_buffer = (int *) calloc (nprocs * recv_buffer_size, sizeof (int));
      for (i = 0; i < nprocs * recv_buffer_size; i++)
        {
          recv_buffer[i] = -1;
        }
      //根进程创建一个数组, recv_counts[i]表示从进程i接收的数据数目.
      recv_counts = (int *) calloc (nprocs, sizeof (int));

      //根进程创建一个数组, displacements[i]表示从进程i接收到的数据存放在
      //以recv_buffer+displacements[i]为起始地址的内存空间中. 
      displacements = (int *) calloc (nprocs, sizeof (int));
      for (i = 0; i < nprocs; i++)
	{
	  recv_counts[i] = i + 1;
	  displacements[i] = i * nprocs;
	}

      // 也可通过探测函数动态检测接收的消息大小.
    }

  MPI_Gatherv (send_buffer, send_count, MPI_INT,
	       recv_buffer, recv_counts, displacements, MPI_INT,
	       root, MPI_COMM_WORLD);

  sleep(2);
  MPI_Barrier(MPI_COMM_WORLD);

  if (myrank == root)
    {
      fprintf (stderr, "\n\n");
      for (i = 0; i < nprocs * recv_buffer_size; i++)
        {
	  printf ("after gatherv, myrank: %d, recv_buffer[%d] = %d\n",
                  myrank, i, recv_buffer[i]);
        }
      free (recv_buffer);
      free (recv_counts);
      free (displacements);
    }
  free (send_buffer);

  MPI_Finalize ();
  return 0;
}
