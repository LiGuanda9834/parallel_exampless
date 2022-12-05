/**
 * 特殊的进程号: MPI_PROC_NULL.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int myrank, nprocs, left, right;
  int my_number, get_number;

  MPI_Request request[2];
  MPI_Status status[2];

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  if (myrank == 0)
    fprintf (stderr, "nporcs = %d \n", nprocs);

  my_number = myrank;
  left = myrank - 1;
  right = myrank + 1;

#if 0
  if ((left < 0)||(left >= nprocs))
    left = MPI_PROC_NULL;
  if ((right < 0)||(right >= nprocs))
    right = MPI_PROC_NULL;
#endif
  //fprintf (stderr, "myrank = %d: left = %d, right = %d\n", myrank,
  //         left, right);

  MPI_Isend (&my_number, 1, MPI_INT, right, 99, MPI_COMM_WORLD, &request[0]);
  MPI_Irecv (&get_number, 1, MPI_INT, left, 99, MPI_COMM_WORLD, &request[1]);
  // do some other jobs

  MPI_Waitall (2, request, status);
  fprintf (stderr, "myrank = %d: my_number = %d, get_number = %d\n", myrank,
	   my_number, get_number);

  MPI_Finalize ();
  return 0;
}
