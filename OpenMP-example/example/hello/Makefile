# Makefile for building and running Hello World examples.
# Requires Intel Compiler.

all: 	hello_world 

hello_world:	
	cc -fopenmp -o hello_omp Hello_World_omp.c
	cc -lpthread -o hello_pthread Hello_World_pthread.c

clean:
	rm -f hello_omp hello_pthread

