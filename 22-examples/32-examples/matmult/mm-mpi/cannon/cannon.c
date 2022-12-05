#include "cannon.h"

static LOCAL_MAT *A;
static LOCAL_MAT *A_tmp;
static LOCAL_MAT *B;
static LOCAL_MAT *B_tmp;
static LOCAL_MAT *C;

double *
get_mem (int size)
{
  int i;
  double *p;
  p = (double *) calloc (size, sizeof (double));

  if (!p)
    {
      fprintf (stderr, "Allocation Error\n");
      exit (1);
    }
  for (i = 0; i < size; i++)
    p[i] = 0.0;
  return p;
}

LOCAL_MAT *
get_local_mat (MAT_INFO * mat_info, int flag)
{
  int i, j, index;
  int n_sub_mat_row = mat_info->n_sub_mat_row;
  int n_sub_mat_col = mat_info->n_sub_mat_col;
  int n_sub_mat = mat_info->n_sub_mat_row * mat_info->n_sub_mat_col;
  int sub_mat_size;
  int mycoords[2];
  LOCAL_MAT *local_mat;

  mycoords[0] = mat_info->mycoords[0];
  mycoords[1] = mat_info->mycoords[1];

  local_mat = calloc (1, sizeof (LOCAL_MAT));
  if (!local_mat)
    {
      fprintf (stderr, "Allocation Error\n");
      exit (1);
    }
  local_mat->n_sub_mat = n_sub_mat;
  local_mat->sub_mat = calloc (n_sub_mat, sizeof (double *));
  assert (local_mat->sub_mat);
  local_mat->local_coords = calloc (n_sub_mat, sizeof (MAT_COORDS));
  assert (local_mat->local_coords);
  local_mat->global_coords = calloc (n_sub_mat, sizeof (MAT_COORDS));
  assert (local_mat->global_coords);

  for (i = 0; i < n_sub_mat; i++)
    {
      switch (flag)
	{
	case 0:
	  sub_mat_size = mat_info->sub_m * mat_info->sub_k;
	  break;
	case 1:
	  sub_mat_size = mat_info->sub_k * mat_info->sub_n;
	  break;
	case 2:
	  sub_mat_size = mat_info->sub_m * mat_info->sub_n;
	  break;
	defult:
	  fprintf (stderr, "Wrong flag.\n");
	  exit (1);
	}
      local_mat->sub_mat[i] = get_mem (sub_mat_size);
    }
  for (i = 0; i < n_sub_mat_row; i++)
    for (j = 0; j < n_sub_mat_col; j++)
      {
	index = i * n_sub_mat_col + j;
	local_mat->local_coords[index][0] = i;
	local_mat->local_coords[index][1] = j;
	local_mat->global_coords[index][0] =
	  i + (mycoords[0] * n_sub_mat_row);
	local_mat->global_coords[index][1] =
	  j + (mycoords[1] * n_sub_mat_col);
      }
  return local_mat;
}

int
get_lcm (int m, int n)
{
  int max, lcm;
  max = MAX (m, n);
  lcm = max;

  while ((lcm % m != 0) || (lcm % n) != 0)
    {
      lcm += max;
    }
  return lcm;
}

int
get_lcm_3 (int m, int k, int n)
{
  int max, lcm;
  max = MAX_3 (m, k, n);
  lcm = max;

  while ((lcm % m != 0) || (lcm % k != 0) || (lcm % n) != 0)
    {
      lcm += max;
    }
  return lcm;
}

