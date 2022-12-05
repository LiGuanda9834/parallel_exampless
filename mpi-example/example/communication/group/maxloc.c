/**
 * 定义一个结构体, 包含一个双精度数和一个整型数, 
 * 其中整型数存储相应双精度数所在进程的进程号.
 * 在这样一个结构体数组中查找双精度数的最大值, 
 * 并返回找到的双精度最大值及其所在进程的进程号.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1

typedef struct double_int DOUBLE_INT;
struct double_int
{
  double var;
  int rank;
};

int
main (int argc, char *argv[])
{
  int i, myrank, nprocs, root;
  DOUBLE_INT in[N], out[N];

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  // 指定进程0为根进程.
  root = 0;

  // 为随机函数设置种子.
  srand (myrank + 2);
  for (i = 0; i < N; i++)
    {
      in[i].var = (double) rand () / 65536;
      in[i].rank = myrank;
    }

  for (i = 0; i < N; i++)
    {
      fprintf (stderr, "myrank = %d, %f\n", myrank, in[i].var);
    }

  MPI_Reduce (in, out, N, MPI_DOUBLE_INT, MPI_MAXLOC, root, MPI_COMM_WORLD);

  if (myrank == root)
    {
      sleep(1);
      for (i = 0; i < N; i++)
	{
	  fprintf (stderr, "myrank = %d, the %dth max number = %f in %d\n",
		   myrank, i, out[i].var, out[i].rank);

	}
    }


  MPI_Finalize ();
  return 0;
}
