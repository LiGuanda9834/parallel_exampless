#include <stdio.h>
#include <omp.h>
#define N 10

int main()
{
  int k, sum;
  sum=0;
#pragma omp parallel for 
  for (k = 0; k < N; k++)
    {
      sum = sum + k;
      fprintf(stderr, "Thread# %d: sum = %d\n", omp_get_thread_num(), sum);
    }

  fprintf(stderr, "# %d: sum = %d\n", omp_get_num_threads(), sum);

  sum=0;
#pragma omp parallel for reduction(+:sum)
  for (k = 0; k < N; k++)
    {
      sum = sum + k;
      fprintf(stderr, "##Thread# %d: sum = %d\n", omp_get_thread_num(), sum);
    }

  fprintf(stderr, "# %d: sum = %d\n", omp_get_num_threads(), sum);
}
