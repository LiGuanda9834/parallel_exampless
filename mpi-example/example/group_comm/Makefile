 
CC	= mpicc
CFLAG	= -g

comm_group: comm_group.c
	$(CC) $(CFLAG) -o $@ $<

group_incl: group_incl.c
	$(CC) $(CFLAG) -o $@ $<

group_range_incl: group_range_incl.c
	$(CC) $(CFLAG) -o $@ $<

group_union: group_union.c
	$(CC) $(CFLAG) -o $@ $<

group_diff: group_diff.c
	$(CC) $(CFLAG) -o $@ $<

comm_creat: comm_creat.c
	$(CC) $(CFLAG) -o $@ $<

split: split.c
	$(CC) $(CFLAG) -o $@ $<

all: comm_group group_incl group_union \
     comm_creat split group_range_incl \
     group_diff

clean:
	rm -f *~ *.o a.out 

cleanall:
	rm -f *~ *.o a.out
	rm -f comm_group group_incl \
              group_union comm_creat \
              split group_range_incl \
              group_diff
