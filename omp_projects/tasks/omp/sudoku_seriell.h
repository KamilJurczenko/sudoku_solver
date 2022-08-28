#ifndef SUDOKU_SERIELL
#define SUDOKU_SERIELL

//#define DEBUG
#ifdef DEBUG
#  define D(x) x
#else
#  define D(x) 
#endif

//extern double solutionTime;

struct grid {
	int* sudoku;
	int size;
};

typedef struct nums {
	int pid;
	struct num_queue* next;
} num_queue;

void intEnqueue(num_queue** head, int pid);

int intDequeue(num_queue** head, int freeFirst);

#endif