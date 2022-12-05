/* 编号问题2: 确定各对象的属主 */

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
/* 比较整数，用于 qsort() */
{
    return *((const int *)p1) - *((const int *)p2);
}

static int *rbuffer, *ranks;

static int
comp2(const void *p1, const void *p2)
/* 比较 (rbuffer[*p1], ranks[*p1]) 和 (rbuffer[*p2], ranks[*p1]) */
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

    /* 随机生成 obj[] 中的数据， 在 0 到 n + n 之间 */
    srand(getpid());
    for (i = 0; i < n; i++)
	obj[i] = rand() % (n + n);
    /* 对 obj[] 排序 */
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
    
    /* 计算 owner[]：owner[i] 等于 obj[i] 的属主进程号。
     * 
     * 可选的算法：
     *	1. 将所有进程中的obj[]同时收集到进程0，问题：进程0上内存可能不够
     *	2. 循环算法，问题：需要 nprocs 完成
     *	3. 将 obj[i] 发送到进程 map(obj[i]) */

{
    /* 算法3的实现 */
    int slen, scnts[nprocs], sdsps[nprocs];
    int rlen, rcnts[nprocs], rdsps[nprocs];
    int j, p, *sbuffer, *ordering;

    /* 计算需发送到各进程的数据个数：cnts[p] 包含需发送到进程p的数据个数 */
    for (p = 0; p < nprocs; p++)
	scnts[p] = 0;			/* 初始化 cnts[] */
    for (i = 0; i < n; i++) {
	p = map(obj[i]);
	scnts[p]++;
    }

    /* 各进程交换 scnts[] */
    MPI_Alltoall(scnts, 1, MPI_INT, rcnts, 1, MPI_INT, MPI_COMM_WORLD);

    /* 计算 sdsps[] 和 rdsps[] */
    slen = rlen = 0;
    for (p = 0; p < nprocs; p++) {
	sdsps[p] = slen;
	rdsps[p] = rlen;
	slen += scnts[p];
	rlen += rcnts[p];
    }
    assert(slen == n);

    /* 分配发送和接收缓冲区 */
    sbuffer = malloc(slen * sizeof(*sbuffer));
    rbuffer = malloc(rlen * sizeof(*rbuffer));
    ranks = malloc(rlen * sizeof(*ranks));
    ordering = malloc(rlen * sizeof(*ordering));

    /* 准备发送缓冲区 */
    for (i = 0; i < n; i++) {
	p = map(obj[i]);
	sbuffer[sdsps[p]++] = obj[i];
    }

    /* 恢复 sdsps[] 数组 */
    for (p = 0; p < nprocs; p++)
	sdsps[p] -= scnts[p];

    /* 发送数据到目的进程 */
    MPI_Alltoallv(sbuffer, scnts, sdsps, MPI_INT,
		  rbuffer, rcnts, rdsps, MPI_INT, MPI_COMM_WORLD);

    /* 初始化 ranks[i] 为 rbuffer[i] 的源进程 */
    for (p = 0; p < nprocs; p++)
	for (i = rdsps[p]; i < rdsps[p] + rcnts[p]; i++)
	    ranks[i] = p;

    /* 排序：计算ordering[] 使得(rbuffer[ordering[i]],ranks[ordering[i]])递增 */
    for (i = 0; i < rlen; i++)
	ordering[i] = i;
    qsort(ordering, rlen, sizeof(*ordering), comp2);

    /* 计算属主：rank最小者为属主 */
    for (i = 0; i < rlen; )  {
	int a, b;
	j = ordering[i];
	a = rbuffer[j];
	b = ranks[j];
	while (++i < rlen && rbuffer[ordering[i]] == a)
	    ranks[ordering[i]] = b;
    }

    /* 将结果发送回各进程 */
    MPI_Alltoallv(ranks,   rcnts, rdsps, MPI_INT,
		  sbuffer, scnts, sdsps, MPI_INT, MPI_COMM_WORLD);

    /* 整理接收到的结果 */
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

    /* 计算N：各进程先计算自己拥有的数据个数 */
    N = 0;
    for (i = 0; i < n; i++)
	if (owner[i] == myrank)
	    N++;
    /* 再全局求和 */
    i = N;
    MPI_Reduce(&i, &N, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (myrank == 0)
	printf("共有 %d 个不同的对象\n", N);
    usleep(125000);

    free(obj);
    free(owner);

    return MPI_Finalize() == MPI_SUCCESS ? 0 : 1;
}
