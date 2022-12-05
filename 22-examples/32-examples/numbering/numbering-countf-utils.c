#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static int
comp(const void *p1, const void *p2)
/* 比较整数，用于 qsort() */
{
    return *((const int *)p1) - *((const int *)p2);
}

void
qsortf_(const int buffer[], const int *n)
{
    qsort((void *)buffer, *n, sizeof(int), comp);
}

void
srand0_(void)
{
    srand(getpid());
}

int
rand0_(void)
{
    return rand();
}
