/** 
 * @authors: Victor Bueno, Yuri Shiba
*/

#include <iostream>
#include <omp.h>
#include <vector>
#include <utility>

#define N 2048 // Tamanho do tabuleiro NxN
#define NUMBER_OF_THREADS 8 // Quantidade de threads a serem criadas no paralelismo
#define NUMBER_OF_GENERATIONS 2000 // Número de iterações de atualização do tabuleiro

using namespace std;

void insertGlider(vector <vector<bool>> &grid) {
    int lin = 1, col = 1;
    grid[lin][col + 1] = 1;
    grid[lin + 1][col + 2] = 1;
    grid[lin + 2][col] = 1;
    grid[lin + 2][col + 1] = 1;
    grid[lin + 2][col + 2] = 1;
}

void insertRpentomino(vector <vector<bool>> &grid) {
    int lin = 10;
    int col = 30;
    grid[lin][col + 1] = 1;
    grid[lin][col + 2] = 1;
    grid[lin + 1][col] = 1;
    grid[lin + 1][col + 1] = 1;
    grid[lin + 2][col + 1] = 1;
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
                neighbours.push_back(make_pair(i, j));
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
        if (grid[neighbours[i].first][neighbours[i].second] == 1) counter++;
    }

    return counter;
}

// Conta o número de células vivas no tabuleiro atual
int count = 0;
void countGridCells(vector <vector<bool>> &grid){
    int i, j;

    #pragma omp parallel shared(grid) private(i,j) reduction(+:count) num_threads(NUMBER_OF_THREADS)
        #pragma omp for
            for(i=0; i < N; i++)
                for (j = 0; j < N; j++) 
                    if(grid[i][j]) 
                        count++;
        
    
    cout << "Total grid cells: " << count << endl;
}

// Atualiza as mudaças individuais em relação à célula após cada iteração
bool recalculateCell(vector <vector<bool>> &grid, int lin, int col) {
    int neighboursNumber = countNeighbours(grid, lin, col);

    if (grid[lin][col] == 0) {
        if (neighboursNumber == 3) return 1;
    } else {
        if (neighboursNumber < 2 || neighboursNumber >= 4) return 0;
    }

    return grid[lin][col];
}

// Atualiza as mudanças no tabuleiro após cada iteração
void recalculateGrid(vector <vector<bool>> &grid, vector <vector<bool>> &newGrid) {
    int lin, col;

#pragma omp parallel private(lin, col) shared(grid, newGrid)
    {
#pragma omp for collapse(2)
        for (lin = 0; lin < (int) grid.size(); lin++)
            for (col = 0; col < (int) grid.size(); col++)
                newGrid[lin][col] = recalculateCell(grid, lin, col);
    }
}

int main() {
    omp_set_num_threads(NUMBER_OF_THREADS);

    float startTime, endTime;

    vector <vector<bool>> grid(N, vector<bool>(N));
    vector <vector<bool>> newGrid(N, vector<bool>(N));

    initNewGrid(grid);

    // cout << "=-=-=> Gen 0: " << countGridCells(grid) << endl;

    for (int i = 1; i <= NUMBER_OF_GENERATIONS ; i++) {
        recalculateGrid(grid, newGrid);
        
        grid = newGrid;
        // cout << "=-=-=> Gen " << i << ": " << countGridCells(grid) << " cells alive!" << endl;
    }

    startTime = omp_get_wtime();
    countGridCells(grid);
    endTime = omp_get_wtime();

    cout << "/--->> Time: " << endTime - startTime << endl;

    return 0;
}