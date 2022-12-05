#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <mpi.h>

#define N 2

int
main (int argc, char *argv[])
{
  int myrank, nprocs;
  int i, n;
  int ranges[N][3];
  int rank_global;
  int size_incl, rank_incl, size_excl, rank_excl;
  MPI_Group group_global, group_incl, group_excl;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  //printf ("process id = %d, myrank = %d\n", getpid (), myrank);

  // 创建一个包含所有进程的进程组。
  MPI_Comm_group (MPI_COMM_WORLD, &group_global);

  // 定义两个进程子集。

  // 进程子集1：{0, 2, 4, 6}
  ranges[0][0] = 0;
  ranges[0][1] = 7;
  ranges[0][2] = 2;

  // 进程子集2：{8, 11, 14}
  ranges[1][0] = 8;
  ranges[1][1] = 15;
  ranges[1][2] = 3;

  // 创建“仅”包含上述两个进程子集的进程组。
  MPI_Group_range_incl (group_global, N, ranges, &group_incl);

  // 创建“不”包含上述两个进程子集的进程组。
  MPI_Group_range_excl (group_global, N, ranges, &group_excl);

  MPI_Group_size (group_incl, &size_incl);
  MPI_Group_rank (group_incl, &rank_incl);

  MPI_Group_size (group_excl, &size_excl);
  MPI_Group_rank (group_excl, &rank_excl);

#if 1

  if (rank_incl >= 0)
    // printf ("process id = %d, size_incl = %d, rank_incl = %d\n\n",
    //	    getpid (), size_incl, rank_incl);
    {
      MPI_Group_translate_ranks (group_incl, 1,
                                   &rank_incl, group_global, &rank_global);

      printf ("process id = %d, rank_incl = %d, rank_global = %d\n\n",
              getpid (), rank_incl, rank_global);
    }
#else
  if (rank_excl >= 0)
    printf ("process id = %d, size_excl = %d, rank_excl = %d\n\n",
            getpid (), size_excl, rank_excl);
#endif

  MPI_Group_free (&group_global);
  MPI_Group_free (&group_incl);
  MPI_Group_free (&group_excl);
  MPI_Finalize ();
  return 0;
}
