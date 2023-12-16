#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef USE_SEQ
    #include <mpi.h>
#endif
#include <omp.h>
#include "utils.h"

//#define NUM_THREADS 4

int main(int argc, char *argv[]){

    int matrixDim = atoi(argv[1]);
    int **matrix = createMatrix(matrixDim);
    #ifdef USE_SEQ


        double startTimeSeq = omp_get_wtime();;
        printf("Biggest Element: %d \n", findBiggestElement(matrix, matrixDim));
        printf("Smallest Element: %d \n", findSmallestElement(matrix, matrixDim));
        printf("Sum of all elements: %d \n", sumMatrixElements(matrix, matrixDim));
        sumLinesAndPrint(matrix, matrixDim);
        sumColumnsAndPrint(matrix, matrixDim);
        double endTimeSeq = omp_get_wtime();;

        printf("-------------------------\n");
        printf("Sequential time: %lfs\n", endTimeSeq - startTimeSeq);


    #endif

    MPI_Init(&argc, &argv);
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