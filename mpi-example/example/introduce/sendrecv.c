/**
 * 进程0向进程1发送一个整数.
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 100000

int
main (int argc, char *argv[])
{
  int i, myrank;
  int a[N];
  MPI_Status status;

  // 初始化MPI环境
  MPI_Init (&argc, &argv);

  // 初始化数组
  for (i = 0; i < N; i++)
    {
      a[i] = -1;
    }

  // 获取当前进程在通信器MPI_COMM_WORLD中的进程号
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  if (myrank == 0)
    {
      for (i = 0; i < N; i++)
        {
          a[i] = 100;
        }
      // 进程0向进程1发送一个整数
      MPI_Send (&a, N, MPI_INT, 1, 99, MPI_COMM_WORLD);
    }
  else
    {
      fprintf (stderr, "myrank = %d, before recv. a[0] = %d\n", myrank, a[0]);
      // 进程1从进程0接收一个整数
      MPI_Recv (&a, N, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
      fprintf (stderr, "myrank = %d, after recv. a[0] = %d\n\n", myrank, a[0]);
    }

  // 结束MPI环境
  MPI_Finalize ();
  return 0;
}
