#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>

#define MAX_RAND_RANGE 10
#define OMP_NUM_THREADS 8

// Function to generate a random matrix of integers
void generate_random_matrix(int N, int *matrix) {
    for (int i = 0; i < N * N; i++) {
        matrix[i] = rand() % MAX_RAND_RANGE;
    }
}

// Function to sum all matrix elements within a range of rows
unsigned long long int sum_matrix_range(int N, int *matrix, int start_row, int end_row) {
    unsigned long long int sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < N; j++) {
            sum += matrix[i * N + j];
        }
    }
    return sum;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    omp_set_num_threads(OMP_NUM_THREADS);

    if (argc != 2) {
        if (rank == 0) {
            fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int N = atoi(argv[1]);

    // Seed the random number generator
    srand((unsigned int)time(NULL) + rank);


    // Allocate memory for the matrix
    int *matrix = NULL;
    if (rank == 0) {
        printf("\n--------------------------------\n");
        printf("EXECUTING PARALLEL MATRIX SUM\n");
        printf("-----------------------------------\n");
        matrix = (int *)malloc(N * N * sizeof(int));
        generate_random_matrix(N, matrix);
    }

    // Scatter chunks of the matrix data to all processes
    int local_rows = N / size;
    int *local_matrix = (int *)malloc(local_rows * N * sizeof(int));
    MPI_Scatter(matrix, local_rows * N, MPI_INT, local_matrix, local_rows * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    // Measure the start time
    double start_time = MPI_Wtime();
    // Sum local matrix elements within each process
    unsigned long long int local_sum = sum_matrix_range(N, local_matrix, 0, local_rows);

    // Gather local sums to calculate the total sum
    unsigned long long int total_sum;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    // Measure the end time
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;

    // Print the result from the root process
    if (rank == 0) {
        printf("Total Matrix Sum: %llu\n", total_sum);
        printf("Parallel time: %lfs\n", elapsed_time);

        printf("\n--------------------------------\n");
        printf("END OF PARALLEL MATRIX SUM\n");
        printf("-----------------------------------\n");
    }

    // Free allocated memory
    free(local_matrix);
    if (rank == 0) {
        free(matrix);
    }

    MPI_Finalize();

    return 0;
}