void
shift_sub_matrix (MAT_INFO * mat_info, int *dest_proc, int *source_proc,
		  MAT_COORDS * dest_coords, MAT_COORDS * source_coords,
		  MPI_Comm new_comm, int flag)
{
  int i, j, sub_mat_size, myrank = mat_info->myrank;
  double *p;
  LOCAL_MAT *mat, *mat_tmp;
  static MPI_Status *send_status;
  static MPI_Status *recv_status;
  static MPI_Request *send_request;
  static MPI_Request *recv_request;

  mat = flag == 0 ? A : B;
  mat_tmp = flag == 0 ? A_tmp : B_tmp;
  sub_mat_size = flag == 0 ? (mat_info->sub_m * mat_info->sub_k)
    : (mat_info->sub_k * mat_info->sub_n);
  if (!send_status)
    {
      send_status = calloc (mat->n_sub_mat, sizeof (MPI_Status));
      assert (send_status);
      recv_status = calloc (mat->n_sub_mat, sizeof (MPI_Status));
      assert (recv_status);
      send_request = calloc (mat->n_sub_mat, sizeof (MPI_Request));
      assert (send_request);
      recv_request = calloc (mat->n_sub_mat, sizeof (MPI_Request));
      assert (recv_request);
    }

  for (i = 0; i < mat->n_sub_mat; i++)
    {
      if (dest_proc[i] >= 0)
	{
#if 0
	  fprintf (stderr, "shift_sub_matrix: i: %d. myrank: %d\n", i,
		   myrank);
	  fprintf (stderr, "dest_proc: %d. source_proc: %d\n", dest_proc[i],
		   source_proc[i]);
	  fprintf (stderr, "recv_tag: %d. send_tag: %d\n",
		   mat->global_coords[i][0] * mat->global_coords[i][1],
		   source_coords[i][0] * source_coords[i][1]);
#endif
	  MPI_Isend (mat->sub_mat[i], sub_mat_size, MPI_DOUBLE, dest_proc[i],
		     mat->global_coords[i][0] * mat->global_coords[i][1],
		     new_comm, &send_request[i]);
	}
      // fprintf (stderr, "after Isend before Irecv. myrank: %d\n", myrank);
      if (source_proc[i] >= 0)
	{
	  MPI_Irecv (mat_tmp->sub_mat[i], sub_mat_size, MPI_DOUBLE,
		     source_proc[i],
		     source_coords[i][0] * source_coords[i][1],
		     new_comm, &recv_request[i]);
//        fprintf (stderr, "after Irecv. myrank: %d\n", myrank);
	}
    }
  for (i = 0; i < mat->n_sub_mat; i++)
    {
      if (dest_proc[i] >= 0)
	MPI_Wait (&recv_request[i], &recv_status[i]);
      if (source_proc[i] >= 0)
	MPI_Wait (&send_request[i], &send_status[i]);
    }
  for (i = 0; i < mat->n_sub_mat; i++)
    {
      if ((source_proc[i] >= 0))
	{
	  p = mat->sub_mat[i];
	  mat->sub_mat[i] = mat_tmp->sub_mat[i];
	  mat_tmp->sub_mat[i] = p;
	}
    }
}

