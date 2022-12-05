CC	= mpicc
CFLAG	= -g
EXE=jacobi_0  jacobi_1  jacobi_2  jacobi_3


%:%.c
	$(CC) $(CFLAG) -o $@ $<

all: $(EXE)

clean:
	rm -f *~ *.o a.out 

cleanall:
	rm -f *~ *.o a.out
	rm -f $(EXE)
