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

  // ����ͨ������
  MPI_Comm_dup (MPI_COMM_WORLD, &comm_new_0);

  // �Ƚ��¾�ͨ������
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

  // ��ȡȫ��ͨ�����Ľ����顣
  MPI_Comm_group (MPI_COMM_WORLD, &group_global);

  n = nprocs / 2;
  ranks = (int *) calloc (n, sizeof (int));

  // ����ranks�д�ŵ�ǰ�������ż�����̺š�
  for (i = 0; i < n; i++)
    ranks[i] = 2 * i;

  // ��ȫ�ֽ����鴴���������ཻ���ӽ����顣
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
    fprintf (stderr, "process id = %d, size_incl = %d, rank_incl = %d\n\n",
             getpid (), size_incl, rank_incl);
  if (rank_excl >= 0)
    fprintf (stderr, "process id = %d, size_excl = %d, rank_excl = %d\n\n",
             getpid (), size_excl, rank_excl);

#endif

  //��ȫ��ͨ�����Լ���ͨ������һ���ӽ����鴴��һ����ͨ������
  MPI_Comm_create (MPI_COMM_WORLD, group_incl, &comm_incl);
  if (comm_incl != MPI_COMM_NULL)
    {
      //����ͨ����comm_incl��ִ�й�Լ������
      MPI_Reduce (&nprocs, &nprocs_sum, 1, MPI_INT, MPI_SUM, 0, comm_incl);

      MPI_Comm_rank (comm_incl, &rank_incl);
      if (rank_incl == 0)
        {
          //����ͨ����comm_incl�У��ҳ�0�Ž�����MPI_COMM_WORLD�ж�Ӧ�Ľ��̵���ţ�
          //�����rank_new_0�С�
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

  //��ȫ��ͨ�����Լ���ͨ������һ���ӽ����鴴��һ���½����顣
  MPI_Comm_create (MPI_COMM_WORLD, group_excl, &comm_excl);
  if (comm_excl != MPI_COMM_NULL)
    {
      //����ͨ����comm_excl��ִ�й�Լ������
      MPI_Reduce (&nprocs, &nprocs_sum, 1, MPI_INT, MPI_SUM, 0, comm_excl);
      MPI_Comm_rank (comm_excl, &rank_excl);
      if (rank_excl == 0)
        {
          //����ͨ����comm_excl�У��ҳ�0�Ž�����MPI_COMM_WORLD�ж�Ӧ�Ľ��̵���ţ�
          //�����rank_new_1�С�
          MPI_Group_translate_ranks (group_excl, 1, &rank_excl, group_global,
                                     &rank_new_1);
          fprintf (stderr, "rank_excl = %d, rank_new_1 = %d\n", rank_excl,
                   rank_new_1);
        }
    }

  //��������ͨ�����ĸ����̼����ͨ�š�
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
