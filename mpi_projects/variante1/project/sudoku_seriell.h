#ifndef SUDOKU_SERIELL
#define SUDOKU_SERIELL

//#define DEBUG
#ifdef DEBUG
#  define D(x) x
#else
#  define D(x) 
#endif

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

grids* initParallel(int processCount, int* listSize, char* sudokuFile);

int solveSudoku(struct grid* grid, int row, int col, grids* gridList, int* gridCount);

#endif