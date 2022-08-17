#ifndef SUDOKU_SERIELL
#define SUDOKU_SERIELL
// N is the size of the 2D matrix N*N
#define N 9

struct grid {
	int sudoku[N][N];
};

typedef struct s_grids {
	struct grid grid;
	int start;
	struct grids* next;
} grids;

void delete_grids(grids* oldGrids);

void printBoard(struct grid* grid);

grids* initParallel(int processCount, int* listCount);

int solveSudoku2(struct grid *grid, int row, int col, grids* gridList, int* gridCount);

#endif