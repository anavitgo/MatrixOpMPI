#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <mpi.h>
#include <time.h>

#include "utils.h"


int **createMatrix(int lines, int columns){
  int **matrix = malloc(lines * sizeof(int *));

  for (int i = 0; i < lines; i++){
    matrix[i] = malloc(columns * sizeof(int));
    for (int j = 0; j < columns; j++){ 
      matrix[i][j] = rand() % 10;
    }
  }

  return matrix;
}

void freeMatrix(int **matrix, int lines){
    for (int i = 0; i < lines; i++){
        free(matrix[i]);
    }
    free(matrix);
}

void printMatrix(int **matrix, int lines, int columns){
    for (int i = 0; i < lines; i++){
        for (int j = 0; j < columns; j++){
            printf("%d ", matrix[i][j]);
            if (j == columns - 1){ 
                printf("\n");
            }
        }
    }
}

int findBiggestElement(int **matrix, int lines, int columns){
  int biggest = matrix[0][0];

  for (int i = 0; i < lines; i++){
    for (int j = 0; j < columns; j++){ 
      if (matrix[i][j] > biggest){
        biggest = matrix[i][j];
      }
    }
  }
  
  return biggest;
}

int findSmallestElement(int **matrix, int lines, int columns){
  int smallest = matrix[0][0];

  for (int i = 0; i < lines; i++){
    for (int j = 0; j < columns; j++){ 
      if (matrix[i][j] < smallest){
        smallest = matrix[i][j];
      }
    }
  }
  
  return smallest;
}

int sumMatrixElements(int **matrix, int lines, int columns){
  int sum = matrix[0][0];

  for (int i = 0; i < lines; i++){
    for (int j = 0; j < columns; j++){ 
        sum += matrix[i][j];
    }
  }
  
  return sum;
}

void sumLinesAndPrint(int **matrix, int lines, int columns){
    int *result = malloc(lines * sizeof(int));

    for (int i = 0; i < lines; i++){
      result[i] = 0;

      for (int j = 0; j < columns; j++){ 
        result[i] += matrix[i][j];
      }
      printf("Line %d, total sum: %d\n", i, result[i]);
    }

    free(result);
}

void sumColumnsAndPrint(int **matrix, int lines, int columns) {
    int *result = malloc(columns * sizeof(int));

    for (int i = 0; i < columns; i++) {
        result[i] = 0;

        for (int j = 0; j < lines; j++) {
            result[i] += matrix[j][i];
        }

        printf("Column %d, total sum: %d\n", i, result[i]);
    }
  
    free(result);
}


// Função para calcular a soma das linhas e imprimir o resultado
void mpiSumLinesAndPrint(int **matrix, int lines, int columns, int rank, int size) {
    int *result = malloc(lines * sizeof(int));

    for (int i = 0; i < lines; i++) {
        result[i] = 0;

        for (int j = 0; j < columns; j++) {
            result[i] += matrix[i][j];
        }
    }

    // Reduz a soma de todas as linhas usando MPI_Reduce
    int *globalResult = malloc(lines * sizeof(int));
    MPI_Reduce(result, globalResult, lines, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Imprime o resultado no processo 0
    if (rank == 0) {
        for (int i = 0; i < lines; i++) {
            printf("Line %d, total sum: %d\n", i, globalResult[i]);
        }
    }

    free(result);
    free(globalResult);
}

// Função para calcular a soma das colunas e imprimir o resultado
void mpiSumColumnsAndPrint(int **matrix, int lines, int columns, int rank, int size) {
    int *result = malloc(columns * sizeof(int));

    for (int i = 0; i < columns; i++) {
        result[i] = 0;

        for (int j = 0; j < lines; j++) {
            result[i] += matrix[j][i];
        }
    }

    // Reduz a soma de todas as colunas usando MPI_Reduce
    int *globalResult = malloc(columns * sizeof(int));
    MPI_Reduce(result, globalResult, columns, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Imprime o resultado no processo 0
    if (rank == 0) {
        for (int i = 0; i < columns; i++) {
            printf("Column %d, total sum: %d\n", i, globalResult[i]);
        }
    }

    free(result);
    free(globalResult);
}

int mpiSumMatrixElements(int **matrix, int lines, int columns, int rank, int size) {
    int sum = matrix[0][0];

    MPI_Win win;
    MPI_Win_create(&sum, sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

    // Distribui as linhas da matriz entre os processos
    int linesPerProcess = lines / size;
    int remainder = lines % size;
    int localLines = (rank < remainder) ? linesPerProcess + 1 : linesPerProcess;
    int localStart = (rank < remainder) ? rank * (linesPerProcess + 1) : (rank * linesPerProcess) + remainder;

    // Calcula a soma local das linhas atribuídas ao processo
    for (int i = 0; i < localLines; i++) {
        for (int j = 0; j < columns; j++) {
            MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, win);
            sum += matrix[localStart + i][j];
            MPI_Win_unlock(rank, win);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Reduz a soma local para obter a soma total
    MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, win);
    MPI_Reduce(MPI_IN_PLACE, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Win_unlock(rank, win);

    MPI_Win_free(&win);

    return sum;
}

int mpiFindSmallestElement(int **matrix, int lines, int columns, int rank, int size) {
    // Inicializa smallest com o primeiro elemento da matriz
    int smallest = matrix[0][0];
    int localSmallest = matrix[0][0];

    // Distribui as linhas da matriz entre os processos
    int linesPerProcess = lines / size;
    int remainder = lines % size;
    int localLines = (rank < remainder) ? linesPerProcess + 1 : linesPerProcess;
    int localStart = (rank < remainder) ? rank * (linesPerProcess + 1) : (rank * linesPerProcess) + remainder;

    // Encontra o menor elemento local nas linhas atribuídas ao processo
    for (int i = 0; i < localLines; i++) {
        for (int j = 0; j < columns; j++) {
            if (matrix[localStart + i][j] < localSmallest) {
                localSmallest = matrix[localStart + i][j];
            }
        }
    }

    // Reduz o menor elemento local para obter o menor elemento global
    MPI_Reduce(&localSmallest, &smallest, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    return smallest;
}

int mpiFindBiggestElement(int **matrix, int lines, int columns, int rank, int size) {
    int biggest = matrix[0][0];
    int localBiggest = matrix[0][0];

    // Distribui as linhas da matriz entre os processos
    int linesPerProcess = lines / size;
    int remainder = lines % size;
    int localLines = (rank < remainder) ? linesPerProcess + 1 : linesPerProcess;
    int localStart = (rank < remainder) ? rank * (linesPerProcess + 1) : (rank * linesPerProcess) + remainder;

    // Encontra o maior elemento local nas linhas atribuídas ao processo
    for (int i = 0; i < localLines; i++) {
        for (int j = 0; j < columns; j++) {
            if (matrix[localStart + i][j] > localBiggest) {
                localBiggest = matrix[localStart + i][j];
            }
        }
    }

    // Reduz o maior elemento local para obter o maior elemento global
    MPI_Reduce(&localBiggest, &biggest, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    return biggest;
}