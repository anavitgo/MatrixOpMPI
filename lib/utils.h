#ifndef UTILS_H
#define UTILS_H

int **createMatrix(int lines, int columns);
void freeMatrix(int **matrix, int lines);
void printMatrix(int **matrix, int lines, int columns);
int findBiggestElement(int **matrix, int lines, int columns);
int findSmallestElement(int **matrix, int lines, int columns);
int sumMatrixElements(int **matrix, int lines, int columns);
void sumLinesAndPrint(int **matrix, int lines, int columns);
void sumColumnsAndPrint(int **matrix, int lines, int columns);
void mpiSumLinesAndPrint(int **matrix, int lines, int columns, int rank, int size);
void mpiSumColumnsAndPrint(int **matrix, int lines, int columns, int rank, int size);
int mpiSumMatrixElements(int **matrix, int lines, int columns, int rank, int size);
int mpiFindSmallestElement(int **matrix, int lines, int columns, int rank, int size);
int mpiFindBiggestElement(int **matrix, int lines, int columns, int rank, int size);
#endif