/**
 * 广播函数示例.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1

int
main (int argc, char *argv[])
{
  int i, myrank, nprocs;
  int *buffer;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  // 所有进程都开辟大小为N的整型内存空间.
  buffer = (int *) calloc (N, sizeof (int));
  for (i = 0; i < N; i++)
    {
      buffer[i] = myrank;
      printf ("before bcasting, myrank = %d, buffer[%d] = %d\n", 
              myrank, i, buffer[i]);

    }
  printf ("\n");

  // 以进程0作为根进程将buffer广播给所有其它进程.
  MPI_Bcast (buffer, N, MPI_INT, 0, MPI_COMM_WORLD);

  for (i = 0; i < N; i++)
    printf ("after bcasting, myrank = %d, buffer[%d] = %d\n", 
            myrank, i, buffer[i]);
  printf ("\n");
  free (buffer);
  MPI_Finalize ();
  return 0;
}
