/********************************************************************

Project:

Modified by:
(sorted by lastname)
	Daitche Konstantin
	Jurczenko Kamil

Original Author:
	Pradeep Mondal P

*********************************************************************/

//#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "sudoku_seriell.h"

#include <direct.h>
#include <time.h>
#define getcwd _getcwd
#include <windows.h>

void enqueue_cpy(grids_queue** grid_queue, struct grid grid)
{
	size_t size = grid.size * grid.size * sizeof(int);
	int* cpy_sudoku = malloc(size);
	int* src_sudoku = grid.sudoku;
	memcpy(cpy_sudoku, src_sudoku, size);
	struct grid cpyGrid;
	cpyGrid.sudoku = cpy_sudoku;
	cpyGrid.size = grid.size;
	enqueue(grid_queue, cpyGrid);
	//printBoard(&cpyGrid);
}

void delete_grids(grids_queue* oldGrids) {
	if(dequeue(&oldGrids).size != NULL)
		delete_grids(oldGrids);
}

// Checks whether it will be legal
// to assign num to the
// given row, col
int isSafe(struct grid g, int row,
	int col, int num)
{
	// Check if we find the same num
	// in the similar row , we return 0
	for (int x = 0; x <= g.size - 1; x++)
		if (g.sudoku[row * g.size + x] == num)
			return 0;

	// Check if we find the same num in the
	// similar column , we return 0
	for (int x = 0; x <= g.size - 1; x++)
		if (g.sudoku[x * g.size + col] == num)
			return 0;

	// Check if we find the same num in the
	// particular 3*3 matrix, we return 0
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

void printPartSudokus(grids_queue* partSudokus)
{
	grids_queue* tmp_partSudokus = partSudokus;
	while (tmp_partSudokus != NULL)
	{
		printBoard(&tmp_partSudokus->item);
		tmp_partSudokus = tmp_partSudokus->next;
	}
}

/* Takes a partially filled-in grid and attempts
to assign values to all unassigned locations in
such a way to meet the requirements for
Sudoku solution (non-duplication across rows,
columns, and boxes)
Return 1 when Solution exist, 0 when no Solution*/
int solveSudoku(struct grid* grid, int row, int col, grids_queue** gridQueue, int* gridCount, int cacheGrids)
{
	int index = row * grid->size + col;
	//if(index > 170)
	//	printBoard(grid);
	// Check if we have reached the 8th row
	// and 9th column (0
	// indexed matrix) , we are
	// returning true to avoid
	// further backtracking
	// Solution found!
	if (row == grid->size - 1 && col == grid->size)
		return 1;

	// Check if column value becomes 9 ,
	// we move to next row and
	// column start from 0
	if (col == grid->size)
	{
		row++;
		col = 0;
	}

	// Check if the current position
	// of the grid already contains
	// value >0, we iterate for next column

	if (grid->sudoku[index] > 0)
		return solveSudoku(grid, row, col + 1, gridQueue, gridCount, cacheGrids);

	int multiplePaths = 0;
	// First for loop to check for multiple next possiblities to attempt to cache one for manager process
	if (cacheGrids == 1) 
	{
		int count = 0;
		for (int num = 1; num <= grid->size; num++)
		{
			if (isSafe(*grid, row, col, num) == 1)
			{
				count++;
				if (count > 1) 
				{
					multiplePaths = 1;
					break;
				}
			}
		}
	}
	int cachedGrid = 0;
	for (int num = 0; num <= grid->size; num++)
	{
		// Check if it is safe to place
		// the num (1-9) in the
		// given row ,col ->we move to next column
		if (isSafe(*grid, row, col, num) == 1)
		{
			/* assigning the num in the
			current (row,col)
			position of the grid
			and assuming our assigned num
			in the position
			is correct	 */
			grid->sudoku[index] = num;

			// Cache grid if starting Grid needed and return to deny backtracking
			if (gridQueue != NULL && &gridCount != NULL)
			{
				//printBoard(grid);
				// Add grid to list
				enqueue_cpy(gridQueue, *grid);
				//printBoard(&gridList->grid);
				//printPartSudokus(gridList);
				// Set Counter
				*gridCount += 1;
			}
			// cache grid
			else if (multiplePaths == 1 && attemptGridCache(*grid) == 1)
			{
				multiplePaths = 0;
			}
			else if (solveSudoku(grid, row, col + 1, NULL, NULL, cacheGrids) == 1)
				return 1;
		}
		// Removing the assigned num ,
		// since our assumption
		// was wrong , and we go for next
		// assumption with
		// diff num value
		grid->sudoku[index] = 0;
	}
	// No Solution return 0;
	return 0;
}

struct grid readGridFromFile(char* fileName)
{
	// Erste Zeile ist die Gr��e des Sudokus gefolgt von keinem Leerzeichen
	// Sonst endet jede Zeile mit einem Leerzeichen

	FILE* myFile = fopen(fileName, "r");
	int   i,      // helper var , use in the for loop
		n,      // helper var, use to read chars and create numbers
		row = -1,  // rows in the array
		col,    // columns in the array
		zero = (int)'0';  // helper when converting chars to integers
	//int(*arr)[GRID_SIZE] = malloc(sizeof(int[GRID_SIZE][GRID_SIZE]));
	int* arr;
	arr = NULL;
	struct grid grid;
	//int arr[GRID_SIZE][GRID_SIZE]; static 1MB on stack!
	int lineLen = 2048;
	char line[2048]; // long enough to pickup one line in the input/text file
	if (myFile) {
		int sizeSet = 0;
		//arr = (int*)malloc(sizeof(int) *pow(*gridSize,2));
		while (fgets(line, 2048, myFile)) {
			col = 0;
			n = 0;          // will help in converting digits into decimal number
			/*if (row == 24) {
				printf("debug");
			}*/
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

// ToDo fehlerz�st�nde abfangen wenn zu viele Prozesse f�r das Sudokufeld gew�hlt wurden
grids_queue* initParallel(int processCount, int* sudokusCount, char* sudokuFile)
{
	struct grid rootGrid;
	int gridSize;

	char cwd[512];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		D(printf("Current working dir: %s\n", cwd));
	}

	rootGrid = readGridFromFile(sudokuFile);

	//printf("Starting Sudoku: \n");
	//printBoard(&rootGrid);

	// init Grid Queue with starting Sudoku Grid
	grids_queue* starting_gridQueue = NULL;
	enqueue(&starting_gridQueue, rootGrid);
	*sudokusCount = 1;
	//int currentDepth = -1; // counter val for current depth of breadth search

	// breadth search till breadth = num or breadth < num
	while (*sudokusCount < processCount)
	{
		// tmp variables for one breadth graph iteration in a depth
		int tmpListCount = 0;
		grids_queue* tmp_gridQueue = NULL;
		grids_queue* ptr = starting_gridQueue; // pointer to first Grid Element
		// Iterate one graph depth
		struct grid nextGrid;
		while (ptr != NULL)
		{
			struct grid mGrid = dequeue(&starting_gridQueue);
			//printBoard(&mGrid);
			solveSudoku(&mGrid, 0, 0, &tmp_gridQueue, &tmpListCount,0);
			ptr = starting_gridQueue;
			//printBoard(&tmpGridList->grid);
		}

		delete_grids(starting_gridQueue);
		starting_gridQueue = tmp_gridQueue;
		*sudokusCount = tmpListCount;
		//currentDepth++;
		//if (currentDepth == 50000) break; // for debugging
	}
	// Print found starting Sudokus and further information			
	D(printf("Found %d Sudokus to parallelize\n", *sudokusCount));
	printf("%d\n", *sudokusCount);
	//printPartSudokus(starting_gridQueue); // for debugging
	return starting_gridQueue;
}
/*
int main()
{
	clock_t beginTime = clock();


	//struct grid startingSudoku;
	//int g[N][N] = {{ 3, 0, 6, 5, 0, 8, 4, 0, 0 },
	//			   { 5, 2, 0, 0, 0, 0, 0, 0, 0 },
	//			   { 0, 8, 7, 0, 0, 0, 0, 3, 1 },
	//			   { 0, 0, 3, 0, 1, 0, 0, 8, 0 },
	//			   { 9, 0, 0, 8, 6, 3, 0, 0, 5 },
	//			   { 0, 5, 0, 0, 9, 0, 6, 0, 0 },
	//			   { 1, 3, 0, 0, 0, 0, 2, 5, 0 },
	//			   { 0, 0, 0, 0, 0, 0, 0, 7, 4 },
	//			   { 0, 0, 5, 2, 0, 6, 3, 0, 0 } };

	grids* g = initParallel(80); // 25x25 Sudoku 2401 erstellte Sudokus Zeit: 0.044 s


	// free memory
	delete_grids(g);


	clock_t endTime = clock();
	double time_spent = (double)(endTime - beginTime) / CLOCKS_PER_SEC;
	printf("Time spent: %lf\n", time_spent);

	return 0;

}
*/