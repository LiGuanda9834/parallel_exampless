/**
 * ���н��̶�д��ͬ���ļ���
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int i, myrank, nprocs;
  char file[20];
  FILE *fp;

  MPI_Init (&argc, &argv);

  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  // �������ú���sprintf()���ɲ�ͬ������ļ���
  // ������ʹ�ú���: snprintf(), swprintf().
  sprintf (file, "file_%d", myrank);

  //fprintf (stderr, "myrank = %d, %s\n", myrank, file);

  fp = fopen (file, "w");
  fprintf (fp, "myrank = %d\n", myrank);
  fclose (fp);

  return (0);
}
