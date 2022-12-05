/**
 * 为连续的相同类型的数据定义新的数据类型.
 */

#include <stdio.h>
#include <mpi.h>

#define N 1

int
main (int argc, char *argv[])
{
  int myrank, dest;
  int my_int[N * 3], get_int[N * 3];
  int i, flag;
  MPI_Request request[2];
  MPI_Status status[2];
  MPI_Datatype newtype;


  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  //将3个连续的整型定义为新数据类型.
  MPI_Type_contiguous (3, MPI_INT, &newtype);
 
  //向MPI系统提交新数据类型.
  MPI_Type_commit (&newtype);

  dest = (myrank == 0) ? (1) : (0);
  for (i = 0; i < 3; i++)
    my_int[i] = (myrank + 1) * (myrank + 1);

  MPI_Isend (my_int, N, newtype, dest, 99, MPI_COMM_WORLD, &request[1]);
  
  MPI_Irecv (get_int, N, newtype, dest, 99, MPI_COMM_WORLD, &request[0]);
  
 // MPI_Irecv (get_int, N * 3, MPI_INT, dest, 99, MPI_COMM_WORLD, &request[0]);

  MPI_Waitall (2, request, status);

  for (i = 0; i < 3; i++)
    printf ("myrank %d: my_int = %d, \tget_int = %d\n", myrank,
	    my_int[i], get_int[i]);

  //释放新数据类型.
  MPI_Type_free (&newtype);
  MPI_Finalize ();
  return 0;
}
