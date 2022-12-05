#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

int
main (int argc, char *argv[])
{
  int i, root, myrank, rank, nprocs;
  MPI_Group global_group, root_group, else_group;
  MPI_Comm else_comm;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  //MPI_Group_size (global_group, &nprocs);
  MPI_Comm_group (MPI_COMM_WORLD, &global_group);

  // 创建仅包含进程0的进程组。
  root = 0;
  MPI_Group_incl (global_group, 1, &root, &root_group);

  // 创建一个进程组，包含除进程0之外所有进程。
  MPI_Group_difference (global_group, root_group, &else_group);
  MPI_Group_size (else_group, &nprocs);

  // 查询进程组else_group中的每个进程在进程组global_group中的编号。
  for (i = 0; i < nprocs; i++)
    {
      MPI_Group_translate_ranks (else_group, 1, &i, global_group, &rank);

      if (myrank == rank)
        fprintf (stderr, " orig rank = %d, current rank = %d\n", myrank, i);
    }

  if (global_group != MPI_GROUP_NULL)
    MPI_Group_free (&global_group);

  if (root_group != MPI_GROUP_NULL)
    MPI_Group_free (&root_group);

  if (else_group != MPI_GROUP_NULL)
    MPI_Group_free (&else_group);

  MPI_Finalize ();
  return 0;
}
