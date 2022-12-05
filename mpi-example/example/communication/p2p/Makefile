 
CC	= mpicc
CFLAG	= -g

all: send_recv sendrecv sendrecv_replace \
     isendirecv probe iprobe error status \
     sendrecv_init tag tag_1 special_rank

send_recv: send_recv.c
	$(CC) $(CFLAG) -o $@ $<

sendrecv: sendrecv.c
	$(CC) $(CFLAG) -o $@ $<

sendrecv_replace: sendrecv_replace.c
	$(CC) $(CFLAG) -o $@ $<

isendirecv: isendirecv.c
	$(CC) $(CFLAG) -o $@ $<

probe: probe.c
	$(CC) $(CFLAG) -o $@ $<

iprobe: iprobe.c
	$(CC) $(CFLAG) -o $@ $<

error: error_deliver.c
	$(CC) $(CFLAG) -o $@ $<

status: status.c
	$(CC) $(CFLAG) -o $@ $<

sendrecv_init: sendrecv_init.c
	$(CC) $(CFLAG) -o $@ $<

tag: tag.c
	$(CC) $(CFLAG) -o $@ $<

tag_1: tag_1.c
	$(CC) $(CFLAG) -o $@ $<

special_rank: special_rank.c
	$(CC) $(CFLAG) -o $@ $<

clean:
	rm -f *~ *.o a.out 

cleanall:
	rm -f *~ *.o a.out
	rm -f sendrecv isendirecv probe iprobe \
              send_recv error status sendrecv_init \
              tag tag_1 special_rank sendrecv_replace
