#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <mpi.h>
#include "utils.h"

//#define NUM_THREADS 4

int main(int argc, char *argv[]){

    MPI_Init(&argc, &argv);
    // Só criamos matrizes quadradas
    int sizeOfMatrix = 1000;

    int **matrix = createMatrix(sizeOfMatrix, sizeOfMatrix);

    // Parte sequencial
    double startTimeSeq = MPI_Wtime();
    // Esse print é mais pra debug, pode tirar depois
    //printMatrix(matrix, sizeOfMatrix, sizeOfMatrix);
    printf("Biggest Element: %d \n", findBiggestElement(matrix, sizeOfMatrix, sizeOfMatrix));
    printf("Smallest Element: %d \n", findSmallestElement(matrix, sizeOfMatrix, sizeOfMatrix));
    printf("Sum of all elements: %d \n", sumMatrixElements(matrix, sizeOfMatrix, sizeOfMatrix));
    sumLinesAndPrint(matrix, sizeOfMatrix, sizeOfMatrix);
    sumColumnsAndPrint(matrix, sizeOfMatrix, sizeOfMatrix);
    double endTimeSeq = MPI_Wtime();

    printf("-------------------------\n");
    printf("Sequential time: %fs\n", endTimeSeq - startTimeSeq);
    printf("CPU usage: ");
    system("mpstat");

    printf("\n \n");
    printf("-------------------------\n");

    // Parte paralela
    

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double startTimeParallel = MPI_Wtime();
    printf("Biggest Element: %d \n", mpiFindBiggestElement(matrix, sizeOfMatrix, sizeOfMatrix, rank, size));
    printf("Smallest Element: %d \n", mpiFindSmallestElement(matrix, sizeOfMatrix, sizeOfMatrix, rank, size));
    printf("Sum of all elements: %d \n", mpiSumMatrixElements(matrix, sizeOfMatrix, sizeOfMatrix, rank, size));
    mpiSumLinesAndPrint(matrix, sizeOfMatrix, sizeOfMatrix, rank, size);
    mpiSumColumnsAndPrint(matrix, sizeOfMatrix, sizeOfMatrix, rank, size);
    double endTimeParallel = MPI_Wtime();

    printf("-------------------------\n");
    printf("Parallel time: %fs\n", endTimeParallel - startTimeParallel);
    printf("CPU usage: ");
    system("mpstat");

    MPI_Finalize();
    freeMatrix(matrix, sizeOfMatrix);

    return 0;
}