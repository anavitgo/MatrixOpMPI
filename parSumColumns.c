#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>

#define MAX_RAND_RANGE 1000
#define OMP_NUM_THREADS 8

// Function to generate a random matrix of integers
void generate_random_matrix(int n, int *matrix) {
    for (int i = 0; i < n * n; i++) {
        matrix[i] = rand() % MAX_RAND_RANGE; 
    }
}

// Function to sum the elements of a column
int sum_column(int n, int *matrix, int col) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += matrix[i * n + col];
    }
    return sum;
}

int main(int argc, char *argv[]) {

    double par_start_time, par_end_time;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <matrix_size>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    if (n <= 0) {
        fprintf(stderr, "Invalid matrix size\n");
        return 1;
    }
    
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Set the number of threads for parallelization
    omp_set_num_threads(OMP_NUM_THREADS); 

    int *matrix = NULL;

    if (rank == 0) {
        printf("\n--------------------------------\n");
        printf("EXECUTING PARALLEL COLUMN SUM\n");
        printf("--------------------------------\n");
        par_start_time = omp_get_wtime();
        // Processor with rank 0 creates the matrix
        matrix = (int *)malloc(n * n * sizeof(int));
        srand(time(NULL));
        generate_random_matrix(n, matrix);
    }

    // Broadcast matrix size to all processors
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Determine the number of columns each processor will handle
    int cols_per_proc = n / size;

    // Allocate memory for the local columns on each processor
    int *local_cols = (int *)malloc(cols_per_proc * n * sizeof(int));
    
    // Distribute columns among processors
    MPI_Scatter(matrix, cols_per_proc * n, MPI_INT, local_cols, cols_per_proc * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform column sums in parallel
    #pragma omp parallel for
    for (int j = 0; j < cols_per_proc; j++) {
        int col_sum = sum_column(n, local_cols, j);

        char processor_name[MPI_MAX_PROCESSOR_NAME];
        int name_len;
        MPI_Get_processor_name(processor_name, &name_len);

        // Print the sum of each column
        printf("Processor %d is running on host %s - Column %d Sum: %d\n", rank, processor_name, j + rank * cols_per_proc, col_sum);
    }

    free(matrix);
    free(local_cols);

    if(rank == 0){
        par_end_time = omp_get_wtime();
        printf("Parallel time: %lfs\n", par_end_time - par_start_time);
    }

    MPI_Finalize();

    if(rank == 0){
        printf("\n--------------------------------\n");
        printf("END OF PARALLEL COLUMN SUM\n");
        printf("--------------------------------\n");
    }

    return 0;
}