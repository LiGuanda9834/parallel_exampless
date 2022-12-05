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

  // ͨ��color��ֵ���ֲ�ͬ�Ľ��̡�
  if (myrank % 3 == 0)
    color = 0;
  else if (myrank % 3 == 1)
    color = 1;
  else
    color = 2;

  // ��ȡȫ��ͨ�����Ľ����顣
  MPI_Comm_group (MPI_COMM_WORLD, &group_global);

  // ����ȫ��ͨ���������������ཻ����ͨ������
  // ��������ͨ�����е�˳��׼ѭ�ھ�ͨ�����е�˳��
  // ��comm_sub������Ψһ��������������ͨ������
  // �ֱ��������: "0, 3, 6, ..."��"1, 4, 7, ..."��"2, 5, 8, ..."��
  key = 1;
  MPI_Comm_split (MPI_COMM_WORLD, color, key, &comm_sub);

  // ��ȡ������ͨ�����Ľ����飨��ͬ�Ľ���������ͨ�����ͽ����鲻ͬ����
  MPI_Comm_group (comm_sub, &group_sub);

  // ÿ�����̻�ȡ�Լ����������ӽ����е���š�
  MPI_Comm_rank (comm_sub, &rank_sub);

#if 1

  fprintf (stderr, "process id = %d, rank_sub = %d\n\n", getpid (), rank_sub);
#endif

  //�ֱ���������ͨ������ִ�й�Լ������
  MPI_Reduce (&a, &b, 1, MPI_INT, MPI_SUM, 0, comm_sub);

#if 0

  if (rank_sub == 0)
    fprintf (stderr, "b = %d\n\n", b);
#endif

  //��������ͨ�����У��ֱ��ҳ�0�Ž�����MPI_COMM_WORLD�ж�Ӧ�Ľ��̵���ţ�
  //�����rank_orig�У���ʱ����0�Ž��̻�õ�rank_orig��Ϊ-1��
  if (rank_sub == 0)
    {
      MPI_Group_translate_ranks (group_sub, 1, &rank_sub, group_global,
                                 &rank_orig);
      fprintf (stderr, "rank_sub = %d, rank_orig = %d\n", rank_sub,
               rank_orig);
    }

  //��MPI_COMM_WORLD�У��ҵ���������ͨ���������Ϊ0�Ľ��̡�
  //�������Ǵ���һ���µ���ͨ������
  if (myrank == rank_orig)
    color = 1;
  else
    color = 0;

  key = 1;
  MPI_Comm_split (MPI_COMM_WORLD, color, key, &comm_sub_1);
  MPI_Comm_rank (comm_sub_1, &myrank_1);

  //��ֻ�����������������Ϊ0�����̵���ͨ�����У�
  //����ǰ�Ĺ�Լ��������ٴι�Լ��
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
