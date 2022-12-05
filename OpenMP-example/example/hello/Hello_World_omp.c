#include <stdio.h>
#include <omp.h>
#define N 10

int main(int argc, char *argv[])
{
  int i;
  printf ("*Hello World! Thread: %d\n",
         omp_get_thread_num());

  #pragma omp parallel for
    for (i = 0; i < N; ++i)
      printf ("Hello World!  Thread: %d, i: %d\n",
              omp_get_thread_num(), i);
   return 0;
}
