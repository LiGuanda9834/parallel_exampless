#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int i, N, threshold, a[100], b[100];

  if (argc != 3)
    {
      printf
        ("##################################################################\n");
      printf
        ("#                                                                #\n");
      printf
        ("#     Usage: %s N threshold                         #\n", argv[0]);
      printf
        ("#                                                                #\n");
      printf
        ("##################################################################\n");
      return (-1);
    }

  N = atoi(argv[1]);
  threshold = atoi(argv[2]);

  fprintf (stderr, "N: %d, threshold: %d\n", N, threshold);

// 当给定的条件满足时，对指定的代码段进行多线程执行。
// 可指定并发线程的数目。缺省情况下系统将根据硬件条件确定并发线程数目。
#pragma omp parallel if (N > threshold)\
            shared(N,a,b) private(i) num_threads(N/threshold)
  {
#pragma omp for
    for (i = 0; i < N; i++)
      {
        a[i] = i;
        b[i] = i;
        fprintf(stderr, "Thread# %d, i = %d\n", omp_get_thread_num(), i);
      }
  }
  return (0);
}
