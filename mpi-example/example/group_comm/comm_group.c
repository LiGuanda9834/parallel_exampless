#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int myrank, nprocs;
  int size_global, size_self, rank_global, rank_self;
  MPI_Group group_global, group_self;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  printf ("process id = %d, myrank = %d\n", getpid (), myrank);

  // 由通信器创建进程组。
  MPI_Comm_group (MPI_COMM_WORLD, &group_global);
  MPI_Comm_group (MPI_COMM_SELF, &group_self);

  // 查询进程组中进程数目。
  MPI_Group_size (group_global, &size_global);
  MPI_Group_size (group_self, &size_self);

  // 每个进程获取自己在进程组中的编号。
  MPI_Group_rank (group_global, &rank_global);
  MPI_Group_rank (group_self, &rank_self);

  printf ("process id = %d, size_global = %d, rank_global = %d\n",
          getpid (), size_global, rank_global);

  printf ("process id = %d, size_self = %d, rank_self = %d\n\n",
          getpid (), size_self, rank_self);

  MPI_Group_free (&group_global);
  MPI_Group_free (&group_self);
  MPI_Finalize ();
  return 0;
}
