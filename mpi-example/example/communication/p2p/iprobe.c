/**
 *  函数MPI_Iprobe非阻塞地查询匹配的消息。
 *  如果存在匹配的消息，则参数flag设置为真；否则为假。
 */

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define N 1

int
main (int argc, char *argv[])
{
  int i, nprocs, myrank, flag, recv_count, source, tag;
  int *send_buf;
  int *recv_buf;
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  if (myrank != nprocs - 1)
    {
      send_buf = (int *) calloc ((myrank + 1), sizeof (int));

      tag = myrank + 10;
      // 所有进程号不等于nprocs-1的进程将自己的send_buf发送给进程nprocs-1.
      MPI_Send (send_buf, myrank + 1, MPI_INT, nprocs - 1, tag, MPI_COMM_WORLD);
      free (send_buf);
    }
  else
    {
      // 进程nprocs-1将接收nprocs-1个消息.
      for (i = 0; i < nprocs - 1; i++)
	{
          // 进程nprocs-1检测网络中是否有发送给自己的消息.
	  MPI_Iprobe (MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
	  if (flag)
	    {
              // 检测到一个发送给自己的消息后, 以整型量为单位查询消息大小.
	      MPI_Get_count (&status, MPI_INT, &recv_count);
              recv_buf = (int *) calloc (recv_count, sizeof (int));
	
              // 获取该消息来源.
              source = status.MPI_SOURCE;
              tag = status.MPI_TAG;
	
              // 进程nprocs-1根据探测出的消息来源和大小进行接收.
              MPI_Recv (recv_buf, recv_count, MPI_INT, source, tag,
			MPI_COMM_WORLD, &status);
	      fprintf (stderr, "myrank = %d, recive %d numbers from %d with tag = %d\n",
                       myrank, recv_count, source, tag);
              free(recv_buf);
              //sleep(1);
	    }
	}
    }

  MPI_Finalize ();
  return 0;
}
