 
#CC	= /usr/bin/gcc41
CC	= gcc
#CFLAG	= -openmp -openmp-report2
CFLAG	= -fopenmp
EXE= 01-get_tid \
02-parallel_for \
03-para_serial_barrier \
04-reduction \
05-condition_for \
06-private \
07-thread_private \
08-first_last_private \
08-noncritical \
09-section \
10-schedule \
11-critical \
12-noncritical \
13-function 


% : %.c
	$(CC) $(CFLAG) -o $@ $<


all: $(EXE)
default: all

clean:
	rm -f *~ *.o a.out 

distclean:
	rm -f *.o a.out $(EXE) 

