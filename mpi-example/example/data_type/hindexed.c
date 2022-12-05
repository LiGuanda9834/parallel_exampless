/**
 * Ϊ��������, ������ͬ�����ݿ鶨������������.
 * ���ݿ�֮���ƫ�������ֽ�Ϊ�߶�.
 * 
 * ������, ����0������my_int�ĵ�0, 1, 3, 4, 5, 6, 9��Ԫ�ط��͸�����1.
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
  MPI_Aint displaces[3];
  MPI_Status status;
  MPI_Datatype newtype;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  lengths[0] = 2;
  lengths[1] = 4;
  lengths[2] = 1;

#if 1
  displaces[0] = 0;
  displaces[1] = 12;
  displaces[2] = 36;
#endif

#if 0
  // ��ȡ����my_int�е�0��, ��3���͵�9��Ԫ�صĵ�ַ.
  MPI_Address (&my_int[0], &displaces[0]);
  MPI_Address (&my_int[3], &displaces[1]);
  MPI_Address (&my_int[9], &displaces[2]);

  // ������������Ԫ������ڵ�0��Ԫ�ص��ڴ��ַ���ƫ����.
  displaces[2] = displaces[2] - displaces[0];
  displaces[1] = displaces[1] - displaces[0];
  displaces[0] = 0;
  fprintf (stderr, "myrank %d: displaces = %d, %d, %d\n", 
           myrank, displaces[0], displaces[1], displaces[2]);
#endif

  MPI_Type_hindexed (3, lengths, displaces, MPI_INT,
		    &newtype);

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

  if (myrank == 1)
    for (i = 0; i < N; i++)
      fprintf (stderr, "myrank %d: get_int[%d] = %d\n", myrank, i, get_int[i]);

  MPI_Type_free (&newtype);
  MPI_Finalize ();
  return 0;
}
