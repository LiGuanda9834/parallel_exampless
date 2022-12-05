/**
 * 所有进程读写不同文件。
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int i, myrank, nprocs;
  char file[20];
  FILE *fp;

  MPI_Init (&argc, &argv);

  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  // 各进程用函数sprintf()生成不同的输出文件名
  // 其它可使用函数: snprintf(), swprintf().
  sprintf (file, "file_%d", 0);

  //fprintf (stderr, "myrank = %d, %s\n", myrank, file);

  fp = fopen (file, "r");
  fscanf (fp, "%d", &myrank);
  fclose (fp);
  fprintf (stderr, "myrank = %d\n", myrank);

  return (0);
}
