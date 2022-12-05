/**
 * 非阻塞通信.
 */

#include <stdio.h>
#include <mpi.h>

//#define N 1
#define N 6553

int
main (int argc, char *argv[])
{
  int myrank, dest, i;
  int my_int[N], get_int[N];
  int tmp[N];
  int flag;

  MPI_Request request[2];
  MPI_Status status[2];

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  my_int[0] = (myrank + 1) * (myrank + 1);

  // 确定消息目的地.
  dest = (myrank == 0) ? (1) : (0);

  MPI_Isend (my_int, N, MPI_INT, dest, 99, MPI_COMM_WORLD, &request[1]);
  MPI_Irecv (get_int, N, MPI_INT, dest, 99, MPI_COMM_WORLD, &request[0]);

  // 计算与通信重叠
  fprintf (stderr, "Waitting...... \n"); 
#if 0
  // 不安全操作.
  for (i = 0; i < N; i++)
    my_int[i] = myrank;
  for (i = 0; i < N; i++)
    tmp[i] = get_int[i];
#endif
  
#if 1
  int j;
  for (j = 0; j < 2; ++j) {
  if (request[0] != MPI_REQUEST_NULL) {
  MPI_Test (&request[0], &flag, &status[0]);
  if (flag)
    {
      printf ("myrank = %d: Isend finished.\n", myrank);
      if (request[0] == MPI_REQUEST_NULL) {
        printf ("request[0] is Null.\n");
      }
    }
  else
    {
      printf ("myrank = %d: Isend unfinished.\n", myrank);
    }
  }
  }
#endif

#if 0
  MPI_Testall (2, request, &flag, status);
  if (flag)
    printf ("myrank %d: all request finished.\n", myrank);
  else
    printf ("myrank %d: all request unfinished.\n", myrank);
#endif

#if 0
  MPI_Wait (&request[0], &status[0]);
#endif

#if 0
  MPI_Waitall (2, request, status);
  printf ("myrank %d: my_int = %d, \tget_int = %d\n", myrank,
	  my_int[0], get_int[0]);
#endif

  MPI_Finalize ();
  return 0;
}
