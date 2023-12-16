#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_RAND_RANGE 1000

// Function to generate a random matrix of integers
void generate_random_matrix(int N, int *matrix) {
    for (int i = 0; i < N * N; i++) {
        matrix[i] = rand() % MAX_RAND_RANGE;
    }
}

// Function to sum the elements of a row
int sum_row(int N, int *row) {
    int sum = 0;
    for (int i = 0; i < N; i++) {
        sum += row[i];
    }
    return sum;
}

int main(int argc, char *argv[]) {

    printf("\n--------------------------------\n");
    printf("EXECUTING SEQUENTIAL ROW SUM\n");
    printf("-----------------------------------\n");

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);

    // Measure the start time
    clock_t start_time = clock();

    // Allocate memory for the matrix
    int *matrix = (int *)malloc(N * N * sizeof(int));

    // Generate a random matrix
    generate_random_matrix(N, matrix);

    // Sum all rows
    for (int i = 0; i < N; i++) {
        int row_sum = sum_row(N, &matrix[i * N]);
        printf("Row %d Sum: %d\n", i, row_sum);
    }

    // Measure the end time
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Sequential time: %lfs\n", elapsed_time);

    // Free allocated memory
    free(matrix);

    printf("\n--------------------------------\n");
    printf("END OF SEQUENTIAL ROW SUM\n");
    printf("-----------------------------------\n");

    return 0;
}
