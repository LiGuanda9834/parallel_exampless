/**
 * 同步函数.
 */

#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

int main(int argc, char * argv[])
{
  int myrank;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  // 不同进程根据自己的进程号睡眠不同的时间.
  fprintf(stderr, "myrank: %d, arrive the barrier, waiting ...\n", myrank);
  sleep(4 * myrank + 1);
  fprintf(stderr, "myrank: %d, awakening ...\n", myrank);

  // 所有进程在此处同步.
  MPI_Barrier(MPI_COMM_WORLD);
  fprintf(stderr, "myrank: %d, wwwwwwwwwwwww\n", myrank);
  MPI_Finalize();
  return 0;
}
