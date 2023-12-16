CC = mpicc
BINARY = out
HOST_FILE = hostfile
NUM_PROC ?= 4 #default value
MATRIX_DIM ?= 100 #default value

check_source:
ifndef SEQ_SOURCE
	$(error SEQ_SOURCE is not set. Please specify the sequential C source file using 'make mpi SEQ_SOURCE=<file>.c')
endif
ifndef PAR_SOURCE
	$(error PAR_SOURCE is not set. Please specify the parallel C source file using 'make mpi PAR_SOURCE=<file>.c')
endif

run:
	@mpirun -np $(NUM_PROC) -hostfile $(HOST_FILE) ./$(BINARY) $(MATRIX_DIM)

mpi: check_source $(SEQ_SOURCE) $(PAR_SOURCE)
	gcc -Wall $(SEQ_SOURCE) -o $(BINARY_SEQ) -fopenmp -lgomp
	$(CC) -Wall $(PAR_SOURCE) -o $(BINARY_PAR) -fopenmp -lgomp

valgrind:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all --show-reachable=yes ./$(BINARY)

clean:
	@rm $(BINARY)
