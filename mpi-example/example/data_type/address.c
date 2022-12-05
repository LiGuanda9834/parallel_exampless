#include <stdio.h>
#include <mpi.h>

typedef struct test TEST;
struct test
{
  int int_var;
  char char_var;
  double double_var;
};

int
main (int argc, char *argv[])
{
  int i, nprocs, myrank;
  TEST a[2];
  MPI_Aint address[8];

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  MPI_Address (&a[0], &address[0]);
  MPI_Address (&a[0].double_var, &address[1]);
  MPI_Address (&a[0].char_var, &address[2]);
  MPI_Address (&a[0].int_var, &address[3]);
  MPI_Address (&a[1], &address[4]);
  MPI_Address (&a[1].double_var, &address[5]);
  MPI_Address (&a[1].char_var, &address[6]);
  MPI_Address (&a[1].int_var, &address[7]);

#if 1
  fprintf (stderr,"MPI_BOTTOM = %p\n\n", MPI_BOTTOM);
  fprintf (stderr,"address of a[0] = %p\n", address[0]);
  fprintf (stderr,"address of a[0].double_var = %p\n", address[1]);
  fprintf (stderr,"address of a[0].char_var = %p\n", address[2]);
  fprintf (stderr,"address of a[0].int_var = %p\n\n", address[3]);
  fprintf (stderr,"address of a[1] = %p\n", address[4]);
  fprintf (stderr,"address of a[1].double_var = %p\n", address[5]);
  fprintf (stderr,"address of a[1].char_var = %p\n", address[6]);
  fprintf (stderr,"address of a[1].int_var = %p\n", address[7]);
#endif
  MPI_Finalize ();
  return 0;
}
