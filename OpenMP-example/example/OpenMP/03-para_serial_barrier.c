#include <stdio.h>
#include <unistd.h>
#include <omp.h>
#define N 6


/* 并行区、单线程执行、隐式同步、显示同步*/
int main(int argc, char *argv[])
{
  int i;
#pragma omp parallel num_threads(N)
  { 
    int tid = omp_get_thread_num();

    #pragma omp for 
    for (i = 0; i < N; i++)
      {
	usleep(10000*tid);
      }
    fprintf(stderr, "Thread# %d\n", tid);

    #pragma omp master
    fprintf(stderr, "==== master Thread# %d ======= \n", tid);
    #pragma omp barrier
    
    #pragma omp for nowait
    for (i = 0; i < N; i++)
      {
	usleep(100000*tid);
      }
    fprintf(stderr, "no wait Thread# %d\n", tid);

    #pragma omp barrier
    #pragma omp single
    fprintf(stderr, "single Thread# %d\n", omp_get_thread_num());
  }
  return (0);
}
