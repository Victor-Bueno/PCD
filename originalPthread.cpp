#include <cstdlib>
#include <vector>
#include <pthread.h>

#define N 10 // Tamanho do tabuleiro NxN
#define NUMBER_OF_THREADS 2 // Quantidade de threads a serem criadas no paralelismo
#define NUMBER_OF_GENERATIONS 2000 // Número de iterações de atualização do tabuleiro


struct ArgumentsType {
    vector<vector<bool>>* grid;
    vector<vector<bool>>* otherGrid;
    int i;
    int j;
};

int countGridCells(vector<vector<bool>> &grid) {
	int count = 0;

	for(int i = 0; i < N; i++) {
		for(int j = 0; j < N; j++) {
			if(grid[i][j]) count++; 
		}
	}

	return count;
}

// Inicializa um novo tabuleiro construindo as figuras de glider e r-pentomino
void initNewGrid(vector<vector<bool>> &newGrid) {
	int lin = 1, col = 1;
	newGrid[lin][col + 1] = 1;
	newGrid[lin + 1][col + 2] = 1;
	newGrid[lin + 2][col] = 1;
	newGrid[lin + 2][col + 1] = 1;
	newGrid[lin + 2][col + 2] = 1;

	lin = 10; col = 30;
	grid[lin][col + 1] = 1;
	grid[lin][col + 2] = 1;
	grid[lin + 1][col  ] = 1;
	grid[lin + 1][col + 1] = 1;
	grid[lin + 2][col + 1] = 1
}

// Calcula a quantidade de vizinhos dada uma posição e um grid
int countNeighbours(vector<vector<bool>> *grid, int lin, int col) {
	int counter = 0;

	// Vizinho superior:
	if(lin == 0) {
		if (grid[N - 1][col] == 1) counter++;
	} else if(grid[lin - 1][col] == 1) counter++;

	// Vizinho à direita:
	if(col == N) {
		if (grid[lin][0] == 1) counter++;
	} else if(grid[lin][col + 1] == 1) counter++;
	
	// Vizinho à esquerda:
	if(col == 0) {
		if (grid[lin][N - 1] == 1) counter++;
	} else if(grid[lin][col - 1] == 1) counter++;

	// Vizinho inferior:
	if(lin == N) {
		if (grid[0][col] == 1) counter++;
	} else if(grid[lin + 1][col] == 1) counter++;

	// Vizinho diagonal superior esquerda:

	// Vizinho diagonal superior direita:

	// Vizinho diagonal inferior esquerda:
	
	// Vizinho diagonal inferior direita:
	

	return counter;
}

// Atualiza as mudaças individuais em relação à célula após cada iteração
void *recalculateCell(void *arguments) {
	struct ArgumentsType* threadInfo = (struct ArgumentsType*) arguments

	vector<vector<bool>> *grid = threadInfo->grid, *otherGrid = threadInfo->otherGrid;
	int lin = threadInfo->i, col = 	threadInfo->j;

	int numberOfNeighbours = countNeighbours(grid, lin, col);
}

// Atualiza as mudanças no tabuleiro após cada iteração
void recalculateGrid(vector<vector<bool>> &grid, vector<vector<bool>> &otherGrid) {
	pthread_t threads[NUMBER_OF_THREADS];
	int threadsCounter = 0;

    struct ArgumentsType *arguments;

	for(int i = 0; i < N; i++) {
		for(int j = 0; j < N; j++) {
			if ((threadsCounter % NUMBER_OF_THREADS == 0) && threadsCounter != 0) {
        		for (int i = 0; i < NUMBER_OF_THREADS; i++) {
                    pthread_join(Threads[i], NULL);
                }        
			}

			arguments = (struct ArgumentsType*) malloc(sizeof(struct ArgumentsType));


            arguments->grid = &grid;
            arguments->otherGrid = &otherGrid;
            arguments->i = i;
            arguments->j = j;

            pthread_create(&threads[threadsCounter % NUMBER_OF_THREADS], NULL, recalculateCell, (void*) arguments);

            threadsCounter++;
		}
	}
}

int main() {
	vector<vector<bool>> grid(N, vector<bool>(N)); // Fonte: https://stackoverflow.com/questions/12375591/vector-of-vectors-to-create-matrix
	vector<vector<bool>> otherGrid(N, vector<bool>(N));

	initNewGrid(grid);

	cout << "=-=-=> Gen 0: " << countGridCells(grid) << " cells alive!" << endl;

    for (int i = 1; i <= NUMBER_OF_GENERATIONS; i++) {
        recalculateGrid(grid, otherGrid);

        grid = otherGrid;
        cout << "=-=-=> Gen " << i << ": " << countGridCells(grid) << " cells alive!" << endl;
    }

    return 0;
}