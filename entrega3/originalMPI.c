/** 
 * @authors: Victor Bueno, Yuri Shiba
*/

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define N 2048 // Tamanho do tabuleiro NxN
#define NUMBER_OF_GENERATIONS 2000 // Número de iterações de atualização do tabuleiro

int **grid, **otherGrid;
int proccessId, numberOfProcesses;
int gridLines;

struct timeval startTime, endTime;

void initGrids()
{
    gridLines = N / numberOfProcesses + (N % numberOfProcesses > proccessId);

    grid = malloc((gridLines + 2) * sizeof(int*));
    otherGrid = malloc((gridLines + 2) * sizeof(int*));

    int i, j;

    // Inicializa os grids com zeros
    for(i = 0; i < gridLines + 2; i++) {
        grid[i] = malloc(N * sizeof(int));
        otherGrid[i] = malloc(N * sizeof(int));
        
        for(j = 0; j < N; j++) {
            grid[i][j] = 0;
            otherGrid[i][j] = 0;
        }
    }

    // Insere a forma do Glider
    int lin = 1, col = 1;
    grid[lin][col + 1] = 1;
    grid[lin + 1][col + 2] = 1;
    grid[lin + 2][col] = 1;
    grid[lin + 2][col + 1] = 1;
    grid[lin + 2][col + 2] = 1;

    // Insere a forma R-pentomino
    lin = 10;
    col = 30;
    grid[lin][col + 1] = 1;
    grid[lin][col + 2] = 1;
    grid[lin + 1][col] = 1;
    grid[lin + 1][col + 1] = 1;
    grid[lin + 2][col + 1] = 1;
}

void exchangeOfInformation(int **gridParam) {
    MPI_Request request;
    int next = (proccessId + 1) % numberOfProcesses;
    int previous = proccessId == 0 ? numberOfProcesses - 1 : proccessId - 1;

    MPI_Isend(gridParam[1], N, MPI_INT, previous, 0, MPI_COMM_WORLD, &request);
    MPI_Isend(gridParam[gridLines], N, MPI_INT, next, 0, MPI_COMM_WORLD, &request);
    MPI_Recv(gridParam[gridLines + 1], N, MPI_INT, next, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(gridParam[0], N, MPI_INT, previous, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}


int countNeighbors(int x, int y, int** gridParam) {
    int neighbors = 0, nb;

    if(y != 0) nb = y - 1;
    else nb = N - 1;

    neighbors += gridParam[x - 1][nb];
    neighbors += gridParam[x][nb];
    neighbors += gridParam[x + 1][nb];
    neighbors += gridParam[x + 1][y];
    neighbors += gridParam[x + 1][(y+ 1) % N];
    neighbors += gridParam[x][(y+ 1) % N];
    neighbors += gridParam[x - 1][(y+ 1) % N];
    neighbors += gridParam[x - 1][y];

    return neighbors;
}

void startNewGeneration(int **previousGen, int **newGen) {
    int i, j, neighbours;

    if(numberOfProcesses > 1) exchangeOfInformation(previousGen);
    else {
        for(i = 0; i < N; i++) {
            previousGen[0][i] = previousGen[gridLines][i];
            previousGen[gridLines + 1][i] = previousGen[1][i];
        }
    };

    for(i = 1; i <= gridLines; i++) {
        for(j = 0; j < N; j++) {
            neighbours = countNeighbors(i, j, previousGen);

            if(previousGen[i][j] && neighbours < 2) newGen[i][j] = 0;
            else if(previousGen[i][j] && (neighbours == 2 || neighbours == 3)) newGen[i][j] = 1;
            else if(previousGen[i][j] && neighbours >= 4) newGen[i][j] = 0;
            else if(!previousGen[i][j] && neighbours == 3) newGen[i][j] = 1;
            else newGen[i][j] = 0;
        }
    }
}

int countGridCells(int **gridParam) {
    int i, j, cells = 0;

    for(i = 1; i < gridLines + 1; i++)
        for(j = 0; j < N; j++)
            cells += gridParam[i][j];

    return cells;
}

int main(int argc, char** argv) {
    int executionTime, cells, i;
    gettimeofday(&startTime, NULL);

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &proccessId);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);

    initGrids();
    
    for(i = 0; i < NUMBER_OF_GENERATIONS; i++) {
       if(i % 2 == 0) startNewGeneration(grid, otherGrid);
       else startNewGeneration(otherGrid, grid);
   }

    if(NUMBER_OF_GENERATIONS % 2 == 0) cells = countGridCells(grid);
    else cells = countGridCells(otherGrid);

    MPI_Finalize();

    if(proccessId == 0) {
        printf("Total de celulas vivas: %d\n", cells);

        gettimeofday(&endTime, NULL);
        executionTime = (int) (endTime.tv_sec - startTime.tv_sec);
        printf("Tempo de execucao: %d segundos\n", executionTime);
    }

    return 0;
}
