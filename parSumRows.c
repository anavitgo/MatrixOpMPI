#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>  // Include this header for unsigned long long int
#include <mpi.h>
#include <omp.h>

#define MAX_RAND_RANGE 10
#define OMP_NUM_THREADS 8

// Function to generate a random matrix of integers
void generate_random_matrix(int n, int *matrix) {
    for (int i = 0; i < n * n; i++) {
        matrix[i] = rand() % MAX_RAND_RANGE;
    }
}

// Function to sum the elements of a row
unsigned long long int sum_row(int n, int *row) {
    unsigned long long int sum = 0;  // Change data type to unsigned long long int
    for (int i = 0; i < n; i++) {
        sum += row[i];
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

    // Processor with rank 0 creates the matrix
    if (rank == 0) {
        printf("\n--------------------------------\n");
        printf("EXECUTING PARALLEL ROW SUM\n");
        printf("--------------------------------\n");
        
        matrix = (int *)malloc(n * n * sizeof(int));
        srand((unsigned int)time(NULL) + rank);
        generate_random_matrix(n, matrix);
    }

    // Broadcast matrix size to all processors
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Determine the number of rows each processor will handle
    int rows_per_proc = n / size;

    // Allocate memory for the local rows on each processor
    int *local_rows = (int *)malloc(rows_per_proc * n * sizeof(int));

    // Distribute rows among processors
    MPI_Scatter(matrix, rows_per_proc * n, MPI_INT, local_rows, rows_per_proc * n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    par_start_time = omp_get_wtime();
    // Perform row sums in parallel
#pragma omp parallel for
    for (int i = 0; i < rows_per_proc; i++) {
        unsigned long long int row_sum = sum_row(n, &local_rows[i * n]);  // Change data type to unsigned long long int

        char processor_name[MPI_MAX_PROCESSOR_NAME];
        int name_len;
        MPI_Get_processor_name(processor_name, &name_len);

        // Print the sum of each row
        printf("Row %d Sum: %llu\n", i, row_sum);

    }

    free(matrix);
    free(local_rows);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    if (rank == 0) {
        par_end_time = omp_get_wtime();
        printf("Parallel time: %lfs\n", par_end_time - par_start_time);
        printf("\n--------------------------------\n");
        printf("END OF PARALLEL ROW SUM\n");
        printf("--------------------------------\n");
    }

    return 0;
}
