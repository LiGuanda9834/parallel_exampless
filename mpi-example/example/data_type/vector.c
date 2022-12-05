/**
 * 为多个类型相同, 相对偏移量固定的数据块定义新数据类型.
 * 每个数据块中旧类型数据的数目相同, 数据块之间的
 * 相对偏移量以旧数据类型大小为尺度.
 */

#include <stdio.h>
#include <mpi.h>

#define N 10

int
main (int argc, char *argv[])
{
  int i, myrank, remot_proc;
  int my_int[N], get_int[N];
  MPI_Status status;
  MPI_Datatype newtype;


  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  //新数据类型中包含3个数据块, 每个数据块中包含两个整型数,
  //相邻两个数据块的起始地址之间的间隔为4个整型.
  MPI_Type_vector (3, 2, 4, MPI_INT, &newtype);

  //向MPI系统提交新数据类型.
  MPI_Type_commit (&newtype);

  for (i = 0; i < N; i++)
    {
      my_int[i] = i;
      get_int[i] = -1;
    }

  remot_proc = (myrank == 0) ? (1) : (0);

  if (myrank == 0)
    MPI_Send (my_int, 1, newtype, remot_proc, 99, MPI_COMM_WORLD);
  else
    MPI_Recv (get_int, 1, newtype, remot_proc, 99, MPI_COMM_WORLD, &status);
    //MPI_Recv (get_int, 10, MPI_INT, remot_proc, 99, MPI_COMM_WORLD, &status);

  if (myrank == 1)
    for (i = 0; i < N; i++)
      fprintf (stderr, "myrank %d: get_int[%d] = %d\n", myrank, i, get_int[i]);

  //释放新数据类型.
  MPI_Type_free (&newtype);
  MPI_Finalize ();
  return 0;
}