void
shift_mat (MAT_INFO * mat_info, int flag)
{
  int i, j;
  int myrank = mat_info->myrank;
  int mycoords[2];
  MPI_Comm new_comm = mat_info->new_comm;
  int n_sub_mat_row = mat_info->n_sub_mat_row;
  int n_sub_mat_col = mat_info->n_sub_mat_col;
  MAT_COORDS global_coords;
  static MAT_COORDS *dest_coords;
  static MAT_COORDS *source_coords;
  static int *dest_proc;
  static int *source_proc;

  mycoords[0] = mat_info->mycoords[0];
  mycoords[1] = mat_info->mycoords[1];
  if (!dest_proc)
    {
      dest_proc = calloc (A->n_sub_mat, sizeof (int));
      assert (dest_proc);
      source_proc = calloc (A->n_sub_mat, sizeof (int));
      assert (source_proc);
      dest_coords = calloc (A->n_sub_mat, sizeof (MAT_COORDS));
      assert (dest_coords);
      source_coords = calloc (A->n_sub_mat, sizeof (MAT_COORDS));
      assert (source_coords);
    }
  for (i = 0; i < A->n_sub_mat; i++)
    {
      dest_proc[i] = source_proc[i] = -1;
      dest_coords[i][0] = dest_coords[i][1] = 0;
      source_coords[i][0] = source_coords[i][1] = 0;
    }

  switch (flag)
    {
    case 1:			/* align the matrices A and B */
      for (i = 0; i < A->n_sub_mat; i++)
	{
	  if (A->global_coords[i][0] > 0)
	    {
	      global_coords[0] = A->global_coords[i][0];
	      global_coords[1] = A->global_coords[i][1];
/* get dest sub-matrix and dest proc for sub-matrix in A. */
	      dest_coords[i][0] = global_coords[0];
	      if (global_coords[1] >= global_coords[0])
		dest_coords[i][1] = global_coords[1] - global_coords[0];
	      else
		dest_coords[i][1] = global_coords[1] +
		  (mat_info->lcm - global_coords[0]);
	      dest_proc[i] =
		mat_info->n_proc_col * (dest_coords[i][0] / n_sub_mat_row)
		+ (dest_coords[i][1] / n_sub_mat_col);
/*get source sub-matrix and source proc for sub-matrix in A.*/
	      source_coords[i][0] = global_coords[0];
	      if (global_coords[1] >= (mat_info->lcm - global_coords[0]))
		source_coords[i][1] =
		  global_coords[1] - (mat_info->lcm - global_coords[0]);
	      else
		source_coords[i][1] = global_coords[0] + global_coords[1];
	      source_proc[i] =
		mat_info->n_proc_col * (source_coords[i][0] / n_sub_mat_row)
		+ (source_coords[i][1] / n_sub_mat_col);
	    }
	}
#if 0
      for (i = 0; i < A->n_sub_mat; i++)
	{
	  fprintf (stderr,
		   "\nmy_coords:(%d, %d) dest_coords:(%d, %d) dest_proc[%d]: %d. myrank: %d\n",
		   A->global_coords[i][0], A->global_coords[i][1],
		   dest_coords[i][0], dest_coords[i][1], i, dest_proc[i],
		   myrank);
	  fprintf (stderr,
		   "my_coords:(%d, %d) source_coords:(%d, %d)  source_proc[%d]: %d. myrank: %d\n",
		   A->global_coords[i][0],
		   A->global_coords[i][1], source_coords[i][0],
		   source_coords[i][1], i, source_proc[i], myrank);
	}
#endif
      shift_sub_matrix (mat_info, dest_proc, source_proc, dest_coords,
			source_coords, new_comm, 0);
      for (i = 0; i < A->n_sub_mat; i++)
	{
	  dest_proc[i] = source_proc[i] = -1;
	  dest_coords[i][0] = dest_coords[i][1] = 0;
	  source_coords[i][0] = source_coords[i][1] = 0;
	}

      //fprintf (stderr, "after shift_sub_matrix: A. myrank: %d\n", myrank);
      for (i = 0; i < B->n_sub_mat; i++)
	{
	  if (B->global_coords[i][1] > 0)
	    {
	      global_coords[0] = B->global_coords[i][0];
	      global_coords[1] = B->global_coords[i][1];
/* get dest sub-matrix and dest proc of sub-matrix in B */
	      dest_coords[i][1] = global_coords[1];
	      if (global_coords[0] >= global_coords[1])
		dest_coords[i][0] = global_coords[0] - global_coords[1];
	      else
		dest_coords[i][0] =
		  global_coords[0] + (mat_info->lcm - global_coords[1]);
	      dest_proc[i] =
		mat_info->n_proc_col * (dest_coords[i][0] / n_sub_mat_row)
		+ (dest_coords[i][1] / n_sub_mat_col);
/* get source sub-matrix and source proc of sub-matrix in B */
	      source_coords[i][1] = global_coords[1];
	      if (global_coords[0] >= (mat_info->lcm - global_coords[1]))
		source_coords[i][0] =
		  global_coords[0] - (mat_info->lcm - global_coords[1]);
	      else
		source_coords[i][0] = global_coords[0] + global_coords[1];
	      source_proc[i] =
		mat_info->n_proc_col * (source_coords[i][0] / n_sub_mat_row)
		+ (source_coords[i][1] / n_sub_mat_col);
	    }
	}
#if 0
      for (i = 0; i < B->n_sub_mat; i++)
	{
	  fprintf (stderr,
		   "\nmy_coords:(%d, %d) dest_coords:(%d, %d) dest_proc[%d]: %d. myrank: %d\n",
		   B->global_coords[i][0], B->global_coords[i][1],
		   dest_coords[i][0], dest_coords[i][1], i, dest_proc[i],
		   myrank);
	  fprintf (stderr,
		   "my_coords:(%d, %d) source_coords:(%d, %d)  source_proc[%d]:		   %d. myrank: %d\n",
		   B->global_coords[i][0],
		   B->global_coords[i][1], source_coords[i][0],
		   source_coords[i][1], i, source_proc[i], myrank);
	}
#endif
      shift_sub_matrix (mat_info, dest_proc, source_proc, dest_coords,
			source_coords, new_comm, 1);
      //  fprintf (stderr, "after shift_sub_matrix: B. myrank: %d\n", myrank);
      break;
    case 2:
      for (i = 0; i < A->n_sub_mat; i++)
	{
	  dest_coords[i][0] = A->global_coords[i][0];
	  dest_coords[i][1] = A->global_coords[i][1] - 1;
	  if (dest_coords[i][1] == -1)
	    dest_coords[i][1] = mat_info->lcm - 1;
	  dest_proc[i] =
	    mat_info->n_proc_col * (dest_coords[i][0] / n_sub_mat_row)
	    + (dest_coords[i][1] / n_sub_mat_col);
#if 0
	  if (A->local_coords[i][1] == 0)
	    dest_proc[i] = mat_info->leftrank;
	  else
	    dest_proc[i] = mat_info->myrank;
#endif
	  source_coords[i][0] = A->global_coords[i][0];
	  source_coords[i][1] = A->global_coords[i][1] + 1;
	  if (source_coords[i][1] == mat_info->lcm)
	    source_coords[i][1] = 0;
	  source_proc[i] =
	    mat_info->n_proc_col * (source_coords[i][0] / n_sub_mat_row)
	    + (source_coords[i][1] / n_sub_mat_col);
#if 0
	  if (A->local_coords[i][1] = mat_info->n_sub_mat_col - 1)
	    source_proc[i] = mat_info->rightrank;
	  else
	    source_proc[i] = mat_info->myrank;
#endif
	}
#if 0
      for (i = 0; i < A->n_sub_mat; i++)
	{
	  fprintf (stderr,
		   "\nmy_coords:(%d, %d) dest_coords:(%d, %d) dest_proc[%d]:		   %d. myrank: %d\n",
		   A->global_coords[i][0], A->global_coords[i][1],
		   dest_coords[i][0], dest_coords[i][1], i,
		   dest_proc[i], myrank);
	  fprintf (stderr,
		   "my_coords:(%d, %d) source_coords:(%d, %d)  source_proc[%d]:		   %d. myrank: %d\n",
		   A->global_coords[i][0], A->global_coords[i][1],
		   source_coords[i][0], source_coords[i][1], i,
		   source_proc[i], myrank);
	}
#endif
      shift_sub_matrix (mat_info, dest_proc, source_proc, dest_coords,
			source_coords, new_comm, 0);
      for (i = 0; i < A->n_sub_mat; i++)
	{
	  dest_proc[i] = source_proc[i] = -1;
	  dest_coords[i][0] = dest_coords[i][1] = 0;
	  source_coords[i][0] = source_coords[i][1] = 0;
	}
      for (i = 0; i < B->n_sub_mat; i++)
	{
	  dest_coords[i][0] = B->global_coords[i][0] - 1;
	  if (dest_coords[i][0] == -1)
	    dest_coords[i][0] = mat_info->lcm - 1;
	  dest_coords[i][1] = B->global_coords[i][1];
	  dest_proc[i] =
	    mat_info->n_proc_col * (dest_coords[i][0] / n_sub_mat_row)
	    + (dest_coords[i][1] / n_sub_mat_col);
#if 0
	  if (B->local_coords[i][0] == 0)
	    dest_proc[i] = mat_info->uprank;
	  else
	    dest_proc[i] = mat_info->myrank;
#endif
	  source_coords[i][0] = B->global_coords[i][0] + 1;
	  if (source_coords[i][0] == mat_info->lcm)
	    source_coords[i][0] = 0;
	  source_coords[i][1] = B->global_coords[i][1];
	  source_proc[i] =
	    mat_info->n_proc_col * (source_coords[i][0] / n_sub_mat_row)
	    + (source_coords[i][1] / n_sub_mat_col);
#if 0
	  if (B->local_coords[i][0] == mat_info->n_sub_mat_row - 1)
	    source_proc[i] = mat_info->downrank;
	  else
	    source_proc[i] = mat_info->myrank;
#endif
	}
#if 0
      for (i = 0; i < B->n_sub_mat; i++)
	{
	  fprintf (stderr,
		   "\nmy_coords:(%d, %d) dest_coords:(%d, %d) dest_proc[%d]: 		   %d. myrank: %d\n",
		   B->global_coords[i][0], B->global_coords[i][1],
		   dest_coords[i][0], dest_coords[i][1], i,
		   dest_proc[i], myrank);
	  fprintf (stderr,
		   "my_coords:(%d, %d) source_coords:(%d, %d)  source_proc[%d]:		   %d. myrank: %d\n",
		   B->global_coords[i][0], B->global_coords[i][1],
		   source_coords[i][0], source_coords[i][1], i,
		   source_proc[i], myrank);
	}
#endif
      shift_sub_matrix (mat_info, dest_proc, source_proc, dest_coords,
			source_coords, new_comm, 1);
      break;
    default:
      fprintf (stderr, "Wrong flag: %d\n", flag);
      exit (1);
    }
}

