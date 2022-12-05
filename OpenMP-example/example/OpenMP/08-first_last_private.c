#include <stdio.h>
#include <omp.h>
#define N 6

int main(int argc, char *argv[])
{
  int i, a[N], b[N], c;
 
#pragma omp parallel
  {
/**
 * 用private说明的变量，其在并行块外的值不能传入并行块，
 * 在并行块内计算出的值也不能传出并行块。为解决这个问题，
 * 可使用firstprivate，和lastprivate。
 */
#pragma omp single
    for (i = 0; i < N; i++)
      {
       a[i] = 10;
      }

    b[N-1]=100;

#pragma omp for
//#pragma omp for private(i,a,b)
//#pragma omp for firstprivate(a,b)
//#pragma omp for firstprivate(a,b) lastprivate(b)
    for (i = 1; i < N; i++)
      {
        b[i] = a[i-1] + i;
	a[i] = b[i];
        fprintf(stderr, "Thread# %d, a[%d]: %d, b[%d]: %d\n", 
                omp_get_thread_num(), i-1, a[i-1], i, b[i]);
      }

    c = b[N - 1];
    fprintf(stderr, "Thread# %d, c: %d\n", omp_get_thread_num(), c);
  }
  return (0);
}
