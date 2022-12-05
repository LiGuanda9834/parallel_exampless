#include <stdio.h>
#include <omp.h>
#include <unistd.h>
#define N 10

int main(int argc, char *argv[])
{
  int i, a[N], b[N];
#pragma omp parallel num_threads(3)
  { 
//    #pragma omp for schedule(static, 2)
//    #pragma omp for schedule(static, 1)
//    #pragma omp for schedule(dynamic, 2)
//    #pragma omp for schedule(guided, 1)
    #pragma omp for schedule(runtime)
    for (i = 0; i < N; i++)
      {
        b[i] = i + 10;
	usleep(100*omp_get_thread_num());
        fprintf(stderr, "Thread# %d, %d\n", omp_get_thread_num(), i);
      }
  }
  return (0);
}
