#include <stdio.h>
#include <omp.h>
#define M 3
#define N 2

int a[M][N], b[N];
#pragma omp threadprivate(b) //全局变量，为每个线程复制一个数组b的副本

int main(int argc, char *argv[])
{
  int i, j;

  for (i = 0; i < N; i++)
    {
      b[i] = 10;
    }
#pragma omp parallel private(i, j) copyin(b)
  { 
    #pragma omp for
    for (i = 0; i < M; i++)
      {
        for (j = 0; j < N; j++)
          {
            b[j] += i;
          }
        
        for (j = 0; j < N; j++)
          {
            a[i][j] = b[j];
            fprintf(stderr, "Thread# %d, a[%d][%d] = %d\n",
                    omp_get_thread_num(), i, j, a[i][j]);
          }
      }

  }
  return (0);
}
