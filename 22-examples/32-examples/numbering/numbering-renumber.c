/* �������3: ȷ��������������Զ������±�� */

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

static struct {
    int value;		/* ������ */
    int owner;		/* ���� */
} *rbuffer2, *sbuffer2;

static int
comp2(const void *p1, const void *p2)
/* �Ƚ� (rbuffer[*p1], ranks[*p1]) �� (rbuffer[*p2], ranks[*p1]) */
{
    int i  = rbuffer2[*((const int *)p1)].value -
	     rbuffer2[*((const int *)p2)].value;
    if (i != 0)
	return i;
    return rbuffer2[*((const int *)p1)].owner -
	   rbuffer2[*((const int *)p2)].owner;
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
    
    /* ���� owner[]��owner[i] ���� obj[i] ���������̺ţ�ͬʱ�� obj[] �е�
     * �������±�� (0,...,N-1)
     * 
     * ��ѡ���㷨��
     *	1. �����н����е�obj[]ͬʱ�ռ�������0�����⣺����0���ڴ���ܲ���
     *	2. ѭ���㷨�����⣺��Ҫ nprocs ���
     *	3. �� obj[i] ���͵����� map(obj[i]) */

{
    /* �㷨3��ʵ�� */
    int slen, scnts[nprocs], sdsps[nprocs];
    int rlen, rcnts[nprocs], rdsps[nprocs];
    int j, p, m, *sbuffer, *rbuffer, *ordering;

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
    sbuffer2 = malloc(slen * sizeof(*sbuffer2));
    rbuffer2 = malloc(rlen * sizeof(*rbuffer2));
    ordering = malloc(rlen * sizeof(*ordering));
    /* to save memory, overlap sbuffer and rbuffer with other buffers */
    sbuffer = (void *)sbuffer2;		/* overlap sbuffer with sbuffer2 */
    rbuffer = ordering;			/* overlap rbuffer with ordering */

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

    /* ��ʼ�� rbuffer2[] */
    for (p = 0; p < nprocs; p++) {
	for (i = rdsps[p]; i < rdsps[p] + rcnts[p]; i++) {
	    rbuffer2[i].value = rbuffer[i];
	    rbuffer2[i].owner = p;		/* ��ʼ����ΪԴ���� */
	    ordering[i] = i;			/* �������� */
	}
    }

    /* ����ʹ��(rbuffer[ordering[i]],ranks[ordering[i]])���� */
    qsort(ordering, rlen, sizeof(*ordering), comp2);

    /* ����������rank��С��Ϊ������ ����rbuffer[] ���±�� */
    for (i = m = 0; i < rlen; m++)  {
	int a, b;
	j = ordering[i];
	a = rbuffer2[j].value;
	b = rbuffer2[j].owner;
	rbuffer2[j].value = m;		/* �¾ֲ���� */
	while (++i < rlen && rbuffer2[ordering[i]].value == a) {
	    rbuffer2[ordering[i]].value = m;	/* �¾ֲ���� */
	    rbuffer2[ordering[i]].owner = b;	/* �������̺� */
	}
    }
    free(ordering);

    /* �ռ����н����е�m */
    MPI_Scan(&m, &i, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    N = i;
    MPI_Bcast(&N, 1, MPI_INT, nprocs - 1, MPI_COMM_WORLD);
    m = i - m;	/* m Ϊ��ʼ��� */

    /* ���� rbuffer[] �е��¾ֲ����Ϊȫ�ֱ�� */
    for (i = 0; i < rlen; i++)
	rbuffer2[i].value += m;

    /* �� rbuffer2[] ���ͻظ����̣����� sbuffer2[] �� */
    MPI_Alltoallv(rbuffer2, rcnts, rdsps, MPI_2INT,
		  sbuffer2, scnts, sdsps, MPI_2INT, MPI_COMM_WORLD);

    /* ������յ��Ľ�� */
    for (i = 0; i < n; i++) {
	p = map(obj[i]);
	j = sdsps[p]++;
	obj[i] = sbuffer2[j].value;
	owner[i] = sbuffer2[j].owner;
    }

    free(sbuffer2);
    free(rbuffer2);
}

    print_buffer("The owner[] array:", n, owner, myrank, nprocs);
    print_buffer("The new obj[] array:", n, obj, myrank, nprocs);

    usleep(125000);
    if (myrank == 0)
	printf("���� %d ����ͬ�Ķ���\n", N);

    free(obj);
    free(owner);

    return MPI_Finalize() == MPI_SUCCESS ? 0 : 1;
}
