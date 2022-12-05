/* 编号问题3: 确定对象的属主并对对象重新编号 */

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

static struct {
    int value;		/* 对象编号 */
    int owner;		/* 属主 */
} *rbuffer2, *sbuffer2;

static int
comp2(const void *p1, const void *p2)
/* 比较 (rbuffer[*p1], ranks[*p1]) 和 (rbuffer[*p2], ranks[*p1]) */
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
    
    /* 计算 owner[]：owner[i] 等于 obj[i] 的属主进程号，同时对 obj[] 中的
     * 内容重新编号 (0,...,N-1)
     * 
     * 可选的算法：
     *	1. 将所有进程中的obj[]同时收集到进程0，问题：进程0上内存可能不够
     *	2. 循环算法，问题：需要 nprocs 完成
     *	3. 将 obj[i] 发送到进程 map(obj[i]) */

{
    /* 算法3的实现 */
    int slen, scnts[nprocs], sdsps[nprocs];
    int rlen, rcnts[nprocs], rdsps[nprocs];
    int j, p, m, *sbuffer, *rbuffer, *ordering;

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
    sbuffer2 = malloc(slen * sizeof(*sbuffer2));
    rbuffer2 = malloc(rlen * sizeof(*rbuffer2));
    ordering = malloc(rlen * sizeof(*ordering));
    /* to save memory, overlap sbuffer and rbuffer with other buffers */
    sbuffer = (void *)sbuffer2;		/* overlap sbuffer with sbuffer2 */
    rbuffer = ordering;			/* overlap rbuffer with ordering */

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

    /* 初始化 rbuffer2[] */
    for (p = 0; p < nprocs; p++) {
	for (i = rdsps[p]; i < rdsps[p] + rcnts[p]; i++) {
	    rbuffer2[i].value = rbuffer[i];
	    rbuffer2[i].owner = p;		/* 初始属主为源进程 */
	    ordering[i] = i;			/* 用于排序 */
	}
    }

    /* 排序使得(rbuffer[ordering[i]],ranks[ordering[i]])递增 */
    qsort(ordering, rlen, sizeof(*ordering), comp2);

    /* 计算属主：rank最小者为属主， 并对rbuffer[] 重新编号 */
    for (i = m = 0; i < rlen; m++)  {
	int a, b;
	j = ordering[i];
	a = rbuffer2[j].value;
	b = rbuffer2[j].owner;
	rbuffer2[j].value = m;		/* 新局部编号 */
	while (++i < rlen && rbuffer2[ordering[i]].value == a) {
	    rbuffer2[ordering[i]].value = m;	/* 新局部编号 */
	    rbuffer2[ordering[i]].owner = b;	/* 属主进程号 */
	}
    }
    free(ordering);

    /* 收集所有进程中的m */
    MPI_Scan(&m, &i, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    N = i;
    MPI_Bcast(&N, 1, MPI_INT, nprocs - 1, MPI_COMM_WORLD);
    m = i - m;	/* m 为起始编号 */

    /* 调整 rbuffer[] 中的新局部编号为全局编号 */
    for (i = 0; i < rlen; i++)
	rbuffer2[i].value += m;

    /* 将 rbuffer2[] 发送回各进程，存在 sbuffer2[] 中 */
    MPI_Alltoallv(rbuffer2, rcnts, rdsps, MPI_2INT,
		  sbuffer2, scnts, sdsps, MPI_2INT, MPI_COMM_WORLD);

    /* 整理接收到的结果 */
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
	printf("共有 %d 个不同的对象\n", N);

    free(obj);
    free(owner);

    return MPI_Finalize() == MPI_SUCCESS ? 0 : 1;
}
