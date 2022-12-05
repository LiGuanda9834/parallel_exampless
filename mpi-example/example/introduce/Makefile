CC	= mpicc
CFLAG	= -g


hello: hello.c
	$(CC) $(CFLAG) -o $@ $<

all: hello wtime rank sendrecv
wtime: wtime.c
	$(CC) $(CFLAG) -o $@ $<

rank: rank.c
	$(CC) $(CFLAG) -o $@ $<

sendrecv: sendrecv.c
	$(CC) $(CFLAG) -o $@ $<

clean:
	rm -f *~ *.o a.out 

cleanall:
	rm -f *~ *.o a.out
	rm -f hello wtime rank sendrecv
	rm -rf doc/
