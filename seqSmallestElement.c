#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>  // Include for INT_MIN

#define MAX_RAND_RANGE 1000

// Function to generate a random matrix of integers
void generate_random_matrix(int N, int *matrix) {
    for (int i = 0; i < N * N; i++) {
        matrix[i] = rand() % MAX_RAND_RANGE;
    }
}

// Function to find the minimum element in the matrix
int find_min_element(int N, int *matrix) {
    int min_element = INT_MAX;  // Initialize to the maximum possible integer value

    for (int i = 0; i < N * N; i++) {
        if (matrix[i] < min_element) {
            min_element = matrix[i];
        }
    }

    return min_element;
}

int main(int argc, char *argv[]) {

    printf("\n--------------------------------\n");
    printf("EXECUTING SEQUENTIAL MIN ELEMENT\n");
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

    // Find the minimum element in the matrix
    int min_element = find_min_element(N, matrix);
    printf("Maximum Element in Matrix: %d\n", min_element);

    // Measure the end time
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Sequential time: %lfs\n", elapsed_time);

    // Free allocated memory
    free(matrix);

    printf("\n--------------------------------\n");
    printf("END OF SEQUENTIAL MIN ELEMENT\n");
    printf("-----------------------------------\n");

    return 0;
}
