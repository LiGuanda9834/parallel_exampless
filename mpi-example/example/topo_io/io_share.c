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
  int i, myrank, nprocs;
  char* prefix = "REMAP_";
  char ch[8];

  MPI_Status status;
  MPI_File mpifp;
  MPI_Offset offset;

  MPI_Init (&argc, &argv);

  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  if (myrank == 0)
    sprintf(ch, "%s%d%s", prefix, myrank, "\n");

  MPI_File_open (MPI_COMM_WORLD, "sharedfile",
                 MPI_MODE_CREATE+MPI_MODE_WRONLY, MPI_INFO_NULL, &mpifp);

  MPI_File_write_shared (mpifp, ch, 8, MPI_CHAR, &status);

  MPI_File_close (&mpifp);

  MPI_Finalize ();

  return (0);
}
