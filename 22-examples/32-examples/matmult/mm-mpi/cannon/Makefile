CC = mpicc
OBJECTS = cannon.o init_mat.o view.o

cannon: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

run6: 
	mpirun -np 6 cannon
	
run12: 
	mpirun -np 12 cannon
clean:
	rm -f *.o info/* cannon

cleanbak:
	rm -f *.o info/* *.c~ *.h~
