/* �������2: ȷ������������� */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define _XOPEN_SOURCE 500
#include <unistd.h>	/* usleep */
#include <mpi.h>

#define map(i) ((i) % nprocs)

static void
print_buffer(const char *msg, int n, const int *buffer, int myrank, int nprocs)
/* print contents of buffer[] on all processes */
{
    int i, p;

    if (msg != NULL) {
	MPI_Barrier(MPI_COMM_WORLD);
	if (myrank == 0)
	    printf("%s\n", msg);
	fflush(stdout);
	usleep(125000);
	MPI_Barrier(MPI_COMM_WORLD);
    }

    for (p = 0; p < nprocs; p++) {
	MPI_Barrier(MPI_COMM_WORLD);
	if (myrank != p)
	    continue;
	printf("    proc %d: n = %d,", myrank, n);
	for (i = 0; i < n; i++)
	    printf(" %d", buffer[i]);
	printf("\n");
	fflush(stdout);
	usleep(125000);
    }
}

static int
comp(const void *p1, const void *p2)
/* �Ƚ����������� qsort() */
{
    return *((const int *)p1) - *((const int *)p2);
}

static int *rbuffer, *ranks;

static int
comp2(const void *p1, const void *p2)
/* �Ƚ� (rbuffer[*p1], ranks[*p1]) �� (rbuffer[*p2], ranks[*p1]) */
{
    int i  = rbuffer[*((const int *)p1)] - rbuffer[*((const int *)p2)];
    return i != 0 ? i : ranks[*((const int *)p1)] - ranks[*((const int *)p2)];
}

int
main(int argc, char *argv[])
{
    int n, myrank, nprocs, *obj;
    int N, *owner;
    int i, j;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    if (argc != 2) {
	if (myrank == 0)
	    fprintf(stderr, "Usage: %s n\n", argv[0]);
	MPI_Finalize();
	return 1;
    }
    n = atoi(argv[1]);
    assert(n > 0);
    obj = malloc(n * sizeof(*obj));
    owner = malloc(n * sizeof(*owner));
    assert(obj != NULL);

    /* ������� obj[] �е����ݣ� �� 0 �� n + n ֮�� */
    srand(getpid());
    for (i = 0; i < n; i++)
	obj[i] = rand() % (n + n);
    /* �� obj[] ���� */
    qsort(obj, n, sizeof(*obj), comp);

#ifdef DEBUG
    print_buffer("Initial values in obj[]:", n, obj, myrank, nprocs);
#endif

    /* delete duplicate entries */
    for (i = j = 0; i < n; j++) {
	int a = obj[i];
	if (j != i)
	    obj[j] = a;
	while (++i < n && obj[i] == a);
    }
    n = j;

    print_buffer("Contents of obj[]:", n, obj, myrank, nprocs);

    /* Now obj[] contains sorted, mutually distinct integers */
    
    /* ���� owner[]��owner[i] ���� obj[i] ���������̺š�
     * 
     * ��ѡ���㷨��
     *	1. �����н����е�obj[]ͬʱ�ռ�������0�����⣺����0���ڴ���ܲ���
     *	2. ѭ���㷨�����⣺��Ҫ nprocs ���
     *	3. �� obj[i] ���͵����� map(obj[i]) */

{
    /* �㷨3��ʵ�� */
    int slen, scnts[nprocs], sdsps[nprocs];
    int rlen, rcnts[nprocs], rdsps[nprocs];
    int j, p, *sbuffer, *ordering;

    /* �����跢�͵������̵����ݸ�����cnts[p] �����跢�͵�����p�����ݸ��� */
    for (p = 0; p < nprocs; p++)
	scnts[p] = 0;			/* ��ʼ�� cnts[] */
    for (i = 0; i < n; i++) {
	p = map(obj[i]);
	scnts[p]++;
    }

    /* �����̽��� scnts[] */
    MPI_Alltoall(scnts, 1, MPI_INT, rcnts, 1, MPI_INT, MPI_COMM_WORLD);

    /* ���� sdsps[] �� rdsps[] */
    slen = rlen = 0;
    for (p = 0; p < nprocs; p++) {
	sdsps[p] = slen;
	rdsps[p] = rlen;
	slen += scnts[p];
	rlen += rcnts[p];
    }
    assert(slen == n);

    /* ���䷢�ͺͽ��ջ����� */
    sbuffer = malloc(slen * sizeof(*sbuffer));
    rbuffer = malloc(rlen * sizeof(*rbuffer));
    ranks = malloc(rlen * sizeof(*ranks));
    ordering = malloc(rlen * sizeof(*ordering));

    /* ׼�����ͻ����� */
    for (i = 0; i < n; i++) {
	p = map(obj[i]);
	sbuffer[sdsps[p]++] = obj[i];
    }

    /* �ָ� sdsps[] ���� */
    for (p = 0; p < nprocs; p++)
	sdsps[p] -= scnts[p];

    /* �������ݵ�Ŀ�Ľ��� */
    MPI_Alltoallv(sbuffer, scnts, sdsps, MPI_INT,
		  rbuffer, rcnts, rdsps, MPI_INT, MPI_COMM_WORLD);

    /* ��ʼ�� ranks[i] Ϊ rbuffer[i] ��Դ���� */
    for (p = 0; p < nprocs; p++)
	for (i = rdsps[p]; i < rdsps[p] + rcnts[p]; i++)
	    ranks[i] = p;

    /* ���򣺼���ordering[] ʹ��(rbuffer[ordering[i]],ranks[ordering[i]])���� */
    for (i = 0; i < rlen; i++)
	ordering[i] = i;
    qsort(ordering, rlen, sizeof(*ordering), comp2);

    /* ����������rank��С��Ϊ���� */
    for (i = 0; i < rlen; )  {
	int a, b;
	j = ordering[i];
	a = rbuffer[j];
	b = ranks[j];
	while (++i < rlen && rbuffer[ordering[i]] == a)
	    ranks[ordering[i]] = b;
    }

    /* ��������ͻظ����� */
    MPI_Alltoallv(ranks,   rcnts, rdsps, MPI_INT,
		  sbuffer, scnts, sdsps, MPI_INT, MPI_COMM_WORLD);

    /* ������յ��Ľ�� */
    for (i = 0; i < n; i++) {
	p = map(obj[i]);
	owner[i] = sbuffer[sdsps[p]++];
    }

    free(sbuffer);
    free(rbuffer);
    free(ranks);
    free(ordering);
}

    print_buffer("The owner[] array:", n, owner, myrank, nprocs);

    /* ����N���������ȼ����Լ�ӵ�е����ݸ��� */
    N = 0;
    for (i = 0; i < n; i++)
	if (owner[i] == myrank)
	    N++;
    /* ��ȫ����� */
    i = N;
    MPI_Reduce(&i, &N, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (myrank == 0)
	printf("���� %d ����ͬ�Ķ���\n", N);
    usleep(125000);

    free(obj);
    free(owner);

    return MPI_Finalize() == MPI_SUCCESS ? 0 : 1;
}
