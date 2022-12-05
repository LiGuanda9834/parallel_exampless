/**
 * 所有进程读写同一个文件。
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <mpi.h>

#define N 100

int
main (int argc, char *argv[])
{
  int i, myrank, nprocs, num;
  int array1[N], array2[N];
  MPI_Status status;
  MPI_File mpifp;
  MPI_Offset offset;

  MPI_Init (&argc, &argv);

  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  num = myrank + 1;
 
  for (i = 0; i < num; i++)
    {
      array1[i] = myrank + 10;
      array2[i] = -1;
    }

  MPI_File_open (MPI_COMM_WORLD, "sharedfile",
                 MPI_MODE_CREATE+MPI_MODE_WRONLY, MPI_INFO_NULL, &mpifp);

  offset = myrank * num * sizeof(int);
  MPI_File_seek (mpifp, offset, MPI_SEEK_SET);
  MPI_File_write_at (mpifp, offset, array1, num, MPI_INT, &status);
  MPI_File_close (&mpifp);

#if 1
  MPI_File_open (MPI_COMM_SELF, "sharedfile",
                 MPI_MODE_RDONLY, MPI_INFO_NULL, &mpifp);

  MPI_File_read_at (mpifp, offset, array2, num, MPI_INT, &status);
  for (i = 0; i < num; i++)
    printf ("myrank = %d, array2[%d] = %d\n", myrank, i, array2[i]);
  MPI_File_close (&mpifp);
#endif

  MPI_Finalize ();

  return (0);
}
