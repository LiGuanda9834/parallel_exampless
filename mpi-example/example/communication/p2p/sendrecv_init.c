/**
 * 创建持久通信通道.
 */

#include <stdio.h>
#include <mpi.h>

//#define N 65536
#define N 1
#define M 3
int
main (int argc, char *argv[])
{
  int myrank, dest, i;
  int my_int[N], get_int[N];
  int flag;
  MPI_Request request;
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  dest = (myrank == 0) ? (1) : (0);
  // my_int[0] = (myrank + 1) * (myrank + 1);

  // 建立持久通信通道.
  if (myrank == 0)
    {
      MPI_Send_init (my_int, N, MPI_INT, dest, 99, MPI_COMM_WORLD, &request);
    }
  else
    {
      MPI_Recv_init (get_int, N, MPI_INT, dest, 99, MPI_COMM_WORLD, &request);
    }

  // 将数组 my_int 从进程0向进程1发送M次。
  for (i = 0; i < M; i++)
    {
      if (myrank == 0)
	{
	  my_int[0] = i + 100;
          // 在持久通信通道中发起一次通信.
	  MPI_Start (&request);
	  MPI_Wait (&request, &status);
          my_int[0] = 0;
	}
      else
	{
	  MPI_Start (&request);
	  // MPI_Startall (request);

	  MPI_Wait (&request, &status);
	  fprintf (stderr,
		   "myrank %d: the %dth recv from %d. get_int[0] = %d\n",
		   myrank, i, status.MPI_SOURCE, get_int[0]);
	}
    }

  // 释放持久通信通道.
  MPI_Request_free (&request);

  MPI_Finalize ();
  return 0;
}
