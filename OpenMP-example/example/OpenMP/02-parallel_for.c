#include <stdio.h>
#include <omp.h>
#define N 10


int main(int argc, char *argv[])
{
  int i, a[N], b[N];
  #pragma omp for
    for (i = 0; i < N; i++)
    {
        a[i] = i;
    }
    fprintf(stderr, "---Thread# %d\n", omp_get_thread_num());
#pragma omp parallel num_threads(3)
  { 
    #pragma omp for
    for (i = 0; i < N; i++)
      {
        b[i] = i + 10;
      }
    fprintf(stderr, "Thread# %d\n", omp_get_thread_num());
  }
  return (0);
}

