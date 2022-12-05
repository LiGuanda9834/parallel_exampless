#include <stdio.h>
#include <omp.h>
#define N 6888

int main(int argc, char *argv[])
{
  int i, sum, a[N];

  sum = 0;
#pragma omp parallel default(none) shared(sum, a) private(i) num_threads(8)
  { 
    #pragma omp for
    for (i = 0; i < N; i++)
      {
        a[i] = i;
      }
    #pragma omp for
    for (i = 0; i < N; i++)
    {
      // 设置临界区，保证任一时刻，仅有一个线程执行该代码段。
        #pragma omp critical (sum)
        sum = sum+ a[i];
      }
    printf("Thread# %d sum = %d\n", omp_get_thread_num(), sum);
  }
  return (0);
}
