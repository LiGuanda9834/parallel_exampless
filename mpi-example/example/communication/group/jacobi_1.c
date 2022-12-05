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

  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  //��ǰ�����Ϸ��Ľ���.
  upper = (myrank + nprocs - 1) % nprocs;
  
  //��ǰ�����·��Ľ���.
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

  //�����̽�����ߴ�͵������������ι㲥�����н���.
  MPI_Bcast (&mat_scale, 2, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast (&step, 1, MPI_INT, 0, MPI_COMM_WORLD);

  //����"�����"������������.
  row_local = mat_scale[0] / nprocs + 2;
  col_local = mat_scale[1] + 2;

  if (myrank == 0)
    fprintf (stderr, "row_local = %d, col_local = %d\n", row_local,
	     col_local);

  //Ϊ����"�����"��"���������"�����ڴ�ռ�.
  mat_local = (double **) calloc (row_local, sizeof (double *));
  mat_tmp = (double **) calloc (row_local, sizeof (double *));
  for (i = 0; i < row_local; i++)
    {
      mat_local[i] = (double *) calloc (col_local, sizeof (double));
      mat_tmp[i] = (double *) calloc (col_local, sizeof (double));
    }

  // �����̳�ʼ���Լ�ӵ�е�"�����", �����, ��ˮƽ����û��ר�Ÿ�������߽�����.
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

      //ÿ������ͬʱ��������������:
      //1. ���Ϸ����ͱ���"�����"�ĵ�1��;  
      //2. ���·����ղ���䱾��"�����"�����һ��.
      MPI_Sendrecv (mat_local[1], col_local, MPI_DOUBLE, upper, 0,
                    mat_local[row_local - 1], col_local, MPI_DOUBLE, down, 0, 
                    MPI_COMM_WORLD, &status);

      //ÿ������ͬʱ��������������:
      //1. ���·����ͱ���"�����"�����һ��
      //2. ���Ϸ����ղ���䱾��"�����"�ĵ�1��.
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

      // ����ָ�ʽ����, ���������ȴ����"��������"��, 
      // ������ɺ�, �ٸ��Ƶ�����"�����"��.
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

  //�ͷ���ǰ���ٵ��ڴ�ռ�.
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
