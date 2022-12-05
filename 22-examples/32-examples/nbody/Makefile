MPICC = mpicc
CC = gcc
EXE = nbody_basic omp_nbody_basic mpi_nbody_basic 
FLAGS=-DNO_OUTPUT -O0 -lm

all: $(EXE)

nbody_basic: nbody_basic.c
	$(CC) $(FLAGS) -o $@ $<

omp_nbody_basic: omp_nbody_basic.c
	$(CC) -fopenmp  $(FLAGS) -o $@ $<

mpi_nbody_basic: mpi_nbody_basic.c
	$(MPICC) $(FLAGS) -o $@ $<

clean:
	rm -f *.o 

cleanall:
	rm -f *.o info/* *.c~ *.h~  $(EXE)

run1:
	./nbody_basic  1000 100  1 100 g  
	./omp_nbody_basic  2 1000 100  1 100 g  
	./omp_nbody_basic  4 1000 100  1 100 g  
	./omp_nbody_basic  8 1000 100  1 100 g  
	mpirun -n 2 ./mpi_nbody_basic  1000 100  1 100 g  
	mpirun -n 4 ./mpi_nbody_basic  1000 100  1 100 g  
	mpirun -n 8 ./mpi_nbody_basic  1000 100  1 100 g  

run2:
	./nbody_basic  2000 100  1 100 g  
	./omp_nbody_basic  2 2000 100  1 100 g  
	./omp_nbody_basic  4 2000 100  1 100 g  
	./omp_nbody_basic  8 2000 100  1 100 g  
	mpirun -n 2 ./mpi_nbody_basic  2000 100  1 100 g  
	mpirun -n 4 ./mpi_nbody_basic  2000 100  1 100 g  
	mpirun -n 8 ./mpi_nbody_basic  2000 100  1 100 g  
