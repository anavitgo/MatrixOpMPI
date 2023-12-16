CC = mpicc
SOURCES = lib/utils.c
BINARY = out
HOST_FILE = hostfile
NUM_PROC ?= 4 #default value
MATRIX_DIM ?= 100 #default value

check_source:
ifndef SOURCE
	$(error SOURCE is not set. Please specify the C source file using 'make mpi SOURCE=<file>.c')
endif

run:
	@mpirun -np $(NUM_PROC) -hostfile $(HOST_FILE) ./$(BINARY) $(MATRIX_DIM)

mpi: check_source $(SOURCE)
	$(CC) -Wall $(SOURCE) $(SOURCES) -o $(BINARY) -fopenmp -I lib

valgrind:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all --show-reachable=yes ./$(BINARY)

clean:
	@rm $(BINARY)
