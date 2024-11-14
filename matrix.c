#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int row;
    int col;
    int size;
    int **matA;
    int **matB;
    int **result;
} MatrixCoord;

void fillMatrix(int size, int **matrix) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int size, int **matrix) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void *computeSum(void *args) {
    MatrixCoord *coord = (MatrixCoord *)args;
    int row = coord->row;
    int col = coord->col;
    coord->result[row][col] = coord->matA[row][col] + coord->matB[row][col];
    free(coord);
    pthread_exit(NULL);
}

void *computeDiff(void *args) {
    MatrixCoord *coord = (MatrixCoord *)args;
    int row = coord->row;
    int col = coord->col;
    coord->result[row][col] = coord->matA[row][col] - coord->matB[row][col];
    free(coord);
    pthread_exit(NULL);
}

void *computeProduct(void *args) {
    MatrixCoord *coord = (MatrixCoord *)args;
    int row = coord->row;
    int col = coord->col;
    coord->result[row][col] = 0;
    for (int k = 0; k < coord->size; k++) {
        coord->result[row][col] += coord->matA[row][k] * coord->matB[k][col];
    }
    free(coord);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.

    // 0. Get the matrix size from the command line and assign it to MAX
    if (argc != 2) {
        printf("Usage: %s <matrix size>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int MAX = atoi(argv[1]);
    if (MAX <= 0) {
        printf("Matrix size must be a positive integer.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for matrices
    int **matA = malloc(MAX * sizeof(int *));
    int **matB = malloc(MAX * sizeof(int *));
    int **matSumResult = malloc(MAX * sizeof(int *));
    int **matDiffResult = malloc(MAX * sizeof(int *));
    int **matProductResult = malloc(MAX * sizeof(int *));
    for (int i = 0; i < MAX; i++) {
        matA[i] = malloc(MAX * sizeof(int));
        matB[i] = malloc(MAX * sizeof(int));
        matSumResult[i] = malloc(MAX * sizeof(int));
        matDiffResult[i] = malloc(MAX * sizeof(int));
        matProductResult[i] = malloc(MAX * sizeof(int));
    }

    // 1. Fill the matrices (matA and matB) with random values.
    fillMatrix(MAX, matA);
    fillMatrix(MAX, matB);

    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(MAX, matA);
    printf("Matrix B:\n");
    printMatrix(MAX, matB);

    // 3. Create pthread_t objects for our threads.
    pthread_t threads[3 * MAX * MAX]; // Because we have three operations

    // 4. Create a thread for each cell of each matrix operation.
    int threadIndex = 0;

    // Sum threads
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            MatrixCoord *coord = malloc(sizeof(MatrixCoord));
            coord->row = i;
            coord->col = j;
            coord->matA = matA;
            coord->matB = matB;
            coord->result = matSumResult;
            pthread_create(&threads[threadIndex++], NULL, computeSum, coord);
        }
    }

    // Difference threads
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            MatrixCoord *coord = malloc(sizeof(MatrixCoord));
            coord->row = i;
            coord->col = j;
            coord->matA = matA;
            coord->matB = matB;
            coord->result = matDiffResult;
            pthread_create(&threads[threadIndex++], NULL, computeDiff, coord);
        }
    }

    // Product threads
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            MatrixCoord *coord = malloc(sizeof(MatrixCoord));
            coord->row = i;
            coord->col = j;
            coord->size = MAX;
            coord->matA = matA;
            coord->matB = matB;
            coord->result = matProductResult;
            pthread_create(&threads[threadIndex++], NULL, computeProduct, coord);
        }
    }

    // 5. Wait for all threads to finish.
    for (int i = 0; i < threadIndex; i++) {
        pthread_join(threads[i], NULL);
    }

    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(MAX, matSumResult);
    printf("Difference:\n");
    printMatrix(MAX, matDiffResult);
    printf("Product:\n");
    printMatrix(MAX, matProductResult);

    // Free allocated memory
    for (int i = 0; i < MAX; i++) {
        free(matA[i]);
        free(matB[i]);
        free(matSumResult[i]);
        free(matDiffResult[i]);
        free(matProductResult[i]);
    }
    free(matA);
    free(matB);
    free(matSumResult);
    free(matDiffResult);
    free(matProductResult);

    return 0;
}
