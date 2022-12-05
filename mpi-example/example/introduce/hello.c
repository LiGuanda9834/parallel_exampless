/**
 * 我是世界上引用率最高的程序，hiahia!
 */

#include <stdio.h>
#include <mpi.h>
int main(int argc, char * argv[])
{
  int myrank;

  // 初始化MPI环境
  MPI_Init(&argc, &argv);

  // 获取当前进程在通信器MPI_COMM_WORLD中的进程号
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    printf("myrank = %d: Hello, world!\n", myrank);

  MPI_Finalize();
  return 0;
}

