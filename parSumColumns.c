#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>

#define MAX_RAND_RANGE 10
#define OMP_NUM_THREADS 8

int main(int argc, char **argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    omp_set_num_threads(OMP_NUM_THREADS);

    if (argc != 2) {
        if (rank == 0) {
            printf("Usage: %s <matrix_size>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int n = atoi(argv[1]);

    // Seed the random number generator
    srand((unsigned int)time(NULL) + rank);

    // Process 0 generates the matrix
    int *matrix = NULL;
    if (rank == 0) {
        printf("\n--------------------------------\n");
        printf("EXECUTING PARALLEL COLUMN SUM\n");
        printf("-----------------------------------\n");
        matrix = (int *)malloc(n * n * sizeof(int));
        for (int i = 0; i < n * n; i++) {
            matrix[i] = rand() % MAX_RAND_RANGE;
        }
    }

    // Scatter the matrix data to all processes
    int local_rows = n / size;
    int *local_matrix = (int *)malloc(local_rows * n * sizeof(int));
    MPI_Scatter(matrix, local_rows * n, MPI_INT, local_matrix, local_rows * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Start the timer
    double start_time = omp_get_wtime();

    // Sum each column locally using OpenMP
    unsigned long long int *local_sums = (unsigned long long int *)calloc(n, sizeof(unsigned long long int));

    #pragma omp parallel for collapse(2)
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < local_rows; i++) {
            local_sums[j] += local_matrix[i * n + j];
        }
    }

    // Reduce the local sums across all processes
    unsigned long long int *global_sums = (unsigned long long int *)malloc(n * sizeof(unsigned long long int));
    MPI_Reduce(local_sums, global_sums, n, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // Stop the timer
    double end_time = omp_get_wtime();

    // Print the result from the root process
    if (rank == 0) {
        printf("\nColumn Sums:\n");
        for (int j = 0; j < n; j++) {
            printf("Column %d: %llu\n", j, global_sums[j]);
        }

        // Print elapsed time
        printf("Parallel time: %lfs\n", end_time - start_time);

        printf("\n--------------------------------\n");
        printf("END OF PARALLEL COLUMN SUM\n");
        printf("-----------------------------------\n");

        // Clean up
        free(matrix);
    }

    // Clean up
    free(local_matrix);
    free(local_sums);
    free(global_sums);

    MPI_Finalize();

    return 0;
}
