 
#include <stdio.h>
#include <omp.h>
#define M 4
#define N 3

int main(int argc, char *argv[])
{
  int i, j, a[M][N], b[N];
  i=0;
  j=0;
#pragma omp parallel num_threads(4)
  {
/**
 * 用private说明的变量，其在并行块外的值不能传入并行块，
 * 在并行块内计算出的值也不能传出并行块。
 */
#pragma omp for 
    for (i = 0; i < M; i++)
      {
        for (j = 0; j < N; j++)
          {
            b[j] = i;
            a[i][j] = b[j] + 1;
            printf("Thread# %d, a[%d][j%d] = %d, b[%d] = %d\n", 
                omp_get_thread_num(), i, j, a[i][j], i, b[j]);
          }
      }

#pragma omp for private(j)
    for (i = 0; i < M; i++)
      {
        for (j = 0; j < N; j++)
          {
            b[j] = i;
            a[i][j] = b[j] + 1;
            printf("(private j) Thread# %d, a[%d][j%d] = %d, b[%d] = %d\n", 
                omp_get_thread_num(), i, j, a[i][j], i, b[j]);
          }
      }
  }

 printf("\n----Thread# %d, a[%d][%d] = %d, b[%d] = %d\n", 
                omp_get_thread_num(), i, j, a[i][j], i, b[j]);
  return (0);
}
