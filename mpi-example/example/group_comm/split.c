#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int myrank, nprocs, nprocs_1;
  int rank_sub, myrank_1, rank_orig = -1;
  int i, n, a, b, c;
  int color, key;
  MPI_Comm comm_sub, comm_sub_1;
  MPI_Group group_global, group_sub;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  a = 1;
  b = 0;
  c = 0;

#if 0

  fprintf (stderr, "process id = %d, myrank = %d\n", getpid (), myrank);
#endif

  // 通过color的值区分不同的进程。
  if (myrank % 3 == 0)
    color = 0;
  else if (myrank % 3 == 1)
    color = 1;
  else
    color = 2;

  // 获取全局通信器的进程组。
  MPI_Comm_group (MPI_COMM_WORLD, &group_global);

  // 基于全局通信器创建两个不相交的子通信器，
  // 进程在新通信器中的顺序准循在旧通信器中的顺序。
  // “comm_sub”名字唯一，但代表了三个通信器，
  // 分别包含进程: "0, 3, 6, ..."，"1, 4, 7, ..."和"2, 5, 8, ..."。
  key = 1;
  MPI_Comm_split (MPI_COMM_WORLD, color, key, &comm_sub);

  // 获取三个子通信器的进程组（不同的进程所属的通信器和进程组不同）。
  MPI_Comm_group (comm_sub, &group_sub);

  // 每个进程获取自己在所属的子进程中的序号。
  MPI_Comm_rank (comm_sub, &rank_sub);

#if 1

  fprintf (stderr, "process id = %d, rank_sub = %d\n\n", getpid (), rank_sub);
#endif

  //分别在三个子通信器中执行规约操作。
  MPI_Reduce (&a, &b, 1, MPI_INT, MPI_SUM, 0, comm_sub);

#if 0

  if (rank_sub == 0)
    fprintf (stderr, "b = %d\n\n", b);
#endif

  //在三个子通信器中，分别找出0号进程在MPI_COMM_WORLD中对应的进程的序号，
  //存放在rank_orig中；此时，非0号进程获得的rank_orig仍为-1。
  if (rank_sub == 0)
    {
      MPI_Group_translate_ranks (group_sub, 1, &rank_sub, group_global,
                                 &rank_orig);
      fprintf (stderr, "rank_sub = %d, rank_orig = %d\n", rank_sub,
               rank_orig);
    }

  //在MPI_COMM_WORLD中，找到在三个子通信器中序号为0的进程。
  //基于它们创建一个新的子通信器。
  if (myrank == rank_orig)
    color = 1;
  else
    color = 0;

  key = 1;
  MPI_Comm_split (MPI_COMM_WORLD, color, key, &comm_sub_1);
  MPI_Comm_rank (comm_sub_1, &myrank_1);

  //在只包含上述“三个序号为0”进程的子通信器中，
  //对先前的归约结果进行再次归约。
  if (myrank == rank_orig)
    {
      MPI_Reduce (&b, &c, 1, MPI_INT, MPI_SUM, 0, comm_sub_1);
      if (myrank_1 == 0)
        fprintf (stderr, "myrank_1 = %d, c = %d\n", myrank_1, c);
    }

  if (comm_sub != MPI_COMM_NULL)
    MPI_Comm_free (&comm_sub);
  if (comm_sub_1 != MPI_COMM_NULL)
    MPI_Comm_free (&comm_sub_1);

  MPI_Group_free (&group_global);
  MPI_Group_free (&group_sub);

  MPI_Finalize ();
  return 0;
}
