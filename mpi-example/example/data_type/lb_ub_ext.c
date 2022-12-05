#include <stdio.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int i, nprocs, myrank;
  int size[4], pack_size[3];
  MPI_Aint extent[4], lb_disp[4], ub_disp[4];

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

#if 0
  MPI_Type_size (MPI_CHAR, &size[0]);
  MPI_Type_size (MPI_INT, &size[1]);
  MPI_Type_size (MPI_FLOAT, &size[2]);
  MPI_Type_size (MPI_DOUBLE, &size[3]);

  fprintf (stderr,"myrank: %d\n", myrank);
  fprintf (stderr,"size of MPI_CHAR = %d\n", size[0]);
  fprintf (stderr,"size of MPI_INT = %d\n", size[1]);
  fprintf (stderr,"size of MPI_FLOAT = %d\n", size[2]);
  fprintf (stderr,"size of MPI_DOUBLE = %d\n", size[3]);
#endif
  
#if 0
  MPI_Type_extent (MPI_CHAR, &extent[0]);
  MPI_Type_extent (MPI_INT, &extent[1]);
  MPI_Type_extent (MPI_FLOAT, &extent[2]);
  MPI_Type_extent (MPI_DOUBLE, &extent[3]);

  fprintf (stderr,"\n");
  fprintf (stderr,"extent of MPI_CHAR = %d\n", extent[0]);
  fprintf (stderr,"extent of MPI_INT = %d\n", extent[1]);
  fprintf (stderr,"extent of MPI_FLOAT = %d\n", extent[2]);
  fprintf (stderr,"extent of MPI_DOUBEL = %d\n", extent[3]);
#endif

#if 0
  MPI_Type_lb (MPI_CHAR, &lb_disp[0]);
  MPI_Type_lb (MPI_INT, &lb_disp[1]);
  MPI_Type_lb (MPI_FLOAT, &lb_disp[2]);
  MPI_Type_lb (MPI_DOUBLE, &lb_disp[3]);

  fprintf (stderr,"\n");
  fprintf (stderr,"lb_disp of MPI_CHAR= %d\n", lb_disp[0]);
  fprintf (stderr,"lb_disp of MPI_INT = %d\n", lb_disp[1]);
  fprintf (stderr,"lb_disp of MPI_FLOAT = %d\n", lb_disp[2]);
  fprintf (stderr,"lb_disp of MPI_DOUBLE = %d\n", lb_disp[3]);
#endif

#if 0
  MPI_Type_ub (MPI_CHAR, &ub_disp[0]);
  MPI_Type_ub (MPI_INT, &ub_disp[1]);
  MPI_Type_ub (MPI_FLOAT, &ub_disp[2]);
  MPI_Type_ub (MPI_DOUBLE, &ub_disp[3]);

  fprintf (stderr,"\n");
  fprintf (stderr,"ub_disp of MPI_CHAR= %d\n", ub_disp[0]);
  fprintf (stderr,"ub_disp of MPI_INT = %d\n", ub_disp[1]);
  fprintf (stderr,"ub_disp of MPI_FLOAT = %d\n", ub_disp[2]);
  fprintf (stderr,"ub_disp of MPI_DOUBLE = %d\n", ub_disp[3]);
#endif

#if 1
  for (i = 0; i < 3; i++)
    {
      MPI_Pack_size (i + 1, MPI_INT, MPI_COMM_WORLD, &pack_size[i]);
      fprintf (stderr,"pack_size[%d] = %d\n", i, pack_size[i]);
    }
#endif
  MPI_Finalize ();
  return 0;
}