void
matrix_multiply (MAT_INFO * mat_info)
{
  int i, j, k, l;
  int sub_m = mat_info->sub_m;
  int sub_k = mat_info->sub_k;
  int sub_n = mat_info->sub_n;
  int n_sub_mat = A->n_sub_mat;

  for (i = 0; i < n_sub_mat; i++)
    {
      for (j = 0; j < sub_m; j++)
	{
	  for (l = 0; l < sub_n; l++)
	    for (k = 0; k < sub_k; k++)
	      C->sub_mat[i][j * sub_n + l] +=
		A->sub_mat[i][j * sub_k + k] * B->sub_mat[i][k * sub_n + l];
	}
    }
}

int
main (int argc, char *argv[])
{
  int i;
  int myrank, nprocs, newmyrank, mycoords[2];
  int uprank, downrank, leftrank, rightrank;
  int dims[2], periods[2];
  MAT_INFO *mat_info;
  MPI_Comm new_comm;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  mat_info = init_mat_info (myrank, nprocs);

  A = get_local_mat (mat_info, 0);
  A_tmp = get_local_mat (mat_info, 0);
  B = get_local_mat (mat_info, 1);
  B_tmp = get_local_mat (mat_info, 1);
  C = get_local_mat (mat_info, 2);
  
  init_mat_AB (mat_info, A, B);

  shift_mat (mat_info, 1);
  for (i = 0; i < mat_info->lcm; i++)
    {
      matrix_multiply (mat_info);
      out_sub_mat_coords (mat_info, A, B);
      shift_mat (mat_info, 2);
    }
  MPI_Finalize ();
  return (0);
}
