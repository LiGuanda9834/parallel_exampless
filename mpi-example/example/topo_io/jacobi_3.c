#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define DIM 2

int
main (int argc, char *argv[])
{
  int myrank, nprocs;
  int i, j, k, upper, down, left, right, step;
  int mat_scale[2], row_local, col_local;
  double **mat_local, **mat_tmp;

  int ndims[DIM];		//仅适用于二维情况
  int periods[DIM];
  int coord[DIM];

  MPI_Comm newcomm;
  MPI_Datatype newtype;

  MPI_Request request[8];
  MPI_Status status[8];

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  for (i = 0; i < DIM; i++)
    {
      ndims[i] = 0;
      periods[i] = 1;
    }
  MPI_Dims_create (nprocs, DIM, ndims);
  periods[0] = 1;

  //创建新通信器。
  MPI_Cart_create (MPI_COMM_WORLD, DIM, ndims, periods, 1, &newcomm);

  //确定本地进程的上、下、左、右邻居。
  MPI_Cart_shift (newcomm, 0, 1, &upper, &down);
  MPI_Cart_shift (newcomm, 1, 1, &left, &right);

  //获取本地进程在新通信器中的进程号。
  MPI_Comm_rank (newcomm, &myrank);

  //获取本地进程在新通信器中的坐标。
  MPI_Cart_coords (newcomm, myrank, DIM, coord);

#if 2
  fprintf (stderr, "\n\tmyrank %d <---> (", myrank);
  for (i = 0; i < DIM; i++)
    fprintf (stderr, " %d ", coord[i]);
  fprintf (stderr, ")");
  fprintf (stderr, " upper: %d, down: %d, left: %d, right: %d\n",
           upper, down, left, right);
#endif
 
  sleep(1);
  if (myrank == 0)
    {
      fprintf (stderr, "Enter the scale of the matrix [M,N]: ");
      scanf ("%d,%d", &mat_scale[0], &mat_scale[1]);

      if (mat_scale[0] % ndims[0] != 0)
	{
	  fprintf (stderr, "Wrong M: %d\n", mat_scale[0]);
	  MPI_Abort (MPI_COMM_WORLD, __LINE__);
	  return 0;
	}
      if (mat_scale[1] % ndims[1] != 0)
	{
	  fprintf (stderr, "Wrong N: %d\n", mat_scale[1]);
	  MPI_Abort (MPI_COMM_WORLD, __LINE__);
	  return 0;
	}

      fprintf (stderr, "Enter the iterate number: ");
      scanf ("%d", &step);
    }

  //根进程将矩阵尺寸和迭代次数分两次广播到所有进程.
  MPI_Bcast (&mat_scale, 2, MPI_INT, 0, newcomm);
  MPI_Bcast (&step, 1, MPI_INT, 0, newcomm);

  //本地"虚矩阵"的行数和列数.
  row_local = mat_scale[0] / ndims[0] + 2;
  col_local = mat_scale[1] / ndims[1] + 2;

  if (myrank == 0)
    fprintf (stderr, "row_local = %d, col_local = %d\n", row_local,
	     col_local);

  //定义新数据类型.
  MPI_Type_vector (row_local - 2, 1, col_local, MPI_DOUBLE, &newtype);

  //向MPI系统提交新数据类型.
  MPI_Type_commit (&newtype);

  //为本地"虚矩阵"和"辅助虚矩阵"开辟内存空间.
  mat_local = (double **) calloc (row_local, sizeof (double *));
  mat_tmp = (double **) calloc (row_local, sizeof (double *));
#if 1
  mat_local[0] = (double *) calloc (col_local*row_local, sizeof (double));
  mat_tmp[0] = (double *) calloc (col_local*row_local, sizeof (double));
  for (i = 1; i < row_local; i++)
    {
      mat_local[i] = mat_local[0]+i*col_local;
      mat_tmp[i] = mat_tmp[0]+i*col_local;
    }
#endif
#if 0
  for (i = 0; i < row_local; i++)
    { 
      mat_local[i] = (double *) calloc (col_local, sizeof (double));
      mat_tmp[i] = (double *) calloc (col_local, sizeof (double));
    }
#endif

  // 各进程初始化自己拥有的数据, 简单起见, 在水平方向没有专门给出物理边界条件.
  for (i = 0; i < row_local; i++)
    for (j = 0; j < col_local; j++)
      {
	mat_local[i][j] = myrank;
      }

  //每个进程首先向上方发送本地"真矩阵"的第1行, 
  //然后从下方接收并填充本地"虚矩阵"的最后一行.
  MPI_Isend (&mat_local[1][1], col_local - 2, MPI_DOUBLE, upper, 0,
	     newcomm, &request[0]);
  MPI_Irecv (&mat_local[row_local - 1][1], col_local - 2, MPI_DOUBLE, down, 0,
	     newcomm, &request[1]);

  //每个进程首先向下方发送本地"真矩阵"的最后一行, 
  //然后从上方接收并填充接收本地"虚矩阵"的第1行.
  MPI_Isend (&mat_local[row_local - 2][1], col_local - 2, MPI_DOUBLE, down, 1,
	     newcomm, &request[2]);
  MPI_Irecv (&mat_local[0][1], col_local - 2, MPI_DOUBLE, upper, 1,
	     newcomm, &request[3]);

  //每个进程首先向左边发送本地"真矩阵"的第一列, 
  //然后从右边接收并填充接收本地"虚矩阵"的最后一列.
  MPI_Isend (&mat_local[1][1], 1, newtype, left, 2, newcomm, &request[4]);
  MPI_Irecv (&mat_local[1][col_local - 1], 1, newtype, right, 2,
	     newcomm, &request[5]);

  //每个进程首先向右边发送本地"真矩阵"的最后一列, 
  //然后从左边接收并填充接收本地"虚矩阵"的第一列.
  MPI_Isend (&mat_local[1][col_local - 2], 1, newtype, right, 3,
	     newcomm, &request[6]);
  MPI_Irecv (&mat_local[1][0], 1, newtype, left, 3, newcomm, &request[7]);

  MPI_Waitall (8, request, status);

  for (k = 0; k < step; k++)
    {
      //首先计算出要发送出去的数据, 即本地"真矩阵"的第1行、最后一行
      //以及"真矩阵"的第一列和最后一列.
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

      j = 1;
      for (i = 1; i < row_local - 1; i++)
	mat_tmp[i][j] = 0.25 * (mat_local[i - 1][j] +
				mat_local[i + 1][j] +
				mat_local[i][j - 1] + mat_local[i][j + 1]);

      j = col_local - 2;
      for (i = 1; i < row_local - 1; i++)
	mat_tmp[i][j] = 0.25 * (mat_local[i - 1][j] +
				mat_local[i + 1][j] +
				mat_local[i][j - 1] + mat_local[i][j + 1]);

      // 启动非阻塞通信操作.

      //每个进程向上方发送本地"真矩阵"的第1行, 
      //然后从下方接收并填充本地"虚矩阵"的最后一行.
      MPI_Isend (&mat_tmp[1][1], col_local - 2, MPI_DOUBLE, upper, 0,
		 newcomm, &request[0]);
      MPI_Irecv (&mat_local[row_local - 1][1], col_local - 2, MPI_DOUBLE, 
                 down, 0, newcomm, &request[1]);

      //每个进程向下方发送本地"真矩阵"的最后一行, 
      //然后从上方接收并填充本地"虚矩阵"的第1行.
      MPI_Isend (&mat_tmp[row_local - 2][1], col_local - 2, MPI_DOUBLE, 
                 down, 1, newcomm, &request[2]);
      MPI_Irecv (&mat_local[0][1], col_local - 2, MPI_DOUBLE, upper, 1,
		 newcomm, &request[3]);

      //每个进程首先向左边发送本地"真矩阵"的第一列, 
      //然后从右边接收并填充接收本地"虚矩阵"的最后一列.
      MPI_Isend (&mat_tmp[1][1], 1, newtype, left, 2, newcomm, &request[4]);
      MPI_Irecv (&mat_local[1][col_local - 1], 1, newtype, right, 2,
		 newcomm, &request[5]);

      //每个进程首先向右边发送本地"真矩阵"的最后一列, 
      //然后从左边接收并填充接收本地"虚矩阵"的第一列.
      MPI_Isend (&mat_tmp[1][col_local - 2], 1, newtype, right, 3,
		 newcomm, &request[6]);
      MPI_Irecv (&mat_local[1][0], 1, newtype, left, 3, newcomm, &request[7]);

#if 1
      fprintf (stderr, "######\nmyrank = %d\n", myrank);
      for (i = 0; i < row_local; i++)
	{
	  for (j = 0; j < col_local; j++)
	    fprintf (stderr, "%f, ", mat_local[i][j]);
	  fprintf (stderr, "\n");
	}
#endif
      // 在通信进行的同时, 计算本地"真矩阵"的剩余部分, 达到通信与计算重叠的目的.
      for (i = 2; i < row_local - 2; i++)
	for (j = 2; j < col_local - 2; j++)
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
      MPI_Waitall (8, request, status);
    }

  //释放先前开辟的内存空间.
#if 0
  for (i = 0; i < row_local; i++)
    {
      free (mat_local[i]);
      free (mat_tmp[i]);
    }
#endif
  free (mat_local[0]);
  free (mat_tmp[0]);
  free (mat_local);
  free (mat_tmp);

  //释放新数据类型.
  MPI_Type_free (&newtype);

  MPI_Finalize ();
  return 0;
}
