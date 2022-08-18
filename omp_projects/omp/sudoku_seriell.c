/********************************************************************

Project:

Modified by:
(sorted by lastname)
	Daitche Konstantin
	Jurczenko Kamil

Original Author:
	Pradeep Mondal P

*********************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "sudoku_seriell.h"
#include <direct.h>
#include <time.h>
#define getcwd _getcwd
//#include <windows.h>

// Linked List Functions
grids* create_grid(struct grid newGrid, int start) {
	grids* newGrids = malloc(sizeof(grids));
	if (NULL != newGrids) {
		newGrids->grid = newGrid;
		newGrids->next = NULL;
		if (start) newGrids->start = 1;
	}
	return newGrids;
}

void insert_end(grids* gridList, struct grid grid)
{
	grids* ptr;
	ptr = gridList;
	size_t size = grid.size * grid.size * sizeof(int);
	int* cpyArr = malloc(size);
	int* srcArr = grid.sudoku;
	memcpy(cpyArr, srcArr, size);
	struct grid cpyGrid;
	cpyGrid.sudoku = cpyArr;
	cpyGrid.size = grid.size;
	//printBoard(&cpyGrid);
	//printBoard(&cpyGrid);
	if (ptr->start != 1)
	{
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
		}
		ptr->next = create_grid(cpyGrid, 0);
	}
	//overwrite root Grid
	else
	{
		ptr->start = 0;
		ptr->grid = cpyGrid;
	}
}

void delete_grids(grids* oldGrids) {
	if (NULL != oldGrids->next) {
		delete_grids(oldGrids->next);
	}
	free(oldGrids->grid.sudoku);
	free(oldGrids);
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
			printf(" %d", grid->sudoku[i*grid->size + j]);
		}
		printf("\n");
	}
	printf("\n");
}

void printPartSudokus(grids* su)
{
	grids* b = su;
	while (b != NULL)
	{
		printBoard(&b->grid);
		b = b->next;
	}
}

/* Takes a partially filled-in grid and attempts
to assign values to all unassigned locations in
such a way to meet the requirements for
Sudoku solution (non-duplication across rows,
columns, and boxes)
Return 1 when Solution exist, 0 when no Solution*/
int solveSudoku(struct grid* grid, int row, int col, grids* gridList, int* gridCount)
{
	int index = row * grid->size + col;
	// Check if we have reached the 8th row
	// and 9th column (0
	// indexed matrix) , we are
	// returning true to avoid
	// further backtracking
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
		return solveSudoku(grid, row, col + 1, gridList, gridCount);

	for (int num = 1; num <= grid->size; num++)
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
			if (gridList != NULL && &gridCount != NULL)
			{
				//printBoard(grid);
				// Add grid to list
				insert_end(gridList, *grid);
				//printBoard(&gridList->grid);
				//printPartSudokus(gridList);
				// Set Counter
				*gridCount += 1;
				continue;
			}

			// Checking for next possibility with next
			// column
			if (solveSudoku(grid, row, col + 1, NULL, gridCount) == 1)
				return 1;
		}

		// Removing the assigned num ,
		// since our assumption
		// was wrong , and we go for next
		// assumption with
		// diff num value
		grid->sudoku[index] = 0;
	}
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

// ToDo fehlerzüstände abfangen wenn zu viele Prozesse für das Sudokufeld gewählt wurden
grids* initParallel(int processCount, int* sudokusCount, char* sudokuFile)
{
	struct grid rootGrid;

	int gridSize;
	/*
	char buildPath[] = "..\\..\\..\\sudokus\\"; // TODO use as argument?
	char debugPath[] = "..\\..\\sudokus\\";
	char fromScript[] = "..\\sudokus\\";
	char* sudokuPath = fromScript; // Change here
	char* path = malloc(strlen(sudokuPath) + strlen(sudokuFile) + 1);
	strcpy(path, sudokuPath);
	strcat(path, sudokuFile);
	*/
	char cwd[512];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		D(printf("Current working dir: %s\n", cwd));
	}
	//sudokuNormal9 , sudokuNormal16, sudokuHard16, sudokuExtreme25, sudoku100 
	//rootGrid = readGridFromFile(strcat(buildPath, file), &gridSize); // fürs exec
	rootGrid = readGridFromFile(sudokuFile, &gridSize); // fürs debuggen

	//printf("Starting Sudoku: \n");
	//printBoard(&rootGrid);

	// init Grid List with starting Sudoku Grid
	grids* gridList = create_grid(rootGrid, 0);
	*sudokusCount = 1;

	//int currentDepth = -1; // counter val for current depth of breadth search

	// breadth search till breadth = num or breadth < num
	while (*sudokusCount < processCount)
	{
		// tmp variables for one breadth graph iteration in a depth
		int tmpListCount = 0;
		grids* tmpGridList = create_grid(rootGrid, 1);
		grids* currentGrid = gridList; // pointer to first Grid Element
		// Iterate one graph depth
		while (currentGrid != NULL)
		{
			solveSudoku(&currentGrid->grid, 0, 0, tmpGridList, &tmpListCount);
			//printBoard(&tmpGridList->grid);
			currentGrid = currentGrid->next;
		}

		/*grids* test = tmpGridList->next;
		grids* test2 = test->next;*/

		delete_grids(gridList);
		gridList = tmpGridList;
		*sudokusCount = tmpListCount;
		//currentDepth++;
		//if (currentDepth == 50000) break; // for debugging
	}
	// Print found starting Sudokus and further information			
	D(printf("Found %d Sudokus to parallelize\n", *sudokusCount));
	printf("%d\n", *sudokusCount);
	//printPartSudokus(gridList); // for debugging
	return gridList;
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