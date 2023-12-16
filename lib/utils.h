#ifndef UTILS_H
#define UTILS_H

int **createMatrix(int N);
void freeMatrix(int **matrix, int lines);
void printMatrix(int **matrix, int matrixDim);
int findBiggestElement(int **matrix, int matrixDim);
int findSmallestElement(int **matrix, int matrixDim);
int sumMatrixElements(int **matrix, int matrixDim);
void sumLinesAndPrint(int **matrix, int matrixDim);
void sumColumnsAndPrint(int **matrix, int matrixDim);
void mpiSumLinesAndPrint(int **matrix, int matrixDim, int rank, int size);
void mpiSumColumnsAndPrint(int **matrix, int matrixDim, int rank, int size);
int mpiSumMatrixElements(int **matrix, int matrixDim, int rank, int size);
int mpiFindSmallestElement(int **matrix, int matrixDim, int rank, int size);
int mpiFindBiggestElement(int **matrix, int matrixDim, int rank, int size);
#endif