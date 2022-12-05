#include "cannon.h"

MAT_INFO *
init_mat_info (int myrank, int nprocs)
{
  int i, global_m, global_n, global_k, lcm;
  int newmyrank, mycoords[2];
  int uprank, downrank, leftrank, rightrank;
  int dims[2], periods[2];
  MPI_Comm new_comm;
  char tmp[20];
  FILE *m_fp;
  char init_file[50] = "init_file";
  MAT_INFO *mat_info;

  mat_info = calloc (1, sizeof (MAT_INFO));
  assert (mat_info);


  if ((m_fp = fopen (init_file, "r")) == NULL)
    {
      fprintf (stderr, "cannot open %s\n", init_file);
      exit (1);
    }
  fgets (tmp, 20, m_fp);
  mat_info->global_m = atoi (tmp);
  fgets (tmp, 20, m_fp);
  mat_info->global_k = atoi (tmp);
  fgets (tmp, 20, m_fp);
  mat_info->global_n = atoi (tmp);
  fgets (tmp, 20, m_fp);
  mat_info->n_proc_row = atoi (tmp);
  fgets (tmp, 20, m_fp);
  mat_info->n_proc_col = atoi (tmp);

  if (mat_info->n_proc_row * mat_info->n_proc_col != nprocs)
    {
      fprintf (stderr, "(n_proc_row * n_proc_col) != nprocs\n");
      exit (1);
    }

  mat_info->lcm = lcm = get_lcm (mat_info->n_proc_row, mat_info->n_proc_col);
  if ((mat_info->global_m % lcm != 0) || (mat_info->global_k % lcm != 0)
      || (mat_info->global_n % lcm != 0))
    {
      fprintf (stderr,
	       "global_m or global_k or global_n can't be divided by lcm: %d\n",
	       lcm);
      exit (1);
    }

  dims[0] = mat_info->n_proc_row;
  dims[1] = mat_info->n_proc_col;
  periods[0] = periods[1] = 1;

  MPI_Cart_create (MPI_COMM_WORLD, 2, dims, periods, 1, &new_comm);
  MPI_Comm_rank (new_comm, &newmyrank);
  MPI_Cart_coords (new_comm, newmyrank, 2, mycoords);
  MPI_Cart_shift (new_comm, 0, -1, &downrank, &uprank);
  MPI_Cart_shift (new_comm, 1, -1, &rightrank, &leftrank);

  mat_info->new_comm = new_comm;
  mat_info->myrank = myrank;
  mat_info->leftrank = leftrank;
  mat_info->rightrank = rightrank;
  mat_info->uprank = uprank;
  mat_info->downrank = downrank;
  mat_info->mycoords[0] = mycoords[0];
  mat_info->mycoords[1] = mycoords[1];

  if (mat_info->myrank != newmyrank)
    {
      fprintf (stderr, "mat_info->myrank: %d != newmyrank: %d\n",
	       mat_info->myrank, newmyrank);
      exit (1);
    }
  mat_info->sub_m = mat_info->global_m / mat_info->lcm;
  mat_info->sub_k = mat_info->global_k / mat_info->lcm;
  mat_info->sub_n = mat_info->global_n / mat_info->lcm;
  /*
     (sub_m, sub_k): the size of the sub-matrixs in A.
     (sub_k, sub_n): the size of the sub-matrixs in B.
     (sub_m, sub_n): the size of the sub-matirxs in C.
   */
  mat_info->n_sub_mat_row = mat_info->lcm / mat_info->n_proc_row;
  mat_info->n_sub_mat_col = mat_info->lcm / mat_info->n_proc_col;
  /*
     n_sub_mat_row: the number of the sub-matrixs in row direction in one proc.
     n_sub_mat_col: the number of the sub-matrixs in col direction in one proc.
     n_sub_mat_row*n_sub_mat_col: the number of the sub-matrixs in one proc.
   */
  fclose (m_fp);
  return (mat_info);
}

void
init_mat_AB (MAT_INFO * mat_info, LOCAL_MAT * A, LOCAL_MAT * B)
{
  int i, j, k;
  int sub_m = mat_info->sub_m;
  int sub_k = mat_info->sub_k;
  int sub_n = mat_info->sub_n;

  for (i = 0; i < A->n_sub_mat; i++)
    {
      for (j = 0; j < sub_m; j++)
	for (k = 0; k < sub_k; k++)
	  A->sub_mat[i][j * sub_k + k] =
	    (A->global_coords[i][0] + 1) * 10 + (A->global_coords[i][1] + 1);
      for (j = 0; j < sub_k; j++)
	for (k = 0; k < sub_n; k++)
	  B->sub_mat[i][j * sub_n + k] =
	    (B->global_coords[i][0] + 1) * 10 + (B->global_coords[i][1] + 1);
    }
}
