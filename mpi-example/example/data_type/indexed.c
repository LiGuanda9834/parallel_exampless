/**
 * 为不连续的, 类型相同的数据块定义新数据类型.
 * 数据块之间的偏移量以旧类型大小为尺度.
 */

#include <stdio.h>
#include <mpi.h>

#define N 10

int
main (int argc, char *argv[])
{
  int i, myrank, remot_proc;
  int my_int[N], get_int[N];
  int lengths[3];
  int displaces[3];

  MPI_Aint type_extent;
  int type_size, recv_count0, recv_count1;

  MPI_Status status;
  MPI_Datatype newtype;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  lengths[0] = 2;
  lengths[1] = 4;
  lengths[2] = 1;

  displaces[0] = 0;
  displaces[1] = 3;
  displaces[2] = 9;


  // 新数据类型中包含3个数据块, 第一个数据块由lengths[0]个整型数组成,
  // 第二个数据块由lengths[1]个整型数组成, 第三个数据块由lengths[2]各整型数组成.
  // 第二个数据块的起始位置相对于第一个数据块的起始位置的偏移量为
  //                                   displaces[1]-displaces[0]个整型数.
  // 第三个数据块的起始位置相对于第一个数据块的起始位置的偏移量为
  //                                   displaces[2]-displaces[0]个整型数.
  MPI_Type_indexed (3, lengths, displaces, MPI_INT, &newtype);

  // 向MPI系统提交新数据类型.
  MPI_Type_commit (&newtype);

#if 1
  if (myrank == 0)
    {
      // 获取新数据类型的跨度.
      MPI_Type_extent (newtype, &type_extent);

      // 获取新数据类型的实际大小.
      MPI_Type_size (newtype, &type_size);

      fprintf (stderr, "type_extent = %d\n", type_extent);
      fprintf (stderr, "type_size = %d\n", type_size);
    }
#endif

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

  if (myrank == 1)
    {
      // 以基本数据类型为标准计算接收到的数据量.
      MPI_Get_elements (&status, newtype, &recv_count0);

      // 以新数据类型为标准计算接收到的数据量.
      MPI_Get_count (&status, newtype, &recv_count1);

      fprintf (stderr, "\nmyrank %d: recv_count0 = %d\n", myrank, recv_count0);
      fprintf (stderr, "myrank %d: recv_count1 = %d\n\n", myrank, recv_count1);
      for (i = 0; i < N; i++)
	fprintf (stderr, "myrank %d: get_int[%d] = %d\n", myrank, i, get_int[i]);

    }

  //释放新数据类型.
  MPI_Type_free (&newtype);
  MPI_Finalize ();
  return 0;
}
