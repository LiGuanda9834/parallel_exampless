#include <stdio.h>
#include <unistd.h>
#include <omp.h>
#define N 6

int main(int argc, char *argv[])
{
  int i, a[N], b[N], c[N], d[N];

#pragma omp parallel default(none) shared(a,b,c,d) private(i)
  {
#pragma omp sections nowait
    {
      usleep(100*omp_get_thread_num());
#pragma omp section
      {
        double tmp;
        tmp = 0.0;
        for (i = 0; i < N; i++)
          {
            a[i] = i;
            b[i] = a[i];
            printf("Thread# %d, a: %d, b: %d\n", 
                    omp_get_thread_num(), a[i], b[i]);
          }    
      }
#pragma omp section
      {
        double aaa;
        aaa = 0.1;
        for (i = 0; i < N; i++)
          {
            c[i] = i;
            d[i] = c[i];
            printf("--Thread# %d, c: %d, d: %d\n", 
                    omp_get_thread_num(), c[i], d[i]);
          } 
      }
    }
  }      

  return (0);
}

