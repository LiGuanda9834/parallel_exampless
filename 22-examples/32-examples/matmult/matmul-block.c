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

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))


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
 
    printf("OPEN:  time: %0.6lf sec\n", et.tv_sec+et.tv_usec*1e-6-st.tv_sec-st.tv_usec*1e-6);

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
matmulIJK(double *A, double *B, double *C)
{
   int i, j, k;
   struct timeval st, et;

   for(i=0;i<N;i++)
       for(j=0;j<N;j++){
           mat(C,i,j)=0.;
   }

   gettimeofday(&st,NULL);

   for(i=0;i<N;i++)
       for(j=0;j<N;j++)
           for(k=0;k<N;k++)
           {
               mat(C,i,j)=mat(C,i,j)+mat(A,i,k)*mat(B,k,j);
           }

    gettimeofday(&et,NULL);
    printf("matrix mult. time I-J-K : %0.6lf sec\n", et.tv_sec+et.tv_usec*1e-6-st.tv_sec-st.tv_usec*1e-6);

   return;
}


void
matmulIJK_B(double *A, double *B, double *C, int nb)
{
   int i, j, k, jj, kk;
   double r;
   struct timeval st, et;

   for(i=0;i<N;i++)
       for(j=0;j<N;j++){
           mat(C,i,j)=0.;
   }

   gettimeofday(&st,NULL);


   for (jj = 0; jj < N; jj += nb) {
      for (kk = 0; kk < N; kk += nb) {
	   for (i = 0; i < N; i++) {
               for (j = jj; j < min(jj + nb, N); j++) {
                    for (k = kk; k < min(kk + nb, N); k++) {
		    	mat(C,i,j)=mat(C,i,j)+mat(A,i,k) * mat(B,k,j);
		    }
           }
	   }
      }
   }

    gettimeofday(&et,NULL);
    printf("matrix mult. time I-J-K-B: %0.6lf sec\n", et.tv_sec+et.tv_usec*1e-6-st.tv_sec-st.tv_usec*1e-6);

   return;
}

void
matmulIJK_BE(double *A, double *B, double *C)
{
   int i, j, k, jj, kk, nb=4;
   double r;
   struct timeval st, et;

   for(i=0;i<N;i++)
       for(j=0;j<N;j++){
           mat(C,i,j)=0.;
   }

   gettimeofday(&st,NULL);


	   for (i = 0; i < N; i++) {
               for (j = 0; j < N; j+=nb) {
                    for (k = 0; k < N; k+=nb) {
		    	mat(C,i,j)+=mat(A,i,k) * mat(B,k,j);
		    	mat(C,i,j)+=mat(A,i,k+1) * mat(B,k+1,j);
		    	mat(C,i,j)+=mat(A,i,k+2) * mat(B,k+2,j);
		    	mat(C,i,j)+=mat(A,i,k+3) * mat(B,k+3,j);
		    	mat(C,i,j+1)+=mat(A,i,k) * mat(B,k,j+1);
		    	mat(C,i,j+1)+=mat(A,i,k+1) * mat(B,k+1,j);
		    	mat(C,i,j+1)+=mat(A,i,k+2) * mat(B,k+2,j);
		    	mat(C,i,j+1)+=mat(A,i,k+3) * mat(B,k+3,j);
		    	mat(C,i,j+2)+=mat(A,i,k) * mat(B,k,j+1);
		    	mat(C,i,j+2)+=mat(A,i,k+1) * mat(B,k+1,j);
		    	mat(C,i,j+2)+=mat(A,i,k+2) * mat(B,k+2,j);
		    	mat(C,i,j+2)+=mat(A,i,k+3) * mat(B,k+3,j);
		    	mat(C,i,j+3)+=mat(A,i,k) * mat(B,k,j+1);
		    	mat(C,i,j+3)+=mat(A,i,k+1) * mat(B,k+1,j);
		    	mat(C,i,j+3)+=mat(A,i,k+2) * mat(B,k+2,j);
		    	mat(C,i,j+3)+=mat(A,i,k+3) * mat(B,k+3,j);
		    }
           }
   	}

    gettimeofday(&et,NULL);
    printf("matrix mult. time I-J-K-BE: %0.6lf sec\n", et.tv_sec+et.tv_usec*1e-6-st.tv_sec-st.tv_usec*1e-6);

   return;
}

