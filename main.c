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
    printf("Sum of the matrix's elements: %d \n", sumMatrixElements(matrix, sizeOfMatrix, sizeOfMatrix));
    sumLinesAndPrint(matrix, sizeOfMatrix, sizeOfMatrix);
    sumColumnsAndPrint(matrix, sizeOfMatrix, sizeOfMatrix);
    double endTimeSeq = MPI_Wtime();

    printf("-------------------------\n");
    printf("Tempo da parte sequencial: %f segundos\n", endTimeSeq - startTimeSeq);
    printf("\n \n");
    printf("Parte paralela \n");
    printf("-------------------------\n");

    // Parte paralela
    

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double startTimeParallel = MPI_Wtime();
    printf("Biggest Element: %d \n", mpiFindBiggestElement(matrix, sizeOfMatrix, sizeOfMatrix, rank, size));
    printf("Smallest Element: %d \n", mpiFindSmallestElement(matrix, sizeOfMatrix, sizeOfMatrix, rank, size));
    printf("Sum of the matrix's elements: %d \n", mpiSumMatrixElements(matrix, sizeOfMatrix, sizeOfMatrix, rank, size));
    mpiSumLinesAndPrint(matrix, sizeOfMatrix, sizeOfMatrix, rank, size);
    mpiSumColumnsAndPrint(matrix, sizeOfMatrix, sizeOfMatrix, rank, size);
    double endTimeParallel = MPI_Wtime();

    printf("-------------------------\n");
    printf("Tempo da parte paralela: %f segundos\n", endTimeParallel - startTimeParallel);

    MPI_Finalize();
    freeMatrix(matrix, sizeOfMatrix);

    return 0;
}