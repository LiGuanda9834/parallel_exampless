/**
 * 为多个类型不同, 数目不同, 相对偏移量不同
 * 的数据块定义新数据类型. 数据块之间的相对偏移量以字节为尺度.
 */

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

/**
 * 利用MPI_BOTTOM定义新数据类型。
 */

int
main (int argc, char *argv[])
{
  int i, myrank, remot_proc;
  int recv_count0, recv_count1;
  int type_size;
  MPI_Aint extent;

  double double_var[3];
  char char_var[1];
  int int_var[2];

  int count = 3;
  int lengths[3] = { 3, 1, 2 };
  MPI_Aint displace[3];
  MPI_Datatype types[3];

  MPI_Datatype newtype;
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  MPI_Address (double_var, &displace[0]);
  MPI_Address (char_var, &displace[1]);
  MPI_Address (int_var, &displace[2]);
  if (myrank == 0)
    fprintf (stderr, "myrank %d: displace = %p, %p, %p\n",
    	     myrank, displace[0], displace[1], displace[2]);

  types[0] = MPI_DOUBLE;
  types[1] = MPI_CHAR;
  types[2] = MPI_INT;

  MPI_Type_struct (3, lengths, displace, types, &newtype);
  MPI_Type_commit (&newtype);

  remot_proc = (myrank == 0) ? (1) : (0);

  if (myrank == 0)
    {
      double_var[0] = double_var[1] = double_var[2]  = 10.1;
      char_var[0] = 'c';
      int_var[0] = int_var[1] = 10;
      MPI_Send (MPI_BOTTOM, 1, newtype, remot_proc, 99, MPI_COMM_WORLD);
    }
  else
    {
      MPI_Recv (MPI_BOTTOM, 1, newtype, remot_proc, 99, MPI_COMM_WORLD, &status);
    }

#if 1
  if (myrank == 1)
    {
      MPI_Get_elements (&status, newtype, &recv_count0);
      MPI_Get_count (&status, newtype, &recv_count1);
      fprintf (stderr, "myrank %d: recv_count0 = %d\n", myrank, recv_count0);
      fprintf (stderr, "myrank %d: recv_count1 = %d\n", myrank, recv_count1);

      fprintf (stderr, "myrank %d: double_var = %f, %f, %f\n", myrank,
	       double_var[0], double_var[1], double_var[2]);
      fprintf (stderr, "myrank %d: char_var = %c\n", myrank, char_var[0]);
      fprintf (stderr, "myrank %d: int_var = %d, %d\n", 
               myrank, int_var[0], int_var[1]);
    }
#endif

  MPI_Type_free (&newtype);
  MPI_Finalize ();
  return 0;
}
