#include <omp.h>
#include "sudoku_seriell.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include <direct.h>

double start_time;
int solution_found;
int threadNum;

struct grid copyGrid(struct grid grid)
{
	size_t size = grid.size * grid.size * sizeof(int);
	int* cpy_sudoku = malloc(size);
	int* src_sudoku = grid.sudoku;
	memcpy(cpy_sudoku, src_sudoku, size);
	struct grid cpyGrid;
	cpyGrid.sudoku = cpy_sudoku;
	cpyGrid.size = grid.size;
	return cpyGrid;
}

int isSafe(struct grid g, int row, int col, int num)
{
	for (int x = 0; x <= g.size - 1; x++)
		if (g.sudoku[row * g.size + x] == num)
			return 0;

	for (int x = 0; x <= g.size - 1; x++)
		if (g.sudoku[x * g.size + col] == num)
			return 0;

	int startRow = row - row % (int)sqrt(g.size),
		startCol = col - col % (int)sqrt(g.size);

	for (int i = 0; i < (int)sqrt(g.size); i++)
		for (int j = 0; j < (int)sqrt(g.size); j++)
			if (g.sudoku[(startRow * g.size + i) + (j * g.size + startCol)] == num)
				return 0;

	return 1;
}

void printBoard(struct grid* grid)
{
	for (int i = 0; i < grid->size; i++)
	{
		for (int j = 0; j < grid->size; j++)
		{
			printf(" %d", grid->sudoku[i * grid->size + j]);
		}
		printf("\n");
	}
	printf("\n");

}

int solveSudoku(struct grid* grid, int row, int col, int depth, int maxDepth)
{
	int index = row * grid->size + col;

	if (row == grid->size - 1 && col == grid->size)
		return 1;

	if (col == grid->size)
	{
		row++;
		col = 0;
	}

	if (grid->sudoku[index] > 0)
		return solveSudoku(grid, row, col + 1, depth, maxDepth);

	num_queue* numsQueue = NULL;
	int followingNodes = 0;
	// First for loop to check for multiple next possiblities to attempt to cache one
	for (int num = 1; num <= grid->size; num++)
	{
		if (solution_found == 1)
			break;
		if (isSafe(*grid, row, col, num) == 1)
		{
			intEnqueue(&numsQueue, num);
			followingNodes++;
		}
	}
	int num;
	while ((num = intDequeue(&numsQueue, 1)) > 0)
	{
		grid->sudoku[index] = num;

		if (followingNodes > 1 && depth < maxDepth)
		{
			struct grid copiedGrid;
			copiedGrid = copyGrid(*grid);
			#pragma omp task firstprivate(copiedGrid,row,col,depth,maxDepth) shared(start_time)
			{
				if (solveSudoku(&copiedGrid, row, col + 1, depth + 1, maxDepth) == 1)
				{
					//printBoard(&copiedGrid);
					double solutionTime = omp_get_wtime() - start_time;
					solution_found = 1;
					printf("%f\n", solutionTime);
					//exit(0);
				}
				free(copiedGrid.sudoku);
			}
		}
		else if (solveSudoku(grid, row, col + 1, depth, maxDepth) == 1)
			return 1;
		followingNodes--;
	}
	grid->sudoku[index] = 0;
	return 0;
}

struct grid readGridFromFile(char* fileName)
{
	// Erste Zeile ist die Größe des Sudokus gefolgt von keinem Leerzeichen
	// Sonst endet jede Zeile mit einem Leerzeichen

	FILE* myFile = fopen(fileName, "r");
	int   i,      // helper var , use in the for loop
		n,      // helper var, use to read chars and create numbers
		row = -1,  // rows in the array
		col,    // columns in the array
		zero = (int)'0';  // helper when converting chars to integers
	int* arr;
	arr = NULL;
	struct grid grid;
	int lineLen = 2048;
	char line[2048]; // long enough to pickup one line in the input/text file
	if (myFile) {
		int sizeSet = 0;
		//arr = (int*)malloc(sizeof(int) *pow(*gridSize,2));
		while (fgets(line, 2048, myFile)) {
			col = 0;
			n = 0;   
			for (i = 0; i < strlen(line); i++) {
				if (line[i] >= '0' && line[i] <= '9') n = 10 * n + (line[i] - zero); // converter char to int
				else {
					if (sizeSet == 0)
					{
						// allocate 2D arr
						grid.size = n;
						arr = malloc(grid.size * grid.size * sizeof(int));

						sizeSet = 1;
					}
					else {
						arr[row * grid.size + col] = n;
						col++;
					}
					n = 0;
				}
			}
			row++;
		}
		fclose(myFile);
	}
	else {
		fprintf(stderr, "can't open %s: %s\n", fileName, strerror(errno));
		exit(1);
	}

	grid.sudoku = arr;
	//printBoard(&grid);
	return grid;
}

int main(int argc, char** argv) 
{
	threadNum = atoi(argv[1]);
	char* file = argv[2];
	// Initialization
	double startInit_time = omp_get_wtime();
	struct grid rootGrid = readGridFromFile(file);
	solution_found = 0;
	
	/*
	if (1)
	{
	int t;
	scanf("%i", &t);
	}
	*/
	
	// CODE START

	if (threadNum == 0)
		return 0;
	start_time = omp_get_wtime();

	#pragma omp parallel num_threads(threadNum)
	{
		#pragma omp single nowait
			solveSudoku(&rootGrid, 0, 0, 0, threadNum);
	}

	return 0;
}
