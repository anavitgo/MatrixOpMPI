#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <mpi.h>
#include "utils.h"

//#define NUM_THREADS 4

int main(int argc, char *argv[]){

    MPI_Init(&argc, &argv);
    int matrixDim = atoi(argv[1]);

    int **matrix = createMatrix(matrixDim);

    // Parte sequencial
    double startTimeSeq = MPI_Wtime();
    // Esse print é mais pra debug, pode tirar depois
    //printMatrix(matrix, matrixDim);
    printf("Biggest Element: %d \n", findBiggestElement(matrix, matrixDim));
    printf("Smallest Element: %d \n", findSmallestElement(matrix, matrixDim));
    printf("Sum of all elements: %d \n", sumMatrixElements(matrix, matrixDim));
    sumLinesAndPrint(matrix, matrixDim);
    sumColumnsAndPrint(matrix, matrixDim);
    double endTimeSeq = MPI_Wtime();

    printf("-------------------------\n");
    printf("Sequential time: %fs\n", endTimeSeq - startTimeSeq);


    printf("\n \n");
    printf("-------------------------\n");

    // Parte paralela
    

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double startTimeParallel = MPI_Wtime();
    printf("Biggest Element: %d \n", mpiFindBiggestElement(matrix, matrixDim, rank, size));
    printf("Smallest Element: %d \n", mpiFindSmallestElement(matrix, matrixDim, rank, size));
    printf("Sum of all elements: %d \n", mpiSumMatrixElements(matrix, matrixDim, rank, size));
    mpiSumLinesAndPrint(matrix, matrixDim, rank, size);
    mpiSumColumnsAndPrint(matrix, matrixDim, rank, size);
    double endTimeParallel = MPI_Wtime();

    printf("-------------------------\n");
    printf("Parallel time: %fs\n", endTimeParallel - startTimeParallel);

    MPI_Finalize();
    freeMatrix(matrix, matrixDim);

    return 0;
}