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
  int i, n, result;
  int *ranks, *ranks_old, *ranks_new;
  int size_incl, rank_incl, size_excl, rank_excl, size_new, rank_new;
  MPI_Group group_global, group_incl, group_excl, group_new;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

#if 0

  fprintf (stderr,"process id = %d, myrank = %d\n", getpid (), myrank);
#endif

  // ��ȡȫ��ͨ�����Ľ����顣
  MPI_Comm_group (MPI_COMM_WORLD, &group_global);

  n = nprocs / 2;
  ranks = (int *) calloc (n, sizeof (int));

  // ����ranks�д�ŵ�ǰ�������ż�����̺š�
  for (i = 0; i < n; i++)
    ranks[i] = 2 * i;

  // ��������group_global���ѳ��������ཻ���ӽ����飺group_incl��group_excl��
  // ���̺�Ϊż���Ľ��̹����½�����group_incl��
  MPI_Group_incl (group_global, n, ranks, &group_incl);

  // ���̺�Ϊ�����Ľ��̹����½�����group_excl��
  MPI_Group_excl (group_global, n, ranks, &group_excl);

  MPI_Group_size (group_incl, &size_incl);
  MPI_Group_rank (group_incl, &rank_incl);

  MPI_Group_size (group_excl, &size_excl);
  MPI_Group_rank (group_excl, &rank_excl);

#if 0

  if (rank_incl >= 0)
    fprintf (stderr,"process id = %d, size_incl = %d, rank_incl = %d\n\n",
             getpid (), size_incl, rank_incl);

  if (rank_excl >= 0)
    fprintf (stderr,"process id = %d, size_excl = %d, rank_excl = %d\n\n",
             getpid (), size_excl, rank_excl);
#endif

  //�Ƚ������ӽ������Ƿ���ͬ��
  MPI_Group_compare (group_incl, group_excl, &result);

#if 0

  if (result == MPI_IDENT)
    fprintf (stderr,"process id = %d, group_incl == group_excl\n", getpid ());
  else if (result == MPI_SIMILAR)
    fprintf (stderr,"process id = %d, group_incl =? group_excl\n", getpid ());
  else if (result == MPI_UNEQUAL)
    fprintf (stderr,"process id = %d, group_incl != group_excl\n", getpid ());
  else
    fprintf (stderr,"Wrong result: %d\n", result);
#endif

  //�������ӽ�����ϲ�Ϊһ���µĽ����顣
  MPI_Group_union (group_incl, group_excl, &group_new);

  //�Ƚ�ԭ����ȫ�ֽ�������ºϳɵĽ������Ƿ���ͬ��
  MPI_Group_compare (group_global, group_new, &result);

#if 0

  if (result == MPI_IDENT)
    fprintf (stderr,"process id = %d, group_global == group_new\n", getpid ());
  else if (result == MPI_SIMILAR)
    fprintf (stderr,"process id = %d, group_global =? group_new\n", getpid ());
  else if (result == MPI_UNEQUAL)
    fprintf (stderr,"process id = %d, group_global != group_new\n", getpid ());
  else
    fprintf (stderr,"Wrong result: %d\n", result);
#endif

  MPI_Group_size (group_new, &size_new);
  MPI_Group_rank (group_new, &rank_new);
  // fprintf (stderr,"process id = %d, size_new = %d, rank_new = %d\n\n",
  //	  getpid (), size_new, rank_new);

  ranks_old = (int *) calloc (nprocs, sizeof (int));
  ranks_new = (int *) calloc (nprocs, sizeof (int));

  for (i = 0; i < nprocs; i++)
    ranks_old[i] = i;

  MPI_Group_translate_ranks (group_global, nprocs, ranks_old, group_new,
                             ranks_new);

#if 1

  fprintf (stderr,"process id = %d, myrank = %d, my new rank = %d\n\n",
           getpid (), myrank, ranks_new[myrank]);
#endif

  MPI_Group_free (&group_global);
  MPI_Group_free (&group_incl);
  MPI_Group_free (&group_excl);
  MPI_Group_free (&group_new);
  free (ranks);
  MPI_Finalize ();
  return 0;
}
