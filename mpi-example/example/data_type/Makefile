CC	= mpicc
CFLAG   = -g

contiguous: contiguous.c
	$(CC) $(CFLAG) -o $@ $<

vector: vector.c
	$(CC) $(CFLAG) -o $@ $<

hvector: hvector.c
	$(CC) $(CFLAG) -o $@ $<

indexed: indexed.c
	$(CC) $(CFLAG) -o $@ $<

hindexed: hindexed.c
	$(CC) $(CFLAG) -o $@ $<

struct: struct.c
	$(CC) $(CFLAG) -o $@ $<

struct_1: struct_1.c
	$(CC) $(CFLAG) -o $@ $<

struct_2: struct_2.c
	$(CC) $(CFLAG) -o $@ $<

pack: pack.c
	$(CC) $(CFLAG) -o $@ $<

mem_align: mem_align.c
	$(CC) $(CFLAG) -o $@ $<

lb_ub_ext: lb_ub_ext.c
	$(CC) $(CFLAG) -o $@ $<

address: address.c
	$(CC) $(CFLAG) -o $@ $<

matrix_trans: matrix_transpose.c
	$(CC) $(CFLAG) -o $@ $<

matrix_multiplication_0: matrix_multiplication_0.c
	$(CC) $(CFLAG) -o $@ $<

matrix_multiplication_1: matrix_multiplication_1.c
	$(CC) $(CFLAG) -o $@ $<

all: contiguous vector hvector indexed \
     hindexed struct pack mem_align \
     lb_ub_ext address matrix*

clean:
	rm -f *~ *.o a.out *.dat

cleanall:
	rm -f *~ *.o a.out *.dat
	rm -f contiguous vector hvector \
              indexed hindexed struct pack \
              mem_align lb_ub_ext address \
              struct_1 struct_2 \
              matrix_trans matrix_multiplication_0 \
              matrix_multiplication_1

