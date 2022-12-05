#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

/**
 * 利用新数据类型再次定义新数据类型。
 */

typedef struct test TEST;
struct test
{
  double double_var;
  char char_var;
  int int_var;
};

int
main (int argc, char *argv[])
{
  int i, myrank, remot_proc;
  int recv_count0, recv_count1;
  int type_size;
  MPI_Aint extent;
  TEST a, b[4], c[4];

  int count = 3;
  int lengths[3] = { 1, 1, 1 };
  MPI_Aint displace[3];
  MPI_Datatype types[3];

  int newlen[2];
  MPI_Aint newdisp[2];

  MPI_Datatype newtype, newertype;
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  MPI_Address (&a.double_var, &displace[0]);
  MPI_Address (&a.char_var, &displace[1]);
  MPI_Address (&a.int_var, &displace[2]);
  displace[2] = displace[2] - displace[0];
  displace[1] = displace[1] - displace[0];
  displace[0] = 0;
#if 0
  if (myrank == 0)
    fprintf (stderr, "myrank %d: displace = %d, %d, %d\n",
	     myrank, displace[0], displace[1], displace[2]);
#endif

  types[0] = MPI_DOUBLE;
  types[1] = MPI_CHAR;
  types[2] = MPI_INT;

  MPI_Type_struct (3, lengths, displace, types, &newtype);
  MPI_Type_commit (&newtype);

  newlen[0] = 2;
  newlen[1] = 2;

  MPI_Address (&b[0], &newdisp[0]);
  MPI_Address (&b[2], &newdisp[1]);
  newdisp[1] = newdisp[1] - newdisp[0];
  newdisp[0] = 0;

  // 用自定义的数据类型重新定义数据类型.
  MPI_Type_hindexed (2, newlen, newdisp, newtype, &newertype);
  MPI_Type_commit (&newertype);

#if 1
  MPI_Type_size (newertype, &type_size);
  MPI_Type_extent (newertype, &extent);

  if (myrank == 0)
    fprintf (stderr, "size=%d, extent=%d\n", type_size, extent);
#endif

  remot_proc = (myrank == 0) ? (1) : (0);

  if (myrank == 0)
    {
      for (i = 0; i < 4; i++)
	{
	  b[i].double_var = 10.1 + i;
	  b[i].char_var = 'c';
	  b[i].int_var = 10 + i;
	}
      MPI_Send (&b, 1, newertype, remot_proc, 99, MPI_COMM_WORLD);
    }
  else
    MPI_Recv (&c, 1, newertype, remot_proc, 99, MPI_COMM_WORLD, &status);

#if 1
  if (myrank == 1)
    {
      MPI_Get_elements (&status, newertype, &recv_count0);
      MPI_Get_count (&status, newertype, &recv_count1);
      fprintf (stderr, "myrank %d: recv_count0 = %d\n", myrank, recv_count0);
      fprintf (stderr, "myrank %d: recv_count1 = %d\n", myrank, recv_count1);
      for (i = 0; i < 4; i++)
	{
	  fprintf (stderr, "myrank %d: c[%d].double_var = %f\n", myrank, i,
		   c[i].double_var);
	  fprintf (stderr, "myrank %d: c[%d].char_var = %c\n", myrank, i,
		   c[i].char_var);
	  fprintf (stderr, "myrank %d: c[%d].int_var = %d\n", myrank, i,
		   c[i].int_var);
	}
    }
#endif

  MPI_Type_free (&newtype);
  MPI_Type_free (&newertype);
  MPI_Finalize ();
  return 0;
}
