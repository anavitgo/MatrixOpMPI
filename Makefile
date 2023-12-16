CC = mpicc
SOURCES = lib/utils.c
BINARY = out
HOST_FILE = hostfile
NUM_PROC ?= 4 #default value
MATRIX_DIM ?= 100 #default value

run:
	@mpirun -np $(NUM_PROC) -hostfile $(HOST_FILE) ./$(BINARY) $(MATRIX_DIM)

all:
	$(CC) -Wall main.c $(SOURCES) -o $(BINARY) -fopenmp -I lib

valgrind:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all --show-reachable=yes ./$(BINARY)

clean:
	@rm $(BINARY)
