#ifndef SUDOKU_SERIELL
#define SUDOKU_SERIELL
// N is the size of the 2D matrix N*N
//#define GRID_SIZE 16

struct grid {
	int* sudoku;
	int size;
};

typedef struct s_grids {
	struct grid grid;
	int start;
	struct grids* next;
} grids;

void delete_grids(grids* oldGrids);

void printBoard(struct grid* grid);

grids* initParallel(int processCount);

int solveSudoku(struct grid* grid, int row, int col, grids* gridList, int* gridCount);

#endif