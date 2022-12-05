#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int i, j, k, m, myrank, nprocs, begin, upper, down;
  int local_scale, global_scale;
  int *a, *b, *c;
  MPI_Datatype col_type, matrix_type;
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  if (myrank == 0)
    {
      fprintf (stderr, "Enter the scale of the matrix M: ");
      scanf ("%d", &global_scale);

      if (global_scale % nprocs != 0)
        {
          fprintf (stderr, "Wrong M: %d\n", global_scale);
          MPI_Abort (MPI_COMM_WORLD, __LINE__);
          return 0;
        }
    }

  MPI_Bcast (&global_scale, 1, MPI_INT, 0, MPI_COMM_WORLD);

  local_scale = global_scale / nprocs;
 
  a = (int *) calloc (local_scale * global_scale, sizeof (int));
  b = (int *) calloc (local_scale, sizeof (int));
  c = (int *) calloc (local_scale, sizeof (int));

  for (i = 0; i < local_scale * global_scale; i++)
    {
      a[i] = i;
    }
  for(i=0;i<local_scale;i++)
  {
      b[i] = 1;
      c[i] = 0;
  }

  upper = (myrank + nprocs - 1) % nprocs;
  down = (myrank + 1) % nprocs;

  begin = myrank * local_scale;
  for (j = 0; j < local_scale; j++)
      for (m = 0; m < local_scale; m++)
        c[j] = c[j] + a[begin + j * global_scale + m] * b[m];
  
  for (i = 1; i < nprocs; i++)
    {
      MPI_Sendrecv_replace (b, local_scale, MPI_INT, 
                            upper, 99, down, 99, MPI_COMM_WORLD, &status);


      begin = ((down + i -1) % nprocs) * local_scale;
      for (j = 0; j < local_scale; j++)
          for (m = 0; m < local_scale; m++)
            c[j] = c[j] + a[begin + j * global_scale + m] * b[m];
    }

  sleep(myrank);
  fprintf (stderr, "######\nmyrank = %d\n", myrank);
  for (i = 0; i < local_scale; i++)
    {
        fprintf (stderr, "%d, ", c[i]);
      fprintf (stderr, "\n");
    }
 
  free (a);
  free (b);
  free (c);
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize ();

  return (0);
}
