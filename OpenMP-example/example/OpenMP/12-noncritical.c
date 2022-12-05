#include <stdio.h>
#include <omp.h>
#define N 6888
#define no 8

int main(int argc, char *argv[])
{
  int i, sum[8], a[N];
  int s=0;

  for (i = 0; i < no; i++)
    sum[i] = 0;

#pragma omp parallel default(none) shared(sum, a) private(i) num_threads(8)
  { 
    int tid;
    tid=omp_get_thread_num();
    #pragma omp for
    for (i = 0; i < N; i++)
      {
        a[i] = i;
      }
    #pragma omp for
    for (i = 0; i < N; i++)
      {
       // 设置临界区，保证任一时刻，仅有一个线程执行该代码段。
       // #pragma omp critical (NAME)
        sum[tid] += a[i];
      }
  }

  for (i = 0; i < 8; i++)
    s += sum[i];

    printf("Thread# %d sum = %d\n", omp_get_thread_num(), s);
  return (0);
}
