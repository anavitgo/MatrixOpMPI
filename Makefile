CC = mpicc
SOURCES = lib/utils.c
BINARY = out
HOST_FILE = hostfile
NUM_PROC ?= 4 #default value
MATRIX_DIM ?= 100 #default value

mpi_run:
	@mpirun -np $(NUM_PROC) -hostfile $(HOST_FILE) ./$(BINARY) $(MATRIX_DIM)

seq_run:
	@./$(BINARY) $(MATRIX_DIM)

seq:
	$(CC) -Wall main.c $(SOURCES) -o $(BINARY) -fopenmp -I lib -DUSE_SEQ

mpi: $(SOURCE)
	$(CC) -Wall $(SOURCE) $(SOURCES) -o $(BINARY) -fopenmp -I lib -DUSE_MPI

valgrind:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all --show-reachable=yes ./$(BINARY)

clean:
	@rm $(BINARY)
