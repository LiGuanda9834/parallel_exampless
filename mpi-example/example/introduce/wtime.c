 
/**
 * ͳ�Ƹ��������ĵ�ǽ��ʱ��.
 */
#include <unistd.h>
#include <stdio.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int myrank, nprocs, name_len, flag;
  double start_time, end_time;
  char host_name[10];

  // ���MPI�����Ƿ��Ѿ���ʼ��.
  MPI_Initialized (&flag);
  fprintf (stderr, "flag: %d\n", flag);

  // ��ʼ��MPI����
  MPI_Init (&argc, &argv);

  // ��ȡ��ǰ������ͨ����MPI_COMM_WORLD�еĽ��̺�
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  // ��ȡͨ����MPI_COMM_WORLD�еĽ�����
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
 
  // ��ȡ����������
  MPI_Get_processor_name(host_name, &name_len);
  if (myrank == 0)
   {
     fprintf (stderr, "precision of MPI_Wtime(): %f.\n", MPI_Wtick());  
     fprintf (stderr, "host name: %s\n", host_name);
   }
  
  //��ȡǽ��ʱ��
  start_time = MPI_Wtime();
  sleep(myrank * 3);
  
  //��ȡǽ��ʱ��
  end_time = MPI_Wtime();
  fprintf (stderr, "myrank: %d. I have slept %f seconds.\n", 
           myrank, end_time - start_time);
  
  // ����MPI����
  MPI_Finalize ();
  return 0;
}
