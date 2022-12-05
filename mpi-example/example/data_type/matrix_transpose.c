/**
 * 通过创建新的数据类型实现矩阵转置.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int i, j, m, n;
  int *matrix, *transposed_matrix;
  FILE *fp;
  MPI_Datatype matrix_type, column_type, transposed_matrix_type;
  MPI_Status status;

  MPI_Init (&argc, &argv);

  if (argc != 3)
    {
      fprintf (stderr, "Usage:\nmpirun -np 1 m n\nor:\n./matrix m n\n");
      MPI_Finalize ();
      return 0;
    }
      
  // 获取矩阵大小, m: 矩阵行数, n: 矩阵列数.
  m = atoi (argv[1]);
  n = atoi (argv[2]);

  // 创建矩阵.
  matrix = (int *) calloc (m * n, sizeof (int));

  // 创建转置矩阵.
  transposed_matrix = (int *) calloc (n * m, sizeof (int));

  // 初始化矩阵, 并将矩阵输出到文件matrix.dat中.
  fp = fopen ("matrix.dat", "w");
  for (i = 0; i < m; i++)
    {
      for (j = 0; j < n; j++)
	{
	  matrix[i * n + j] = i * n + j;
	  fprintf (fp, "%d\t", matrix[i * n + j]);
	}
      fprintf (fp, "\n");
    }
  fclose (fp);

  MPI_Type_vector(m, 1, n, MPI_INT, &column_type);
  MPI_Type_commit (&column_type);

  //矩阵转置(通过转置每一行)。
  for (i = 0; i < n; i++)
    {
      MPI_Sendrecv (matrix + i, 1, column_type, 0, 99,
                    transposed_matrix + i * m, m, MPI_INT, 0, 99,
    		    MPI_COMM_SELF, &status);
    }

  MPI_Type_free (&column_type);

#if 0
  //为m行n列的矩阵定义一个新数据类型：“matrix_type”。
  MPI_Type_vector (m, n, n, MPI_INT, &matrix_type);
  MPI_Type_commit (&matrix_type);

  //在一个n行m列的矩阵中，为它的一列定义一个新数据类型：“column_type”。
  MPI_Type_vector (n, 1, m, MPI_INT, &column_type);
  MPI_Type_commit (&column_type);

  //将m个“column_type”按一个整型数的偏移量放在一起，
  //就形成一个可以表示n行m列矩阵的类型：“transposed_matrix_type”。
  MPI_Type_hvector (m, 1, sizeof (int), column_type, &transposed_matrix_type);
  MPI_Type_commit (&transposed_matrix_type);

  //矩阵转置。
  MPI_Sendrecv (matrix, 1, matrix_type, 0, 99,
		transposed_matrix, 1, transposed_matrix_type, 0, 99,
		MPI_COMM_SELF, &status);

  //释放先前定义的新数据类型。
  MPI_Type_free (&matrix_type);
  MPI_Type_free (&column_type);
  MPI_Type_free (&transposed_matrix_type);
#endif

  // 将转置后的矩阵输出到文件transposed_matrix.dat中.
  fp = fopen ("transposed_matrix.dat", "w");
  for (i = 0; i < n; i++)
    {
      for (j = 0; j < m; j++)
	{
	  fprintf (fp, "%d\t", transposed_matrix[i * m + j]);
	}
      fprintf (fp, "\n");
    }
  fclose (fp);

  //释放先前开辟的内存空间。
  free (matrix);
  free (transposed_matrix);

  MPI_Finalize ();
  return 0;
}
