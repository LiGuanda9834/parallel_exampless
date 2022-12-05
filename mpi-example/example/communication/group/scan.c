/**
 * 前缀归约. 每个进程保存该进程与该进程前面进程做归约后的结果.
 */

#include <stdio.h>
#include <mpi.h>

void
operator (void *invec, void *inoutvec, int *length, MPI_Datatype * data_type)
{
  int i;
  int *a = (int *) invec;
  int *b = (int *) inoutvec;
  for (i = 0; i < *length; i++)
    {
      b[i] += a[i];
    }
}

int
main (int argc, char *argv[])
{
  int myrank;
  MPI_Op op;
  int buffer[3] = { 1, 5, 10 };
  int recv_buffer[3];

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
 
  // 创建规约算子.
  MPI_Op_create (&operator, 0, &op);

  //MPI_Scan (buffer, recv_buffer, 3, MPI_INT, op, MPI_COMM_WORLD);
  MPI_Scan (buffer, recv_buffer, 3, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  // 释放所创建的规约算子.
  MPI_Op_free (&op);

  fprintf (stderr, "myrank = %d, recv_buffer = {%d, %d, %d}\n", 
   myrank, recv_buffer[0], recv_buffer[1], recv_buffer[2]);

  MPI_Finalize ();

  return 0;
}
