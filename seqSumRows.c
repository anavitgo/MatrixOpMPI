#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>  // Include this header for unsigned long long int
#define MAX_RAND_RANGE 10

// Function to generate a random matrix of integers
void generate_random_matrix(int N, int *matrix) {
    for (int i = 0; i < N * N; i++) {
        matrix[i] = rand() % MAX_RAND_RANGE;
    }
}

// Function to sum the elements of a row
unsigned long long int sum_row(int N, int *row) {
    unsigned long long int sum = 0;  // Change data type to unsigned long long int
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
        unsigned long long int row_sum = sum_row(N, &matrix[i * N]);  // Change data type to unsigned long long int
        printf("Row %d Sum: %llu\n", i, row_sum);  // Change format specifier to %llu
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
