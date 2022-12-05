/*  使用sendrecv通讯 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "timer.h"

int
main (int argc, char *argv[])
{
  int myrank, nprocs;
  int i, j, k, upper, down, step;
  int mat_scale[2], row_local, col_local;
  double **mat_local, **mat_tmp;
  double t_start, t_end, elapsed;


  MPI_Status status;

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

  GET_TIME(t_start);

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

  // 各进程初始化自己拥有的"虚矩阵", 简单起见, 在水平方向没有专门给出物理边界条件.
  for (i = 0; i < row_local; i++)
    for (j = 0; j < col_local; j++)
      {
	mat_local[i][j] = myrank;
      }

#if 0
  fprintf (stderr, "######\nmyrank = %d\n", myrank);
  for (i = 0; i < row_local; i++)
    {
      for (j = 0; j < col_local; j++)
	fprintf (stderr, "%f, ", mat_local[i][j]);
      fprintf (stderr, "\n");
    }
#endif

  for (k = 0; k < step; k++)
    {

      //每个进程同时做下面两个操作:
      //1. 向上方发送本地"真矩阵"的第1行;  
      //2. 从下方接收并填充本地"虚矩阵"的最后一行.
      MPI_Sendrecv (mat_local[1], col_local, MPI_DOUBLE, upper, 0,
                    mat_local[row_local - 1], col_local, MPI_DOUBLE, down, 0, 
                    MPI_COMM_WORLD, &status);

      //每个进程同时做下面两个操作:
      //1. 向下方发送本地"真矩阵"的最后一行
      //2. 从上方接收并填充本地"虚矩阵"的第1行.
      MPI_Sendrecv (mat_local[row_local - 2], col_local, MPI_DOUBLE, down, 1,
                    mat_local[0], col_local, MPI_DOUBLE, upper, 1, 
                    MPI_COMM_WORLD, &status);

#if 0
      fprintf (stderr, "######\nmyrank = %d\n", myrank);
      for (i = 0; i < row_local; i++)
	{
	  for (j = 0; j < col_local; j++)
	    fprintf (stderr, "%f, ", mat_local[i][j]);
	  fprintf (stderr, "\n");
	}
#endif

      // 五点差分格式计算, 计算结果首先存放在"辅助矩阵"中, 
      // 计算完成后, 再复制到本地"真矩阵"中.
      for (i = 1; i < row_local - 1; i++)
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

    }

    GET_TIME(t_end);
    elapsed=t_end-t_start;
    fprintf (stderr, "(myrank = %d)  elapsed time  %e seconds\n", myrank, elapsed);


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
