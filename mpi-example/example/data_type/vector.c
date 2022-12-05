/**
 * Ϊ���������ͬ, ���ƫ�����̶������ݿ鶨������������.
 * ÿ�����ݿ��о��������ݵ���Ŀ��ͬ, ���ݿ�֮���
 * ���ƫ�����Ծ��������ʹ�СΪ�߶�.
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

  //�����������а���3�����ݿ�, ÿ�����ݿ��а�������������,
  //�����������ݿ����ʼ��ַ֮��ļ��Ϊ4������.
  MPI_Type_vector (3, 2, 4, MPI_INT, &newtype);

  //��MPIϵͳ�ύ����������.
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

  //�ͷ�����������.
  MPI_Type_free (&newtype);
  MPI_Finalize ();
  return 0;
}
