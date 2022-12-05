CC	= mpicc
CFLAG	= -g

topo: topo.c
	$(CC) $(CFLAG) -o $@ $<

argc: argc.c
	$(CC) $(CFLAG) -o $@ $<

jacobi: jacobi_3.c
	$(CC) $(CFLAG) -o $@ $<

no_mpi_io: no_mpi_io.c
	$(CC) $(CFLAG) -o $@ $<

no_mpi_io_1: no_mpi_io_1.c
	$(CC) $(CFLAG) -o $@ $<

io: io.c
	$(CC) $(CFLAG) -o $@ $<

io_share: io_share.c
	$(CC) $(CFLAG) -o $@ $<

all: topo argc jacobi no_mpi_io no_mpi_io_1 io io_share

clean:
	rm -f *~ *.o a.out output/* sharedfile

cleanall:
	rm -f *~ *.o a.out file_* sharedfile io_share
	rm -f topo argc jacobi no_mpi_io io no_mpi_io_1
