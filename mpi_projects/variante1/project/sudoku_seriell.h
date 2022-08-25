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

typedef struct grids {
	struct grid item;
	struct grids_queue *next;
} grids_queue;

typedef struct processes {
	int pid;
	struct processes_queue* next;
} processes_queue;

const static struct grid emptyGrid = { NULL, 0 };

void enqueue(grids_queue** head, struct grid val);

struct grid dequeue(grids_queue** head);

void intEnqueue(processes_queue** head, int pid);

int intDequeue(processes_queue** head);

void delete_grids(grids_queue* oldGrids);

void printBoard(struct grid* grid);

void printPartSudokus(grids_queue* partSudokus);

grids_queue* initParallel(int processCount, int* listSize, char* sudokuFile);

int solveSudoku(struct grid* grid, int row, int col, grids_queue** grid_queue, int* gridCount, int cacheGrids);

int attemptGridCache(struct grid sudokuGrid);

#endif