void
matmulIJK_B3(double *A, double *B, double *C, int nbi, int nbj, int nbk)
{
   int i, j, k, ii,jj, kk;
   double r;
   struct timeval st, et;

   for(i=0;i<N;i++)
       for(j=0;j<N;j++){
           mat(C,i,j)=0.;
   }

   gettimeofday(&st,NULL);


   for (ii = 0; ii < N; ii += nbi) {
       for (jj = 0; jj < N; jj += nbj) {
          for (kk = 0; kk < N; kk += nbk) {
            for (i = ii; i < min(ii + nbi, N); i++) {
               for (j = jj; j < min(jj + nbj, N); j++) {
                    for (k = kk; k < min(kk + nbk, N); k++) {
		    	mat(C,i,j)=mat(C,i,j)+mat(A,i,k) * mat(B,k,j);
		    }
	       }
	   }
      	}
      }
   }

    gettimeofday(&et,NULL);
    printf("matrix mult. time I-J-K-B3: %0.6lf sec\n", et.tv_sec+et.tv_usec*1e-6-st.tv_sec-st.tv_usec*1e-6);

   return;
}


void
matmulIKJ(double *A, double *B, double *C)
{
   int i, j, k;
   struct timeval st, et;

   for(i=0;i<N;i++)
       for(j=0;j<N;j++){
           mat(C,i,j)=0.;
   }

   gettimeofday(&st,NULL);

   for(i=0;i<N;i++)
           for(k=0;k<N;k++)
       for(j=0;j<N;j++)
           {
               mat(C,i,j)=mat(C,i,j)+mat(A,i,k)*mat(B,k,j);
           }

    gettimeofday(&et,NULL);
    printf("matrix mult. time I-K-J: %0.6lf sec\n", et.tv_sec+et.tv_usec*1e-6-st.tv_sec-st.tv_usec*1e-6);

   return;
}

void
matmulIKJ_B(double *A, double *B, double *C, int nb)
{
   int i, j, k, ii, jj, kk;
   double r;
   struct timeval st, et;

   for(i=0;i<N;i++)
       for(j=0;j<N;j++){
           mat(C,i,j)=0.;
   }

   gettimeofday(&st,NULL);


   for (kk = 0; kk < N; kk += nb) {
   	for (jj = 0; jj < N; jj += nb) {
	   for (i = 0; i < N; i++) {
               for (k = kk; k < min(kk + nb, N); k++) {
               	    for (j = jj; j < min(jj + nb, N); j++) {
			    mat(C,i,j)=mat(C,i,j)+mat(A,i,k) * mat(B,k,j);
		    }
           	}
	   }
      }
   }

    gettimeofday(&et,NULL);
    printf("matrix mult. time I-K-J-B: %0.6lf sec\n", et.tv_sec+et.tv_usec*1e-6-st.tv_sec-st.tv_usec*1e-6);

   return;
}

void
matmulIKJ_B3(double *A, double *B, double *C, int nbi, int nbj, int nbk)
{
   int i, j, k, ii,jj, kk;
   double r;
   struct timeval st, et;

   for(i=0;i<N;i++)
       for(j=0;j<N;j++){
           mat(C,i,j)=0.;
   }

   gettimeofday(&st,NULL);


   for (ii = 0; ii < N; ii += nbi) {
          for (kk = 0; kk < N; kk += nbk) {
       for (jj = 0; jj < N; jj += nbj) {
            for (i = ii; i < min(ii + nbi, N); i++) {
                    for (k = kk; k < min(kk + nbk, N); k++) {
               for (j = jj; j < min(jj + nbj, N); j++) {
		    	mat(C,i,j)=mat(C,i,j)+mat(A,i,k) * mat(B,k,j);
		    }
	       }
	   }
      	}
      }
   }

    gettimeofday(&et,NULL);
    printf("matrix mult. time I-K-J-B3: %0.6lf sec\n", et.tv_sec+et.tv_usec*1e-6-st.tv_sec-st.tv_usec*1e-6);

   return;
}


int main(int argc, char* argv[])
{
    double *A,*B,*C;
    int nb1,nb2,nb3;

    A = (double*) calloc( N, N * sizeof(double));
    B = (double*) calloc( N, N * sizeof(double));
    C = (double*) calloc( N, N * sizeof(double));


    if (argc != 4) {
        fprintf(stderr, "Usage: %s NBi NBj NBk\n", argv[0]);
        exit(1);
    }

    nb1 = atoi(argv[1]);
    nb2 = atoi(argv[2]);
    nb3 = atoi(argv[3]);

    gen_mat(A,B);
    printf("Gen matrix finished!\n");

    matmulIJK(A,B,C);
    matmulIJK_B(A,B,C, nb1);
    matmulIJK_B3(A,B,C,nb1, nb2, nb3);
    matmulIJK_BE(A,B,C);
    matmulIKJ(A,B,C);
    matmulIKJ_B(A,B,C, nb1);
    matmulIKJ_B3(A,B,C,nb1, nb2, nb3);



    check_result(A,B,C);

    free(A);
    free(B);
    free(C);
    return 0;
}
