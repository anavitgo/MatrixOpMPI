#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>

#define MAX_RAND_RANGE 1000
#define OMP_NUM_THREADS 8

int main(int argc, char **argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

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

    // Each process generates its own part of the matrix
    int local_rows = n / size;
    int local_cols = n;
    int *local_matrix = (int *)malloc(local_rows * local_cols * sizeof(int));

    for (int i = 0; i < local_rows; i++) {
        for (int j = 0; j < local_cols; j++) {
            local_matrix[i * local_cols + j] = rand() % MAX_RAND_RANGE; // Random numbers between 0 and 9
        }
    }

    // Start the timer
    double start_time = omp_get_wtime();

    // Sum each column locally using OpenMP
    int *local_sums = (int *)calloc(local_cols, sizeof(int));

    #pragma omp parallel for collapse(2)
    for (int j = 0; j < local_cols; j++) {
        for (int i = 0; i < local_rows; i++) {
            local_sums[j] += local_matrix[i * local_cols + j];
        }
    }

    // Reduce the local sums across all processes
    int *global_sums = (int *)malloc(local_cols * sizeof(int));
    MPI_Reduce(local_sums, global_sums, local_cols, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Stop the timer
    double end_time = omp_get_wtime();

    // Print the result from the root process
    if (rank == 0) {
        printf("Original Matrix:\n");
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                printf("%d ", local_matrix[i * n + j]);
            }
            printf("\n");
        }

        printf("\nColumn Sums:\n");
        for (int j = 0; j < n; j++) {
            printf("Column %d: %d\n", j, global_sums[j]);
        }

        // Print elapsed time
        printf("\nElapsed Time: %f seconds\n", end_time - start_time);
    }

    // Clean up
    free(local_matrix);
    free(local_sums);
    free(global_sums);

    MPI_Finalize();

    return 0;
}
