 
/**
 * 从通信操作返回的状态量(MPI_Status类型)可得到什么信息.
 */

#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>
#define N 3

int
main (int argc, char *argv[])
{
  int myrank, nprocs, i, count;
  int *send_buf;
  int *recv_buf;
  MPI_Status *status;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  status = (MPI_Status *) calloc (nprocs, sizeof(MPI_Status));

  if (myrank != 0)
    {
      send_buf = (int *) calloc ((myrank + 1), sizeof (int));

      for (i = 0; i < N; i++)
	{
          // 向进程0发送标签为i的消息。
	  MPI_Send (send_buf, myrank+1, MPI_INT, 0, i, MPI_COMM_WORLD);
	}
      free (send_buf);
    }
  else
    {
      // recv_buf必须足够大。
      recv_buf = (int *) calloc (nprocs, sizeof (int));
      for (i = 0; i < N * (nprocs - 1); i++)
	{
          // 从MPI_ANY_SOURCE接受标签为MPI_ANY_TAG的消息。
	  MPI_Recv (recv_buf, nprocs, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
		    MPI_COMM_WORLD, &status[0]);

          // 从status获取接受到的整型数的个数。
	  MPI_Get_count (&status[0], MPI_INT, &count);

          // 从status获取消息的实际来源和实际标签。
	  fprintf (stderr, "myrank: %d, receive %d numbers from %d with tag %d\n",
                   myrank, count, status[0].MPI_SOURCE, status[0].MPI_TAG);
	}
      free (recv_buf);
    }
  MPI_Finalize ();
  return 0;
}
