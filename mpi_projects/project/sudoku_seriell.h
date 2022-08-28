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
	struct grids_queue* next;
} grids_queue;

typedef struct nums {
	int pid;
	struct num_queue* next;
} num_queue;

void enqueue(grids_queue** head, struct grid val);

struct grid dequeue(grids_queue** head);

void intEnqueue(num_queue** head, int pid);

int intDequeue(num_queue** head, int freeFirst);

#endif