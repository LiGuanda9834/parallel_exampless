/**
 * 计算pi.
 */

#include "mpi.h"
#include <stdio.h>
#include <math.h>

double
f (double a)
{
  return (4.0 / (1.0 + a * a));
}

int
main (int argc, char *argv[])
{
  int myrank, nprocs, i, N, n, end;
  double PI25DT = 3.141592653589793238462643;
  double mypi, pi, h, sum, x;
  double startwtime = 0.0, endwtime;
  int namelen;
  char processor_name[MPI_MAX_PROCESSOR_NAME];

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Get_processor_name (processor_name, &namelen);

  // fprintf (stderr, "Process %d on %s\n", myrank, processor_name);

  if (myrank == 0)
    {
      // 设置剖分区域数.
      printf("Enter the number of intervals: ");
      scanf("%d",&N);

      startwtime = MPI_Wtime ();
    }

  // 进程0将剖分区域的数目广播出去.
  MPI_Bcast (&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (N > 0)
    {
      h = 1.0 / (double) N;
      sum = 0.0;
#if 0
      // 以轮循的方式分配任务.
      for (i = myrank + 1; i <= N; i += nprocs)
        {
          x = h * ((double) i - 0.5);
          sum += f (x);
        }
      mypi = h * sum;
#else
      // 以块的方式分配任务.
      n = N / nprocs + 1;
      end = ((myrank + 1) * n) > N? N : ((myrank + 1) * n);
      for (i = myrank * n + 1; i <= end ; i++)
        {
          x = h * ((double) i - 0.5);
          sum += f (x);
        }
      mypi = h * sum;
#endif
      // 将计算结果规约到进程0.
      MPI_Reduce (&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

      if (myrank == 0)
        {
          printf ("pi is approximately %.16f, Error is %.16f\n",
                  pi, fabs (pi - PI25DT));
          endwtime = MPI_Wtime ();
          printf ("wall clock time = %f\n", endwtime - startwtime);
        }
    }
  MPI_Finalize ();

  return 0;
}
