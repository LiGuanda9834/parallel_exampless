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

  printf ("process id = %d, argc = %d\n", getpid (), argc);
  for (i = 0; i < argc; i++)
    printf ("process id = %d, argv[%d] = %s\n", getpid (), i, argv[i]);
#if 1
  MPI_Init (&argc, &argv);

  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  printf ("myrank = %d, argc = %d\n", myrank, argc);
  for (i = 0; i < argc; i++)
    printf ("process id = %d, argv[%d] = %s\n", getpid (), i, argv[i]);
  //sleep(40);
  MPI_Finalize ();
#endif

  return (0);
}
