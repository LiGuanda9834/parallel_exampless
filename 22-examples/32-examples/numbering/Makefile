.SUFFIXES: .tex .pdf .dvi

CC = mpicc -Wall -O2 -g #-DENABLE_TIMEOUT
FC = mpif90 -O2
F77 = mpif77 -O2 -g -Wall

all: numbering-count numbering-countf numbering-owner numbering-renumber

numbering-countf: numbering-countf.o numbering-countf-utils.o
	$(FC) -o $@ $^

clean:
	rm -f *.o numbering-count numbering-countf numbering-owner numbering-renumber
