/**
 * 将不同的数据打包传输.
 */

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int
main (int argc, char *argv[])
{
  int myrank, remot_proc;

  int size[3], send_size;

  int my_int1[2];
  double my_double[3];
  char my_char;

  int position = 0;
  char *mem_send;

  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

#if 0
  size[0] = 2 * sizeof (int);
  size[1] = 3 * sizeof (double);
  size[2] = sizeof (char);
  if (myrank == 0)
    fprintf (stderr, "myrank: %d, size: %d, %d, %d\n\n\n",
   	     myrank, size[0], size[1], size[2]);
#endif

  //获取打包两个整型数需要的内存空间。
  MPI_Pack_size (2, MPI_INT, MPI_COMM_WORLD, &size[0]);

  //获取打包三个双精度浮点数需要的内存空间。
  MPI_Pack_size (3, MPI_DOUBLE, MPI_COMM_WORLD, &size[1]);

  //获取打包一个字符需要的内存空间。
  MPI_Pack_size (1, MPI_CHAR, MPI_COMM_WORLD, &size[2]);

  //开辟所需的总内存空间。 
  send_size = size[0] + size[1] + size[2];
  mem_send = (char *) malloc (send_size);

#if 1
  if (myrank == 0)
    {
      fprintf (stderr, "myrank: %d, size: %d, %d, %d\n",
               myrank, size[0], size[1], size[2]);
      fprintf (stderr, "myrank: %d, send_size: %d\n", myrank, send_size);
    }
#endif

  remot_proc = (myrank == 0) ? (1) : (0);
  //fprintf (stderr,"myrank: %d, remot_proc: %d\n", myrank, remot_proc);

  if (myrank == 0)
    {
      my_int1[0] = my_int1[1] = myrank + 13;
      my_double[0] = 1.1;
      my_double[1] = 1.1;
      my_double[2] = 1.1;
      my_char = 'c';

      fprintf (stderr, "myrank: %d, position: %d\n", myrank, position);
      //打包两个整型数。
      MPI_Pack (my_int1, 2, MPI_INT, mem_send, send_size, &position,
		MPI_COMM_WORLD);

      fprintf (stderr, "myrank: %d, position: %d\n", myrank, position);
      //打包三个双精度浮点数。
      MPI_Pack (my_double, 3, MPI_DOUBLE, mem_send, send_size, &position,
		MPI_COMM_WORLD);

      fprintf (stderr, "myrank: %d, position: %d\n", myrank, position);
      //打包一个字符。
      MPI_Pack (&my_char, 1, MPI_CHAR, mem_send, send_size, &position,
		MPI_COMM_WORLD);

      fprintf (stderr, "myrank: %d, position: %d\n", myrank, position);

      MPI_Send (mem_send, position, MPI_PACKED, remot_proc, 99,
		MPI_COMM_WORLD);
    }
  else
    {
      MPI_Recv (mem_send, send_size, MPI_PACKED, remot_proc, 99,
		MPI_COMM_WORLD, &status);

      //解包两个整型数。
      MPI_Unpack (mem_send, send_size, &position, my_int1, 2,
		  MPI_INT, MPI_COMM_WORLD);

      //解包三个双精度浮点数。
      MPI_Unpack (mem_send, send_size, &position, my_double, 3,
		  MPI_DOUBLE, MPI_COMM_WORLD);

      //解包一个字符。
      MPI_Unpack (mem_send, send_size, &position, &my_char, 1,
		  MPI_CHAR, MPI_COMM_WORLD);

      fprintf (stderr,"myrank: %d, position: %d\n", myrank, position);

      fprintf (stderr, "myrank %d: my_int1 = %d, %d\n", myrank, my_int1[0],
	       my_int1[1]);
      fprintf (stderr, "myrank %d: my_double = %f, %f, %f\n", myrank,
	       my_double[0], my_double[1], my_double[2]);
      fprintf (stderr, "myrank %d: my_char = %c\n", myrank, my_char);
    }

  free (mem_send);

  MPI_Finalize ();
  return 0;
}
