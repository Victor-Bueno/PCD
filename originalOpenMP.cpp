#include <iostream>
#include <cstdlib>
#include <omp.h>
#include <vector>
#include <utility>

#define N 2048 // Tamanho do tabuleiro NxN
#define NUMBER_OF_THREADS 4 // Quantidade de threads a serem criadas no paralelismo
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

int Neighbours_Count(vector <vector<bool>> &Grid, int X, int Y) {
    int Nb_Total = 0;
    vector <pair<int, int>> Nb_List;

    for (int i = X - 1; i <= X + 1; i++)
        for (int j = Y - 1; j <= Y + 1; j++)
            if (!(i == X && j == Y))
                Nb_List.push_back(make_pair(i, j));


    for (int i = 0; i < 8; i++) {
        if (Nb_List[i].first < 0)
            Nb_List[i].first = Grid.size() - 1;
        else if (Nb_List[i].first > (signed int) Grid.size() - 1)
            Nb_List[i].first = 0;

        if (Nb_List[i].second < 0)
            Nb_List[i].second = Grid.size() - 1;
        else if (Nb_List[i].second > (signed int) Grid.size() - 1)
            Nb_List[i].second = 0;
    }

    for (int i = 0; i < 8; i++)
        if (Grid[Nb_List[i].first][Nb_List[i].second] == 1)
            Nb_Total++;

    return Nb_Total;
}

int Cells_Total(vector <vector<bool>> &Grid) {
    int Cells_Sum = 0;

    for (unsigned int i = 0; i < Grid.size(); i++)
        for (unsigned int j = 0; j < Grid[i].size(); j++)
            if (Grid[i][j] == 1)
                Cells_Sum++;

    return Cells_Sum;
}


bool Cell_Update(vector <vector<bool>> &Grid, int X, int Y) {
    int Nb_Count = Neighbours_Count(Grid, X, Y);

    if (Grid[X][Y] == 0) {
        if (Nb_Count == 3)
            return 1;
    } else {
        if (Nb_Count < 2 || Nb_Count >= 4)
            return 0;
    }

    return Grid[X][Y];
}


void Grid_Update(vector <vector<bool>> &Grid, vector <vector<bool>> &New_Grid) {
    int X, Y;

#pragma omp parallel private(X, Y) shared(Grid, New_Grid)
    {
#pragma omp for collapse(2)
        for (X = 0; X < (int) Grid.size(); X++)
            for (Y = 0; Y < (int) Grid.size(); Y++)
                New_Grid[X][Y] = Cell_Update(Grid, X, Y);
    }
}


int main() {
    omp_set_num_threads(NUMBER_OF_THREADS);
    float startTime = omp_get_wtime();

    vector <vector<bool>> Grid(N, vector<bool>(N));
    vector <vector<bool>> New_Grid(N, vector<bool>(N));

    initNewGrid(Grid);

    cout << "Generation 0: " << Cells_Total(Grid) << endl;

    for (int i = 1; i <= NUMBER_OF_GENERATIONS ; i++) {
        Grid_Update(Grid, New_Grid);
        Grid = New_Grid;
        cout << "Generation " << i << ": " << Cells_Total(Grid) << endl;
    }

    float endTime = omp_get_wtime();
    cout << endl << "Time Elapsed: " << endTime - startTime << endl;

    return 0;
}