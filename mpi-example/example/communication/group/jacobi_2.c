#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int myrank, nprocs;
  int i, j, k, upper, down, step;
  int mat_scale[2], row_local, col_local;
  double **mat_local, **mat_tmp;

  MPI_Request request[4];
  MPI_Status status[4];

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  //当前进程上方的进程.
  upper = (myrank + nprocs - 1) % nprocs;

  //当前进程下方的进程.
  down = (myrank + 1) % nprocs;

#if 0
  if (myrank == 0)
    upper = MPI_PROC_NULL;
  if (myrank == nporcs - 1)
    down = MPI_PROC_NULL;
#endif

  if (myrank == 0)
    {
      fprintf (stderr, "Enter the scale of the matrix [M,N]: ");
      scanf ("%d,%d", &mat_scale[0], &mat_scale[1]);

      if (mat_scale[0] % nprocs != 0)
	{
	  fprintf (stderr, "Wrong M: %d\n", mat_scale[0]);
	  MPI_Abort (MPI_COMM_WORLD, __LINE__);
	  return 0;
	}

      fprintf (stderr, "Enter the iterate number: ");
      scanf ("%d", &step);
    }

  //根进程将矩阵尺寸和迭代次数分两次广播到所有进程.
  MPI_Bcast (&mat_scale, 2, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast (&step, 1, MPI_INT, 0, MPI_COMM_WORLD);

  //本地"虚矩阵"的行数和列数.
  row_local = mat_scale[0] / nprocs + 2;
  col_local = mat_scale[1] + 2;

  if (myrank == 0)
    fprintf (stderr, "row_local = %d, col_local = %d\n", row_local,
	     col_local);

  //为本地"虚矩阵"和"辅助虚矩阵"开辟内存空间.
  mat_local = (double **) calloc (row_local, sizeof (double *));
  mat_tmp = (double **) calloc (row_local, sizeof (double *));
  for (i = 0; i < row_local; i++)
    {
      mat_local[i] = (double *) calloc (col_local, sizeof (double));
      mat_tmp[i] = (double *) calloc (col_local, sizeof (double));
    }

  // 各进程初始化自己拥有的数据, 简单起见, 在水平方向没有专门给出物理边界条件.
  for (i = 0; i < row_local; i++)
    for (j = 0; j < col_local; j++)
      {
	mat_local[i][j] = myrank;
      }

  //每个进程首先向上方发送本地"真矩阵"的第1行, 
  //然后从下方接收并填充本地"虚矩阵"的最后一行.
  MPI_Isend (mat_local[1], col_local, MPI_DOUBLE, upper, 0,
	     MPI_COMM_WORLD, &request[0]);
  MPI_Irecv (mat_local[row_local - 1], col_local, MPI_DOUBLE, down, 0,
	     MPI_COMM_WORLD, &request[1]);

  //每个进程首先向下方发送本地"真矩阵"的最后一行, 
  //然后从上方接收并填充接收本地"虚矩阵"的第1行.
  MPI_Isend (mat_local[row_local - 2], col_local, MPI_DOUBLE, down, 1,
	     MPI_COMM_WORLD, &request[2]);
  MPI_Irecv (mat_local[0], col_local, MPI_DOUBLE, upper, 1,
	     MPI_COMM_WORLD, &request[3]);

  MPI_Waitall (4, request, status);

  for (k = 0; k < step; k++)
    {
      //首先计算出要发送出去的数据, 即本地"真矩阵"的第1行和最后一行.
      i = 1;
      for (j = 1; j < col_local - 1; j++)
	mat_tmp[i][j] = 0.25 * (mat_local[i - 1][j] +
				mat_local[i + 1][j] +
				mat_local[i][j - 1] + mat_local[i][j + 1]);

      i = row_local - 2;
      for (j = 1; j < col_local - 1; j++)
	mat_tmp[i][j] = 0.25 * (mat_local[i - 1][j] +
				mat_local[i + 1][j] +
				mat_local[i][j - 1] + mat_local[i][j + 1]);

      // 起动非阻塞通信操作.

      //每个进程向上方发送本地"真矩阵"的第1行, 
      //然后从下方接收并填充本地"虚矩阵"的最后一行.
      MPI_Isend (mat_tmp[1], col_local, MPI_DOUBLE, upper, 0,
		 MPI_COMM_WORLD, &request[0]);
      MPI_Irecv (mat_local[row_local - 1], col_local, MPI_DOUBLE, down, 0,
		 MPI_COMM_WORLD, &request[1]);

      //每个进程向下方发送本地"真矩阵"的最后一行, 
      //然后从上方接收并填充本地"虚矩阵"的第1行.
      MPI_Isend (mat_tmp[row_local - 2], col_local, MPI_DOUBLE, down, 1,
		 MPI_COMM_WORLD, &request[2]);
      MPI_Irecv (mat_local[0], col_local, MPI_DOUBLE, upper, 1,
		 MPI_COMM_WORLD, &request[3]);

#if 0
      fprintf (stderr, "######\nmyrank = %d\n", myrank);
      for (i = 0; i < row_local; i++)
	{
	  for (j = 0; j < col_local; j++)
	    fprintf (stderr, "%f, ", mat_local[i][j]);
	  fprintf (stderr, "\n");
	}
#endif
      // 在通信进行的同时, 计算本地"真矩阵"的剩余行, 达到通信与计算重叠的目的.
      for (i = 2; i < row_local - 2; i++)
	for (j = 1; j < col_local - 1; j++)
	  {
	    mat_tmp[i][j] = 0.25 * (mat_local[i - 1][j] +
				    mat_local[i + 1][j] +
				    mat_local[i][j - 1] + 
				    mat_local[i][j + 1]);
	  }

      for (i = 1; i < row_local - 1; i++)
	for (j = 1; j < col_local - 1; j++)
	  mat_local[i][j] = mat_tmp[i][j];

      //等待非阻塞通信完成, 就可以计算下次要发送数据,
      //即: 本地"真矩阵"的第1行和最后一行.
      MPI_Waitall (4, request, status);
    }

  //释放先前开辟的内存空间.
  for (i = 0; i < row_local; i++)
    {
      free (mat_local[i]);
      free (mat_tmp[i]);
    }
  free (mat_local);
  free (mat_tmp);

  MPI_Finalize ();
  return 0;
}
