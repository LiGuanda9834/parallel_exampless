 
CC	= mpicc
CFLAG	= -g

all: group_comm group_reduce

bcast: bcast.c
	$(CC) $(CFLAG) -o $@ $<

barrier: barrier.c
	$(CC) $(CFLAG) -o $@ $<

scatter: scatter.c
	$(CC) $(CFLAG) -o $@ $<

scatterv: scatterv.c
	$(CC) $(CFLAG) -o $@ $<

gather: gather.c
	$(CC) $(CFLAG) -o $@ $<

gatherv: gatherv.c
	$(CC) $(CFLAG) -o $@ $<

allgather: allgather.c
	$(CC) $(CFLAG) -o $@ $<

alltoall: alltoall.c
	$(CC) $(CFLAG) -o $@ $<

reduce: reduce.c
	$(CC) $(CFLAG) -o $@ $<

allreduce: allreduce.c
	$(CC) $(CFLAG) -o $@ $<

reduce_scatter: reduce_scatter.c
	$(CC) $(CFLAG) -o $@ $<

scan: scan.c
	$(CC) $(CFLAG) -o $@ $<

jacobi_0: jacobi_0.c
	$(CC) $(CFLAG) -o $@ $<

jacobi_1: jacobi_1.c
	$(CC) $(CFLAG) -o $@ $<

jacobi_2: jacobi_2.c
	$(CC) $(CFLAG) -o $@ $<

cpi: cpi.c
	$(CC) $(CFLAG) -o $@ $<

maxloc: maxloc.c
	$(CC) $(CFLAG) -o $@ $<

group_comm: bcast scatter scatterv gather gatherv allgather alltoall barrier

group_reduce: reduce allreduce reduce_scatter scan

clean:
	rm -f *~ *.o a.out 

cleanall:
	rm -f *~ *.o a.out
	rm -f allgather allreduce alltoall cpi maxloc\
              bcast barrier scatterv gather gatherv \
              isendrecv reduce reduce_scatter \
              scan scatter jacobi_0 jacobi_1 jacobi_2
