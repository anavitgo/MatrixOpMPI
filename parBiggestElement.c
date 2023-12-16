#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <mpi.h>
#include <omp.h>

#define MAX_RAND_RANGE 1000
#define OMP_NUM_THREADS 8

// Function to generate a random matrix of integers
void generate_random_matrix(int N, int *matrix) {
    for (int i = 0; i < N * N; i++) {
        matrix[i] = rand() % MAX_RAND_RANGE;
    }
}

// Function to find the maximum element in the matrix within a range of rows
int find_max_element_range(int N, int *matrix, int start_row, int end_row) {
    int max_element = -1;  // Initialize to a value smaller than the possible range

    #pragma omp parallel for reduction(max:max_element)
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < N; j++) {
            if (matrix[i * N + j] > max_element) {
                max_element = matrix[i * N + j];
            }
        }
    }

    return max_element;
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

    // Measure the start time
    double start_time = MPI_Wtime();

    // Allocate memory for the matrix
    int *matrix = NULL;
    if (rank == 0) {
        printf("\n--------------------------------\n");
        printf("EXECUTING PARALLEL MATRIX MAX ELEMENT\n");
        printf("-----------------------------------\n");
        matrix = (int *)malloc(N * N * sizeof(int));
        generate_random_matrix(N, matrix);
    }

    // Scatter chunks of the matrix data to all processes
    int local_rows = N / size;
    int *local_matrix = (int *)malloc(local_rows * N * sizeof(int));
    MPI_Scatter(matrix, local_rows * N, MPI_INT, local_matrix, local_rows * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Find the local maximum element within each process
    int local_max = find_max_element_range(N, local_matrix, 0, local_rows);

    // Gather local maximum elements to find the total maximum element
    int total_max;
    MPI_Reduce(&local_max, &total_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    // Measure the end time
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;

    // Print the result from the root process
    if (rank == 0) {
        printf("Maximum Element in Matrix: %d\n", total_max);
        printf("Parallel time: %lfs\n", elapsed_time);

        printf("\n--------------------------------\n");
        printf("END OF PARALLEL MATRIX MAX ELEMENT\n");
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
