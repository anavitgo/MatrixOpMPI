CC = mpicc
SOURCES = lib/utils.c
BINARY = out
MATRIX_DIM ?= 100

run:
	@mpirun -np 1 ./$(BINARY) $(MATRIX_DIM)

all:
	$(CC) -Wall main.c $(SOURCES) -o $(BINARY) -fopenmp -I lib

mpi:
	$(CC) -Wall main_mpi.c $(SOURCES) -o $(BINARY)_mpi -fopenmp -I lib -DUSE_MPI

valgrind:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all --show-reachable=yes ./$(BINARY)

clean:
	@rm $(BINARY) out.bmp $(BINARY)_mpi
