/** 
 * @authors: Victor Bueno, Yuri Shiba
*/

#include <iostream>
#include <vector>
#include <utility>
#include <mpi.h>

#define NUMBER_OF_GENERATIONS 100 // Número de gerações
#define N 2048 // Tamanho do tabuleiro NxN

using namespace std;

int elementsPerProcess;
int processRank;
int totalProcesses;

void insertGlider(vector <vector<bool>> &grid) {
    int lin = 1, col = 1;

    grid[lin * N + (col + 1)] = 1;
    grid[(lin + 1) * N + (col + 2)] = 1;
    grid[(lin + 2) * N + col] = 1;
    grid[(lin + 2) * N + (col + 1)] = 1;
    grid[(lin + 2) * N + (col + 2)] = 1;
}

void insertRpentomino(vector <vector<bool>> &grid) {
    int lin = 10, col = 30;

    grid[lin * N + (col + 1)] = 1;
    grid[lin * N + (col + 2)] = 1;
    grid[(lin + 1) * N + col] = 1;
    grid[(lin + 1) * N + (col + 1)] = 1;
    grid[(lin + 2) * N + (col + 1)] = 1;
}

void initNewGrid(vector <vector<bool>> &grid) {
    insertGlider(grid);
    insertRpentomino(grid);
}

int countNeighbours(vector <vector<bool>> &grid, int lin, int col) {
    int counter = 0;
    vector <pair<int, int>> neighbours;

    for (int i = lin - 1; i <= lin + 1; i++) {
        for (int j = col - 1; j <= col + 1; j++) {
            if (!(i == lin && j == col)) {
                neighbours.push_back(make_pair(i + rankProccess * elementsPerProcess, j));
            }
        }
    }

    for (int i = 0; i < 8; i++) {
        if (neighbours[i].first < 0)
            neighbours[i].first = grid.size() - 1;
        else if (neighbours[i].first > (signed int) grid.size() - 1)
            neighbours[i].first = 0;

        if (neighbours[i].second < 0)
            neighbours[i].second = grid.size() - 1;
        else if (neighbours[i].second > (signed int) grid.size() - 1)
            neighbours[i].second = 0;
    }

    for (int i = 0; i < 8; i++) {
        if (grid[neighbours[i].first * N + neighbours[i].second] == 1) counter++;
    }

    return counter;
}

// Conta o número de células vivas no tabuleiro atual
int countGridCells(vector <vector<bool>> &grid) {
    int count = 0;

    for (unsigned int i = 0; i < grid.size(); i++) {
        for (unsigned int j = 0; j < grid[i].size(); j++) {
            if (grid[i * N + j] == 1) count++;
        }
    }

    return count;
}

// Atualiza as mudaças individuais em relação à célula após cada iteração
bool recalculateCell(bool *partOfGrid, vector <vector<bool>> &grid, int lin, int col) {
    int neighboursNumber = countNeighbours(grid, lin, col);

    if (partOfGrid[lin * N + col] == 0) {
        if (neighboursNumber == 3) return 1;
    } else {
        if (neighboursNumber < 2 || neighboursNumber >= 4) return 0;
    }

    return partOfGrid[lin * N + col];
}

// Atualiza as mudanças no tabuleiro após cada iteração
void recalculateGrid(vector <vector<bool>> &grid, vector <vector<bool>> &newGrid) {
    int lin, col;

    bool *partOfGrid = new bool[elementsPerProcess];
	bool *newPartOfGrid = new bool[elementsPerProcess];

    MPI_Scatter(grid, elementsPerProcess, MPI_CXX_BOOL, partOfGrid, elementsPerProcess, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);

    for (lin = 0; lin < elementsPerProcess / N; lin++)
        for (col = 0; col < N; col++)
            newPartOfGrid[lin * N + col] = recalculateCell(partOfGrid, grid, lin, col);

    MPI_Barrier(MPI_COMM_WORLD);

	MPI_Allgather(newPartOfGrid, elementsPerProcess, MPI_CXX_BOOL, newGrid, N * N, MPI_CXX_BOOL, MPI_COMM_WORLD);
}

int main() {
    MPI_Init(NULL, NULL);

	MPI_Comm_size(MPI_COMM_WORLD, &totalProcesses);
  	MPI_Comm_rank(MPI_COMM_WORLD, &processRank);

    elementsPerProcess = N * N / totalProcesses;

    double startTime, endTime;

    bool *grid = new bool[N * N];
    bool *newGrid = new bool[N * N];

    initNewGrid(grid);

    if(processRank == 0)
        cout << "=-=-=> Gen 0: " << countGridCells(grid) << endl;

    startTime = MPI_Wtime();
    for (int i = 1; i <= NUMBER_OF_GENERATIONS ; i++) {
        recalculateGrid(grid, newGrid);
        
        grid = newGrid;
        if(processRank == 0)
            cout << "=-=-=> Gen " << i << ": " << countGridCells(grid) << " cells alive!" << endl;
    }
    endTime = MPI_Wtime();

    if(processRank == 0)
        cout << "/--->> Time: " << endTime - startTime << endl;

    MPI_Finalize();

    return 0;
}