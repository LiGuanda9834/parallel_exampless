/**
 *  函数MPI_Probe阻塞地查询系统中匹配的消息。
 */

#include <stdio.h>
#include <mpi.h>

#define N 1

int
main (int argc, char *argv[])
{
  int i, nprocs, myrank;
  int data1[N], data2[N];
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  if (nprocs != 3)
    {
      fprintf (stderr,"\nThis code must run on 3 processes.\n");
      MPI_Finalize ();
      return 0;
    }
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  data1[0] = myrank + 10;

  if (myrank == 2)
    MPI_Send (data1, N, MPI_INT, 0, 99, MPI_COMM_WORLD);
  else if (myrank == 1)
    MPI_Send (data1, N, MPI_INT, 0, 99, MPI_COMM_WORLD);
  else
    {
      for (i = 0; i < 2; i++)
        {
          MPI_Probe (MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &status);
          if (status.MPI_SOURCE == 1)
            {
              MPI_Recv (data1, N, MPI_INT, 1, 99, MPI_COMM_WORLD, &status);
              printf ("myrank = %d: data1 = %d\n", myrank, data1[0]);
            }
          else
            {
              MPI_Recv (data2, N, MPI_INT, 2, 99, MPI_COMM_WORLD, &status);
              printf ("myrank = %d: data2 = %d\n", myrank, data2[0]);
            }
        }

    }
  MPI_Finalize ();
  return 0;
}
