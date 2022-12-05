#include "cannon.h"
void
out_mat_info (MAT_INFO * mat_info)
{
  char m_info[] = "info/mat_info";
  char infoname[100];
  FILE *m_fp = NULL;

  sprintf (infoname, "%s.%d", m_info, mat_info->myrank);
  if ((m_fp = fopen (infoname, "w")) == NULL)
    {
      printf ("cannot open %s\n", infoname);
      return;
    }

  fprintf (m_fp, "mat_info->sub_m: %d\n", mat_info->sub_m);
  fprintf (m_fp, "mat_info->sub_k: %d\n", mat_info->sub_k);
  fprintf (m_fp, "mat_info->sub_n: %d\n", mat_info->sub_n);
  fprintf (m_fp, "mat_info->n_proc_row: %d\n", mat_info->n_proc_row);
  fprintf (m_fp, "mat_info->n_proc_col: %d\n", mat_info->n_proc_col);
  fprintf (m_fp, "mat_info->lcm: %d\n", mat_info->lcm);
  fprintf (m_fp, "mat_info->n_sub_mat_row: %d\n", mat_info->n_sub_mat_row);
  fprintf (m_fp, "mat_info->n_sub_mat_col: %d\n", mat_info->n_sub_mat_col);
  fprintf (m_fp, "mat_info->myrank: %d\n", mat_info->myrank);
  fprintf (m_fp, "mat_info->leftrank: %d\n", mat_info->leftrank);
  fprintf (m_fp, "mat_info->rightrank: %d\n", mat_info->rightrank);
  fprintf (m_fp, "mat_info->uprank: %d\n", mat_info->uprank);
  fprintf (m_fp, "mat_info->downrank: %d\n", mat_info->downrank);
  fprintf (m_fp, "mat_info->mycoord: %d  %d\n", mat_info->mycoords[0],
	   mat_info->mycoords[1]);
  fclose (m_fp);
}

void
out_local_mat (MAT_INFO * mat_info, LOCAL_MAT * local_mat, char *mat)
{
  int i, j, k, sub_x, sub_y;
  char m_info[] = "info/local_mat";
  char infoname[100];
  FILE *m_fp = NULL;

  sprintf (infoname, "%s_%s.%d", m_info, mat, mat_info->myrank);
  if ((m_fp = fopen (infoname, "w")) == NULL)
    {
      printf ("cannot open %s\n", infoname);
      return;
    }
  if (mat == "A")
    {
      sub_x = mat_info->sub_m;
      sub_y = mat_info->sub_k;
    }
  else if (mat == "B")
    {
      sub_x = mat_info->sub_k;
      sub_y = mat_info->sub_n;
    }
  else if (mat == "C")
    {
      sub_x = mat_info->sub_m;
      sub_y = mat_info->sub_n;
    }
  else
    {
      fprintf (stderr, "Wrong mat: %s\n", mat);
      exit;
    }

  fprintf (m_fp, "sub_x: %d.  sub_y: %d\n", sub_x, sub_y);
  for (i = 0; i < local_mat->n_sub_mat; i++)
    {
      fprintf (m_fp, "%s->sub_mat[%d]:\n", mat, i);
      fprintf (m_fp, "local_coords: (%d %d), global_coords: (%d %d)",
	       local_mat->local_coords[i][0], local_mat->local_coords[i][1],
	       local_mat->global_coords[i][0],
	       local_mat->global_coords[i][1]);
      fprintf (m_fp, "\n[");
      for (j = 0; j < sub_x; j++)
	{
	  fprintf (m_fp, "\n ");
	  for (k = 0; k < sub_y; k++)
	    fprintf (m_fp, "%f  ", local_mat->sub_mat[i][j * sub_y + k]);
	}
      fprintf (m_fp, "\n];\n");
    }
  fclose (m_fp);
}

void
out_sub_mat_coords (MAT_INFO *mat_info, LOCAL_MAT * A, LOCAL_MAT * B)
{
  int i, j, k, sub_x, sub_y;
  static int step;
  char m_info[] = "info/sub_mat_coords";
  char infoname[100];
  FILE *m_fp = NULL;

  sprintf (infoname, "%s.%d", m_info, mat_info->myrank);
  if ((m_fp = fopen (infoname, "a+")) == NULL)
    {
      printf ("cannot open %s\n", infoname);
      return;
    }
  if (step == 0)
    fprintf (m_fp, "######## align matrix\n");
  else
    fprintf (m_fp, "######## shift %d\n", step);
  step++;
  for (i = 0; i < A->n_sub_mat; i++)
    {
      fprintf (m_fp, "A[%d]--B[%d]\n", (int)A->sub_mat[i][0],
	       (int)B->sub_mat[i][0]);
    }
  fclose (m_fp);
}
