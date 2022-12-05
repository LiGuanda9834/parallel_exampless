/**
 * 为多个类型不同, 数目不同, 相对偏移量不同
 * 的数据块定义新数据类型. 数据块之间的相对偏移量以字节为尺度.
 */

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N 3

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
  TEST a, b;

  int count = 3;
  int lengths[3] = { 1, 1, 1 };
  MPI_Aint displace[3];
  MPI_Datatype types[3];

  MPI_Datatype newtype;
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

#if 0
  displace[0] = 0;
  displace[1] = displace[0] + sizeof (double);
  displace[2] = displace[1] + sizeof (char);
  fprintf (stderr, "myrank %d: displace = %d, %d, %d\n",
	   myrank, displace[0], displace[1], displace[2]);
#endif

#if 1
  MPI_Address (&a.double_var, &displace[0]);
  MPI_Address (&a.char_var, &displace[1]);
  MPI_Address (&a.int_var, &displace[2]);
  displace[2] = displace[2] - displace[0];
  displace[1] = displace[1] - displace[0];
  displace[0] = 0;
  if (myrank == 0)
    fprintf (stderr, "myrank %d: displace = %d, %d, %d\n",
    	     myrank, displace[0], displace[1], displace[2]);
#endif

  types[0] = MPI_DOUBLE;
  types[1] = MPI_CHAR;
  types[2] = MPI_INT;

  MPI_Type_struct (3, lengths, displace, types, &newtype);
  MPI_Type_commit (&newtype);

#if 1
  MPI_Type_size (newtype, &type_size);
  MPI_Type_extent (newtype, &extent);
  if (myrank == 0)
    fprintf (stderr, "size=%d, ext=%d\n",
    	     type_size, extent);
#endif

  remot_proc = (myrank == 0) ? (1) : (0);

  if (myrank == 0)
    {
      a.double_var = 10.1;
      a.char_var = 'c';
      a.int_var = 10;
      MPI_Send (&a, 1, newtype, remot_proc, 99, MPI_COMM_WORLD);
    }
  else
    {
      MPI_Recv (&b, 1, newtype, remot_proc, 99, MPI_COMM_WORLD, &status);
    }

#if 1
  if (myrank == 1)
    {
      MPI_Get_elements (&status, newtype, &recv_count0);
      MPI_Get_count (&status, newtype, &recv_count1);
      fprintf (stderr, "myrank %d: recv_count0 = %d\n", myrank, recv_count0);
      fprintf (stderr, "myrank %d: recv_count1 = %d\n", myrank, recv_count1);

      fprintf (stderr, "myrank %d: b.double_var = %f\n", myrank, b.double_var);
      fprintf (stderr, "myrank %d: b.char_var = %c\n", myrank, b.char_var);
      fprintf (stderr, "myrank %d: b.int_var = %d\n", myrank, b.int_var);
    }
#endif

  MPI_Type_free (&newtype);
  MPI_Finalize ();
  return 0;
}
