#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>

// Function to generate a random matrix of integers
void generate_random_matrix(int n, int *matrix) {
    for (int i = 0; i < n * n; i++) {
        matrix[i] = rand() % 100; // Adjust the range as needed
    }
}

// Function to sum the elements of a row
int sum_row(int n, int *row) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += row[i];
    }
    return sum;
}

// Function to process a host (replace this with your actual data sending logic)
void process_host(const char *host, int row, int row_sum) {
    // Replace this with your actual data sending logic to the host
    // For demonstration purposes, it prints the host, row, and row sum
    printf("Sending data to %s for row %d sum: %d\n", host, row, row_sum);
}

int main(int argc, char *argv[]) {
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
    omp_set_num_threads(4); // Adjust the number of threads as needed

    int *matrix = NULL;

    if (rank == 0) {
        // Processor with rank 0 creates the matrix
        matrix = (int *)malloc(n * n * sizeof(int));
        srand(time(NULL));
        generate_random_matrix(n, matrix);

        // Print the original matrix
        printf("Original Matrix:\n");
        for (int i = 0; i < n; i++) {
            int sum = 0;
            for (int j = 0; j < n; j++) {
                printf("%d\t", matrix[i * n + j]);
                sum += matrix[i * n + j];
            }
            printf("Row: %d, total sum: %d\n", i, sum);
            printf("\n");
        }
    }

    // Broadcast matrix size to all processors
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Determine the number of rows each processor will handle
    int rows_per_proc = n / size;

    // Allocate memory for the local rows on each processor
    int *local_rows = (int *)malloc(rows_per_proc * n * sizeof(int));
    
    // Distribute rows among processors
    MPI_Scatter(matrix, rows_per_proc * n, MPI_INT, local_rows, rows_per_proc * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform row sums in parallel
    #pragma omp parallel for
    for (int i = 0; i < rows_per_proc; i++) {
        int row_sum = sum_row(n, &local_rows[i * n]);

        // Print the sum of each row
        printf("Processor %d - Row %d Sum: %d\n", rank, i + rank * rows_per_proc, row_sum);

        // Process the host for each row sum
        char host[256];
        sprintf(host, "Host%d", rank); // Placeholder logic, replace with actual logic
        process_host(host, i + rank * rows_per_proc, row_sum);
    }

    free(matrix);
    free(local_rows);

    MPI_Finalize();

    return 0;
}
