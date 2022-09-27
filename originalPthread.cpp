#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <vector>
#include <stack>
#include <time.h>
#include <sys/time.h>

using namespace std;

#define N 2048 // Tamanho do tabuleiro NxN
#define NUMBER_OF_THREADS 2 // Quantidade de threads a serem criadas no paralelismo
#define NUMBER_OF_GENERATIONS 2000 // Número de iterações de atualização do tabuleiro

struct ArgumentsType {
    vector <vector<bool>>* grid;
    vector <vector<bool>>* otherGrid;
    stack <pair<int, int>>* locationStack;
};


// Inicializa um novo tabuleiro construindo as figuras de glider e r-pentomino
void initNewGrid(vector<vector<bool>> &newGrid) {
	int lin = 1, col = 1;
	newGrid[lin][col + 1] = 1;
	newGrid[lin + 1][col + 2] = 1;
	newGrid[lin + 2][col] = 1;
	newGrid[lin + 2][col + 1] = 1;
	newGrid[lin + 2][col + 2] = 1;

	lin = 10; col = 30;
	newGrid[lin][col + 1] = 1;
	newGrid[lin][col + 2] = 1;
	newGrid[lin + 1][col  ] = 1;
	newGrid[lin + 1][col + 1] = 1;
	newGrid[lin + 2][col + 1] = 1;
}

// Calcula a quantidade de vizinhos dada uma posição e um grid
int countNeighbours(vector<vector<bool>> &grid, int lin, int col) {
    int counter = 0;
    vector <pair <int, int>> neighbours;

    for(int i = lin - 1; i <= lin + 1; i++) {
        for(int j = col - 1; j <= col + 1; j++) {
            if (!(i == lin && j == col)) neighbours.push_back(make_pair(i, j));
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
        if (grid[neighbours[i].first][neighbours[i].second] == 1) 
            counter++;
    }

    return counter;
}

// Conta o número de células vivas no tabuleiro atual
int countGridCells(vector<vector<bool>> &grid) {
	int count = 0;

	for(int i = 0; i < N; i++) {
		for(int j = 0; j < N; j++) {
			if(grid[i][j]) count++; 
		}
	}

	return count;
}

// Atualiza as mudaças individuais em relação à célula após cada iteração
void *recalculateCell(void *arguments) {
	struct ArgumentsType* threadInfo = (struct ArgumentsType*) arguments;

    vector <vector<bool>> *grid = threadInfo->grid, *otherGrid = threadInfo->otherGrid;
    stack <pair<int, int>> *locationStack = threadInfo->locationStack;

    int X, Y;
    while(!(*locationStack).empty()) {
        X = (*locationStack).top().first;
        Y = (*locationStack).top().second;

        (*locationStack).pop();

        int counterOfNeighbours = countNeighbours(*grid, X, Y);
    
        if ((*grid)[X][Y] == 0) {
            if (counterOfNeighbours == 3) {
                (*otherGrid)[X][Y] = 1;
                continue;
            }
        } else {
            if (counterOfNeighbours < 2 || counterOfNeighbours >= 4) {
                (*otherGrid)[X][Y] = 0;
                continue;
            }       
        }
    
        (*otherGrid)[X][Y] = (*grid)[X][Y];
    }

    pthread_exit(NULL);
}

// Atualiza as mudanças no tabuleiro após cada iteração
void recalculateGrid(vector<vector<bool>> &grid, vector<vector<bool>> &otherGrid) {
    pthread_t threads[NUMBER_OF_THREADS];
    int threadsCounter = 0;

    struct ArgumentsType arguments[NUMBER_OF_THREADS];
    stack <pair <int, int>> locationStack[NUMBER_OF_THREADS];

    for (int i = 0; i < (int)grid.size(); i++) {
        for (int j = 0; j < (int)grid[0].size(); j++) {
            locationStack[threadsCounter % NUMBER_OF_THREADS].push(make_pair(i, j));
            threadsCounter++;
        }
    }

    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        arguments[i].grid = &grid;
        arguments[i].otherGrid = &otherGrid;
        arguments[i].locationStack = &(locationStack[i]);
        pthread_create(&threads[i], NULL, recalculateCell, (void*) &(arguments[i]));
    }

    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}

//==================================================================================================================================

int main() {
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);	

    vector<vector<bool>> grid(N, vector<bool>(N)); // Fonte: https://stackoverflow.com/questions/12375591/vector-of-vectors-to-create-matrix
    vector<vector<bool>> otherGrid(N, vector<bool> (N));

    initNewGrid(grid);

    cout << "=-=-=> Gen 0: " << countGridCells(grid) << " cells alive!" << endl;

    for (int i = 1; i <= NUMBER_OF_GENERATIONS; i++) {
        recalculateGrid(grid, otherGrid);

        grid = otherGrid;
        cout << "=-=-=> Gen " << i << ": " << countGridCells(grid) << " cells alive!" << endl;
    }

    gettimeofday(&endTime, NULL);
    cout << endl << "/--->> Time: " << (int) (endTime.tv_sec - startTime.tv_sec) << endl;

    return 0;
}