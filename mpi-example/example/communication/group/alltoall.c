/**
 * 每个进程向所有其它进程发送一个数据. 并从所有其它进程接收一个数据.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int i, myrank, nprocs;
  int *send_buffer;
  int *recv_buffer;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  // send_buffer[i]发送到进程i.
  send_buffer = (int *) calloc (nprocs * 2, sizeof (int));

  // recv_buffer[i]将接收进程i发送来的数据.
  recv_buffer = (int *) calloc (nprocs * 2, sizeof (int));

  for (i = 0; i < nprocs * 2; i++)
    send_buffer[i] = myrank+10;

  MPI_Alltoall (send_buffer, 2, MPI_INT,
		recv_buffer, 2, MPI_INT, MPI_COMM_WORLD);

  for (i = 0; i < nprocs * 2; i++)
    printf ("myrank = %d, recv_buffer[%d] = %d\n", myrank, i, recv_buffer[i]);

  free (recv_buffer);
  free (send_buffer);
  MPI_Finalize ();
  return 0;
}
