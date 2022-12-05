#include <stdio.h>
#include <unistd.h>
#include <omp.h>

int 
main(int argc, char *argv[])
{
  int tnum, tid, to, pnum;

  // 设置嵌套并行深度为1。
  omp_set_nested(1);

// 指定线程数为2。
#pragma omp parallel num_threads(2)
  {
    // 每个线程获取并打印处理器数.
    pnum = omp_get_num_procs();
    fprintf(stderr, "Thread# %d: pnum = %d\n", omp_get_thread_num(), pnum);

    if (omp_get_thread_num() == 0)
      {
        // 线程0指定自己可以调度的线程数。
        omp_set_num_threads(2);
      }
    else
      {
        // 线程1指定自己可以调度的线程数。
        omp_set_num_threads(4);
      }
    fprintf(stderr, "Thread: %d, sum of all threads = %d, the max of threads num allowed = %d \n", omp_get_thread_num(), omp_get_num_threads(),omp_get_max_threads());

    #pragma omp parallel
    {
      #pragma omp master
      {
        // 线程0和线程1作为主线程打印自己调度的线程数。
	usleep(100*omp_get_num_threads());
        fprintf (stderr, "Thread %d , total thread : %d\n", omp_get_thread_num(), omp_get_num_threads());
      }
    }

      #pragma omp parallel
      {
      	fprintf (stderr, "Count me, %d\n", omp_get_thread_num());
      }

  }

  return(0);
}
