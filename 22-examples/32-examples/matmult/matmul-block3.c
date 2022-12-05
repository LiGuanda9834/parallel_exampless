/* Matrix multiply: optimization by blocking */

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

#ifndef N
#  define N 1000
#endif

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

static void
matmul_orig(double x[N][N], double y[N][N], double z[N][N])
{
    int i, j, k;

    for (i = 0; i < N; i++)
	for (j = 0; j < N; j++)
	    x[i][j] = 0;

    for (i = 0; i < N; i++) {
	for (k = 0; k < N; k++) {
	    for (j = 0; j < N; j++) {
		x[i][j] += y[i][k] * z[k][j];
	    }
	}
    }
}

static void
matmul_bloc(double x[N][N], double y[N][N], double z[N][N],
		int nb1, int nb2, int nb3)
{
    int i, j, k, ii, jj, kk;

    for (i = 0; i < N; i++)
	for (j = 0; j < N; j++)
	    x[i][j] = 0;

    for (ii = 0; ii < N; ii += nb1) {
	for (kk = 0; kk < N; kk += nb3) {
	    for (jj = 0; jj < N; jj += nb2) {
		for (i = ii; i < min(ii + nb1, N); i++) {
		    for (k = kk; k < min(kk + nb3, N); k++) {
			for (j = jj; j < min(jj + nb2, N); j++) {
			    x[i][j] += y[i][k] * z[k][j];
			}
		    }
		}
	    }
	}
    }
}

static double x[N][N], y[N][N], z[N][N];

int main(int argc, char *argv[])
{
    int i, j, nb1, nb2, nb3;
    struct rusage RU;
    double t, c;

    if (argc != 4) {
	fprintf(stderr, "Usage: %s nb1 nb2 nb3\n", argv[0]);
	fprintf(stderr, "(set NB==0 to test the original loop).\n");
	exit(1);
    }

    nb1 = atoi(argv[1]);
    nb2 = atoi(argv[2]);
    nb3 = atoi(argv[3]);

    srand(12345);
    for (i = 0; i < N; i++)
	for (j = 0; j < N; j++) {
	    y[i][j] = rand()/(double)RAND_MAX;
	    z[i][j] = rand()/(double)RAND_MAX;
	}

    getrusage(RUSAGE_SELF, &RU);
    t = RU.ru_utime.tv_sec + (double)RU.ru_utime.tv_usec * 1e-6;

    if (nb1 <= 0 || nb2 <= 0 || nb3 <= 0)
	matmul_orig(x, y, z);
    else
	matmul_bloc(x, y, z, nb1, nb2, nb2);

    getrusage(RUSAGE_SELF, &RU);
    t = RU.ru_utime.tv_sec + (double)RU.ru_utime.tv_usec * 1e-6 - t;

    c = 0;
    for (i = 0; i < N; i++)
	for (j = 0; j < N; j++)
	    c += x[i][j];

    printf("N: %d   NB: %d %d %d   Time: %0.2lf   Cksum: %0.2lf",
	    N, nb1, nb2, nb3, t, c);
    printf("    Mflops: %0.2lf\n",
	    (2.0 * N * (double)N * (double)N - N * (double)N) / t * 1e-6);

    return 0;
}
