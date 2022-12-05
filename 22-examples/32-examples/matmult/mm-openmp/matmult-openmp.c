#include "stdio.h"
#include "stdlib.h"
#include <math.h>
#include <sys/time.h>

#define USEBLAS 1

#if USEBLAS
#include "openblas/cblas.h"
#endif

#define N 1000
#define mat(A, i, j)  (*(A+i*N+j))

void
gen_mat(double* A, double* B)
{
    int i,j;
    for(i=0;i<N;i++)
        for(j=0;j<N;j++){
          mat(A,i,j)=-1+2.0*random()/(RAND_MAX+1.0);
          mat(B,i,j)=-1+2.0*random()/(RAND_MAX+1.0);
     }
    return;
}


void
check_result(double* A, double* B, double* C)
{
   double res1, res2;
   int i,j,k;

#if USEBLAS
    struct timeval st, et;
#endif

   res1=0;
   for(i=0;i<N;i++)
       for(j=0;j<N;j++){
          res1=res1+fabs(mat(C,i,j));
          mat(C,i,j)=0.;
   }
#if USEBLAS
    gettimeofday(&st,NULL);
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                N, N, N, 1.0, A, N, B, N, 0.0, C, N);
    gettimeofday(&et,NULL);
 
    printf("BLAS:  time: %0.6lf sec\n", et.tv_sec+et.tv_usec*1e-6-st.tv_sec-st.tv_usec*1e-6);

#else
   for(i=0;i<N;i++)
       for(j=0;j<N;j++)
           for(k=0;k<N;k++)
   {
               mat(C,i,j)=mat(C,i,j)+mat(A,i,k)*mat(B,k,j);
   }
#endif

   res2=0;
   for(i=0;i<N;i++)
       for(j=0;j<N;j++){
          res2=res2+fabs(mat(C,i,j));
   }

   res1-=res2;
   if(fabs(res1/res2) < 1e-10)
       printf("MatMul. passed!\n");
   else
       printf("MatMul. faild %e/%e=%e\n", res1, res2, res1/res2);
}

/* Need to be Opt. */
void
matmul(double *A, double *B, double *C)
{
   int i, j, k;

   for(i=0;i<N;i++)
       for(j=0;j<N;j++){
           mat(C,i,j)=0.;
   }

#pragma omp parallel for private(i,j,k)
        for(i=0;i<N;i++)
      	   for(k=0;k<N;k++)
      	      for(j=0;j<N;j++)
         	{
               mat(C,i,j)=mat(C,i,j)+mat(A,i,k)*mat(B,k,j);
         	}

   return;
}

int main(int argc, char* argv[])
{
    double *A,*B,*C;
    struct timeval st, et;

    A = (double*) calloc( N, N * sizeof(double));
    B = (double*) calloc( N, N * sizeof(double));
    C = (double*) calloc( N, N * sizeof(double));

    gen_mat(A,B);
    printf("Gen matrix finished!\n");
    gettimeofday(&st,NULL);

    matmul(A,B,C);

    gettimeofday(&et,NULL);
/*
    if( et.tv_usec < st.tv_usec )
    {
        et.tv_sec = et.tv_sec - st.tv_sec - 1;
        et.tv_usec = et.tv_usec + (1000000 - st.tv_usec);
    }
    else
    {
        et.tv_sec -= st.tv_sec;
        et.tv_usec -= st.tv_usec;
    }
*/
    
    printf("matrix mult. time: %0.6lf sec\n", et.tv_sec+et.tv_usec*1e-6-st.tv_sec-st.tv_usec*1e-6);

    check_result(A,B,C);

    free(A);
    free(B);
    free(C);
    return 0;
}
