#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int myrank, nprocs;
  int i, n;
  int *ranks;
  int size_incl, rank_incl, size_excl, rank_excl;
  MPI_Group group_global, group_incl, group_excl;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  printf ("process id = %d, myrank = %d\n", getpid (), myrank);

  MPI_Comm_group (MPI_COMM_WORLD, &group_global);

  n = nprocs / 2;
  ranks = (int *) calloc (n, sizeof (int));

  // 数组ranks中存放当前进程组的偶数进程号。
  for (i = 0; i < n; i++)
    ranks[i] = 2 * i;

  // 由全局进程组创建两个不相交的子进程组。
  // 进程号为偶数的进程构成新进程组group_incl。
  MPI_Group_incl (group_global, n, ranks, &group_incl);

  // 进程号为奇数的进程构成新进程组group_excl。
  MPI_Group_excl (group_global, n, ranks, &group_excl);

  MPI_Group_size (group_incl, &size_incl);
  MPI_Group_rank (group_incl, &rank_incl);

  MPI_Group_size (group_excl, &size_excl);
  MPI_Group_rank (group_excl, &rank_excl);

  if (rank_incl >= 0)
    printf ("process id = %d, size_incl = %d, rank_incl = %d\n\n",
            getpid (), size_incl, rank_incl);

  if (rank_excl >= 0)
    printf ("process id = %d, size_excl = %d, rank_excl = %d\n\n",
            getpid (), size_excl, rank_excl);

  MPI_Group_free (&group_global);
  MPI_Group_free (&group_incl);
  MPI_Group_free (&group_excl);
  free (ranks);
  MPI_Finalize ();
  return 0;
}
