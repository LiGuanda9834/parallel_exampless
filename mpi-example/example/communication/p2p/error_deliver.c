/**
 * 接力棒游戏: 所有进程围成一圈, 每个进程都从自己左面的进程接收信息, 
 *             向自己右面的进程发送消息.
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
    {
      fprintf (stderr, "nporcs = %d \n", nprocs);
    }

  my_number = myrank;
 
  // 确定自己左边的进程和右边的进程.
  left = (myrank + nprocs - 1) % nprocs;
  //left = (myrank - 1) % nprocs;
  right = (myrank + 1) % nprocs;

  fprintf (stderr, "myrank = %d: left = %d, right = %d\n", myrank,
           left, right);

  MPI_Isend (&my_number, 1, MPI_INT, right, 99, MPI_COMM_WORLD, &request[0]);
  MPI_Irecv (&get_number, 1, MPI_INT, left, 99, MPI_COMM_WORLD, &request[1]);
  // do some other jobs

  MPI_Waitall (2, request, status);
  fprintf (stderr, "myrank = %d: my_number = %d, get_number = %d\n", myrank,
	   my_number, get_number);

  MPI_Finalize ();
  return 0;
}
