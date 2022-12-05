#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAX(a,b)     ((a) > (b) ? (a) : (b))
#define MAX_3(a,b,c) ((MAX(a,b)) > c ? (MAX(a,b)) : (c))

typedef int MAT_COORDS[2];

typedef struct local_mat LOCAL_MAT;
struct local_mat
{
  int n_sub_mat;
  double **sub_mat;
  MAT_COORDS *local_coords;
  MAT_COORDS *global_coords;
};

typedef struct mat_info MAT_INFO;
struct mat_info
{
  int global_m;
  int global_k;
  int global_n;
  int sub_m;
  int sub_k;
  int sub_n;
  int n_proc_row;
  int n_proc_col;
  int lcm;
  int n_sub_mat_row;
  int n_sub_mat_col;
  int myrank;
  int leftrank;
  int rightrank;
  int uprank;
  int downrank;
  int mycoords[2];
  MPI_Comm new_comm;
};

MAT_INFO * init_mat_info (int myrank, int nprocs);

void init_mat_AB (MAT_INFO * mat_info, LOCAL_MAT * A, LOCAL_MAT * B);

void out_mat_info (MAT_INFO * mat_info);

void out_local_mat (MAT_INFO * mat_info, LOCAL_MAT *local_mat, char *mat);

void out_sub_mat_coords (MAT_INFO * mat_info, LOCAL_MAT * A, LOCAL_MAT * B);
