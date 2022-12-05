#include <stdio.h>
#include <omp.h>
//#include <unistd.h>

int main()
{
  int x;
  x = 7;
#pragma omp parallel num_threads(4) shared(x)
//#pragma omp parallel private(x)
  {
    // 定义私有变量y。
    int y;
    y = 3;
    if (omp_get_thread_num() == 0)
      {
        x = -1;
        y = 6;
      }
    else
      {
        x = omp_get_thread_num();
      }
    sleep(4-x);
    fprintf(stderr, "Thread# %d: x = %d, y = %d\n", omp_get_thread_num(), x, y);
  } // end for pragma omp parallle
    fprintf(stderr, "x = %d\n", x);
}
