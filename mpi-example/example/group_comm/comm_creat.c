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
  int i, n, sum_temp, nprocs_sum = -1;
  int *ranks;
  int size_incl, rank_incl, size_excl, rank_excl, result;
  int rank_new_0, rank_new_1;
  MPI_Group group_global, group_incl, group_excl;
  MPI_Comm comm_new_0, comm_incl, comm_excl;

  MPI_Request request;
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  //fprintf (stderr, "process id = %d, myrank = %d\n", getpid (), myrank);

  // 复制通信器。
  MPI_Comm_dup (MPI_COMM_WORLD, &comm_new_0);

  // 比较新旧通信器。
  MPI_Comm_compare (MPI_COMM_WORLD, comm_new_0, &result);

#if 0

  if (result == MPI_IDENT)
    fprintf (stderr, "process id = %d, comm_new == MPI_COMM_WORLD\n",
             getpid ());
  else if (result == MPI_CONGRUENT)
    fprintf (stderr, "process id = %d, comm_new ?= MPI_COMM_WORLD\n",
             getpid ());
  else if (result == MPI_SIMILAR)
    fprintf (stderr, "process id = %d, comm_new =? MPI_COMM_WORLD\n",
             getpid ());
  else if (result == MPI_UNEQUAL)
    fprintf (stderr, "process id = %d, comm_new != MPI_COMM_WORLD\n",
             getpid ());
  else
    fprintf (stderr, "Wrong result: %d\n", result);
#endif

  // 获取全局通信器的进程组。
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

#if 0

  if (rank_incl >= 0)
    fprintf (stderr, "process id = %d, size_incl = %d, rank_incl = %d\n\n",
             getpid (), size_incl, rank_incl);
  if (rank_excl >= 0)
    fprintf (stderr, "process id = %d, size_excl = %d, rank_excl = %d\n\n",
             getpid (), size_excl, rank_excl);

#endif

  //由全局通信器以及该通信器的一个子进程组创建一个新通信器。
  MPI_Comm_create (MPI_COMM_WORLD, group_incl, &comm_incl);
  if (comm_incl != MPI_COMM_NULL)
    {
      //在新通信器comm_incl中执行规约操作。
      MPI_Reduce (&nprocs, &nprocs_sum, 1, MPI_INT, MPI_SUM, 0, comm_incl);

      MPI_Comm_rank (comm_incl, &rank_incl);
      if (rank_incl == 0)
        {
          //在子通信器comm_incl中，找出0号进程在MPI_COMM_WORLD中对应的进程的序号，
          //存放在rank_new_0中。
          MPI_Group_translate_ranks (group_incl, 1, &rank_incl, group_global,
                                     &rank_new_0);
          fprintf (stderr, "rank_incl = %d, rank_new_0 = %d\n", rank_incl,
                   rank_new_0);
        }
    }

#if 0
  fprintf (stderr, "process id = %d, rank_incl = %d, sum of nprocs = %d\n\n",
           getpid (), rank_incl, nprocs_sum);
#endif

  //由全局通信器以及该通信器的一个子进程组创建一个新进程组。
  MPI_Comm_create (MPI_COMM_WORLD, group_excl, &comm_excl);
  if (comm_excl != MPI_COMM_NULL)
    {
      //在子通信器comm_excl中执行规约操作。
      MPI_Reduce (&nprocs, &nprocs_sum, 1, MPI_INT, MPI_SUM, 0, comm_excl);
      MPI_Comm_rank (comm_excl, &rank_excl);
      if (rank_excl == 0)
        {
          //在子通信器comm_excl中，找出0号进程在MPI_COMM_WORLD中对应的进程的序号，
          //存放在rank_new_1中。
          MPI_Group_translate_ranks (group_excl, 1, &rank_excl, group_global,
                                     &rank_new_1);
          fprintf (stderr, "rank_excl = %d, rank_new_1 = %d\n", rank_excl,
                   rank_new_1);
        }
    }

  //在两个子通信器的根进程间进行通信。
  if (myrank == rank_new_0)
    MPI_Irecv (&sum_temp, 1, MPI_INT, 1, 88, MPI_COMM_WORLD, &request);
  else if (myrank == rank_new_1)
    MPI_Isend (&nprocs_sum, 1, MPI_INT, 0, 88, MPI_COMM_WORLD, &request);

  MPI_Wait (&request, &status);

  if (myrank == rank_new_0)
    {
      nprocs_sum = nprocs_sum + sum_temp;
      fprintf (stderr, "myrank = %d, nprocs_sum = %d\n", myrank,
               nprocs_sum);
    }

  MPI_Group_free (&group_global);
  MPI_Group_free (&group_incl);
  MPI_Group_free (&group_excl);
  free (ranks);

  if (comm_incl != MPI_COMM_NULL)
    MPI_Comm_free (&comm_incl);
  if (comm_excl != MPI_COMM_NULL)
    MPI_Comm_free (&comm_excl);

  MPI_Finalize ();
  return 0;
}
