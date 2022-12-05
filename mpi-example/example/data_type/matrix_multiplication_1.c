#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

/**
 * 计算矩阵乘积c=a*b. 矩阵按行分块, 不同块分布于不同处理器.
 * 计算前,先将矩阵b转置, 以提高计算速度.
 */

int
main (int argc, char *argv[])
{
  int i, j, k, m, myrank, nprocs, begin, upper, down;
  int local_scale, global_scale;
  int *a, *b, *c, *b_t;
  MPI_Datatype col_type, matrix_type;
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  if (myrank == 0)
    {
      fprintf (stderr, "Enter the scale of the matrix M: ");
      scanf ("%d", &global_scale);

      if (global_scale % nprocs != 0)
        {
          fprintf (stderr, "Wrong M: %d\n", global_scale);
          MPI_Abort (MPI_COMM_WORLD, __LINE__);
          return 0;
        }
    }

  //根进程将矩阵尺寸广播到所有进程.
  MPI_Bcast (&global_scale, 1, MPI_INT, 0, MPI_COMM_WORLD);

  //本地矩阵的尺寸.
  local_scale = global_scale / nprocs;

  //为本地矩阵开辟内存空间, 用一维数组存储, b_t为b的转置矩阵.
  a = (int *) calloc (local_scale * global_scale, sizeof (int));
  b = (int *) calloc (local_scale * global_scale, sizeof (int));
  c = (int *) calloc (local_scale * global_scale, sizeof (int));
  b_t = (int *) calloc (global_scale * local_scale, sizeof (int));

  //初始化矩阵(随意给的数字).
  for (i = 0; i < local_scale * global_scale; i++)
    {
      a[i] = 2;
      b[i] = 3;
      c[i] = 0;
    }
 
  //构造新数据类型col_type, 描述本地矩阵的一列.
  MPI_Type_vector (local_scale, 1, global_scale, MPI_INT, &col_type);
  MPI_Type_commit (&col_type);

  //构造新数据类型matrix_type, 描述本地矩阵.
  MPI_Type_hvector (global_scale, 1, sizeof(int), col_type, &matrix_type);
  MPI_Type_commit (&matrix_type);

  //当前进程上方的进程.
  upper = (myrank + nprocs - 1) % nprocs;

  //当前进程下方的进程.
  down = (myrank + 1) % nprocs;

  //利用新数据类型, 将本地矩阵b转置, 并存储在b_t中.
  MPI_Sendrecv (b, 1, matrix_type, myrank, 99,
                b_t, global_scale * local_scale, MPI_INT, myrank, 99,
                MPI_COMM_WORLD, &status);

  //计算a*b_t, 其中a[begin]表示与当前矩阵b_t"匹配"的第一个元素.
  begin = myrank * local_scale;
  for (j = 0; j < local_scale; j++)
    for (k = 0; k < global_scale; k++)
      for (m = 0; m < local_scale; m++)
        c[j * global_scale + k] = c[j * global_scale + k]
                                 + a[begin + j * global_scale + m]
                                 * b_t[k * local_scale + m];
  for (i = 1; i < nprocs; i++)
    {
      //将本地矩阵b_t发送到上面的进程, 并从下面的进程接收新的矩阵b_t.
      MPI_Sendrecv_replace (b_t, global_scale * local_scale, MPI_INT, 
                       upper, 99, down, 99, MPI_COMM_WORLD, &status);

      //计算a*b_t, 其中a[begin]表示与当前矩阵b_t"匹配"的第一个元素.
      begin = down * local_scale;
      for (j = 0; j < local_scale; j++)
        for (k = 0; k < global_scale; k++)
          for (m = 0; m < local_scale; m++)
            c[j * global_scale + k] = c[j * global_scale + k]
                                     + a[begin + j * global_scale + m]
                                     * b_t[k * local_scale + m];
    }

  sleep(myrank);
  fprintf (stderr, "######\nmyrank = %d\n", myrank);
  for (i = 0; i < local_scale; i++)
    {
      for (j = 0; j < global_scale; j++)
        fprintf (stderr, "%d, ", c[i * global_scale + j]);
      fprintf (stderr, "\n");
    }

  // 释放先前开辟的内存空间.
  free (a);
  free (b);
  free (c);
  free (b_t);
  MPI_Finalize ();

  return (0);
}
