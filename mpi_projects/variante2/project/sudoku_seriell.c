/********************************************************************

Project:

Modified by:
(sorted by lastname)
	Daitche Konstantin
	Jurczenko Kamil

Original Author:
	Pradeep Mondal P

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "sudoku_seriell.h"

// N is the size of the 2D matrix N*N
//#define N 9
//
int g[N][N] = { { 3, 0, 6, 5, 0, 8, 4, 0, 0 },
					{ 5, 2, 0, 0, 0, 0, 0, 0, 0 },
					{ 0, 8, 7, 0, 0, 0, 0, 3, 1 },
				    { 0, 0, 3, 0, 1, 0, 0, 8, 0 },
					{ 9, 0, 0, 8, 6, 3, 0, 0, 5 },
					{ 0, 5, 0, 0, 9, 0, 6, 0, 0 },
					{ 1, 3, 0, 0, 0, 0, 2, 5, 0 },
					{ 0, 0, 0, 0, 0, 0, 0, 7, 4 },
					{ 0, 0, 5, 2, 0, 6, 3, 0, 0 } };

	//sehr schwer
//int g[N][N] = { { 11, 0, 0, 16, 0, 0, 12, 0,   9, 0, 0, 4, 0, 8, 0, 7,},
//				{ 8, 0, 0, 0, 9, 4, 1, 2,   0, 0, 0, 13, 0, 14, 12, 6, },
//				{ 0, 0, 6, 0, 11, 3, 0, 0,   8, 0, 0, 0, 0, 0, 1, 0,},
//				{ 0, 0, 2, 0, 0, 0, 0, 15,   0, 0, 6, 12, 11, 3, 13, 0,},
//
//				{ 0, 0, 16, 0, 14, 12, 6, 0,   0, 0, 0, 0, 0, 0, 7, 0,},
//				{ 0, 7, 15, 8, 0, 0, 2, 9,   0, 11, 0, 0, 14, 0, 0, 0, },
//				{ 12, 6, 0, 0, 0, 0, 0, 0,   0, 8, 15, 7, 4, 1, 0, 0,},
//				{ 1, 0, 0, 4, 5, 0, 0, 8,   0, 14, 10, 0, 3, 0, 0, 11,},
//
//				{ 0, 0, 11, 3, 12, 0, 0, 14,   0, 0, 9, 0, 5, 7, 0, 0,},
//				{ 7, 15, 0, 0, 0, 0, 0, 0,   0, 3, 0, 16, 0, 0, 10, 14, },
//				{ 6, 0, 14, 12, 13, 16, 0, 0,   0, 0, 0, 15, 0, 0, 9, 4,},
//				{ 0, 0, 0, 1, 7, 15, 8, 5,   0, 0, 14, 10, 13, 0, 0, 0,},
//
//				{ 10, 0, 0, 6, 0, 11, 3, 13,   0, 0, 0, 0, 0, 0, 4, 1,},
//				{ 0, 0, 0, 0, 0, 0, 4, 1,   16, 0, 3, 0, 0, 0, 0, 12, },
//				{ 16, 11, 3, 0, 0, 10, 14, 0,   2, 1, 4, 0, 0, 15, 0, 5,},
//				{ 9, 0, 0, 0, 0, 8, 0, 0,   0, 0, 0, 0,16, 0, 0, 0,}, };;


//int g[N][N] = { {
//0,15,0,1,0,2,10,14,12,0,0,0,0,0,0,0}
//,{0,6,3,16,12,0,8,4,14,15,1,0,2,0,0,0}
//,{14,0,9,7,11,3,15,0,0,0,0,0,0,0,0,0}
//,{4,13,2,12,0,0,0,0,6,0,0,0,0,15,0,0}
//,{0,0,0,0,14,1,11,7,3,5,10,0,0,8,0,12}
//,{3,16,0,0,2,4,0,0,0,14,7,13,0,0,5,15}
//,{11,0,5,0,0,0,0,0,0,9,4,0,0,6,0,0}
//,{0,0,0,0,13,0,16,5,15,0,0,12,0,0,0,0}
//,{0,0,0,0,9,0,1,12,0,8,3,10,11,0,15,0}
//,{2,12,0,11,0,0,14,3,5,4,0,0,0,0,9,0}
//,{6,3,0,4,0,0,13,0,0,11,9,1,0,12,16,2}
//,{0,0,10,9,0,0,0,0,0,0,12,0,8,0,6,7}
//,{12,8,0,0,16,0,0,10,0,13,0,0,0,5,0,0}
//,{5,0,0,0,3,0,4,6,0,1,15,0,0,0,0,0}
//,{0,9,1,6,0,14,0,11,0,0,2,0,0,0,10,8}
//,{0,14,0,0,0,13,9,0,4,12,11,8,0,0,2,0,} };;

//int checkSolution(int recGrid[N][N]) {
//	int r = 1;
//	for (int i = 0; i < N; i++)
//	{
//		for (int j = 0; j < N; j++)
//		{
//			if (recGrid[i][j] != l[i][j]){
//				r=0;
//				printf("aua\n");
//				}
//
//		}
//	}
//	return r;
//}


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

/*grids* add_grid(grids* gridList, struct grid grid) {
	grids* newGrids = create_grid(grid);
	if (NULL != newGrids) {
		newGrids->next = gridList;
	}
	return newGrids;
}*/

void insert_end(grids* gridList, struct grid grid)
{
    grids *ptr;
	ptr = gridList;
	if (ptr->start != 1)
	{
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
		}
		ptr->next = create_grid(grid, 0);
	}
	//overwrite root Grid
	else
	{
		ptr->start = 0;
		ptr->grid = grid;
	}
}

void delete_grids(grids* oldGrids) {
	if (NULL != oldGrids->next) {
		delete_grids(oldGrids->next);
	}
	free(oldGrids);
}

/* A utility function to print grid */
void print(int arr[N][N])
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			printf("%d ", arr[i][j]);
		printf("\n");
	}
}

// Checks whether it will be legal
// to assign num to the
// given row, col
int isSafe(struct grid g, int row,
	int col, int num)
{

	// Check if we find the same num
	// in the similar row , we return 0
	for (int x = 0; x <= N - 1; x++)
		if (g.sudoku[row][x] == num)
			return 0;

	// Check if we find the same num in the
	// similar column , we return 0
	for (int x = 0; x <= N - 1; x++)
		if (g.sudoku[x][col] == num)
			return 0;

	// Check if we find the same num in the
	// particular 3*3 matrix, we return 0
	int startRow = row - row % (int)sqrt(N),
		startCol = col - col % (int)sqrt(N);

	for (int i = 0; i < (int)sqrt(N); i++)
		for (int j = 0; j < (int)sqrt(N); j++)
			if (g.sudoku[i + startRow][j +
				startCol] == num)
				return 0;

	return 1;
}

/* Takes a partially filled-in grid and attempts
to assign values to all unassigned locations in
such a way to meet the requirements for
Sudoku solution (non-duplication across rows,
columns, and boxes) 
Return 1 when Solution exist, 0 when no Solution*/
int solveSudoku2(struct grid *grid, int row, int col, grids* gridList, int* gridCount)
{
	// Check if we have reached the 8th row
	// and 9th column (0
	// indexed matrix) , we are
	// returning true to avoid
	// further backtracking
	if (row == N - 1 && col == N)
		return 1;

	// Check if column value becomes 9 ,
	// we move to next row and
	// column start from 0
	if (col == N)
	{
		row++;
		col = 0;
	}

	// Check if the current position
	// of the grid already contains
	// value >0, we iterate for next column
	if (grid->sudoku[row][col] > 0)
		return solveSudoku2(grid, row, col + 1, gridList, gridCount);

	for (int num = 1; num <= N; num++)
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
			grid->sudoku[row][col] = num;
			//printBoard(grid);
			// Cache grid if starting Grid needed and return to deny backtracking
			if (gridList != NULL && &gridCount != NULL)
				//if (gridList != NULL)
			{
				// Add grid to list
				insert_end(gridList, *grid);
				// Set Counter
				*gridCount += 1;
				continue;
			}

			// Checking for next possibility with next
			// column
			if (solveSudoku2(grid, row, col + 1, NULL, gridCount) == 1)


				return 1;

		}

		// Removing the assigned num ,
		// since our assumption
		// was wrong , and we go for next
		// assumption with
		// diff num value
		grid->sudoku[row][col] = 0;
	}
	return 0;
}

void printBoard(struct grid* grid)
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			printf(" %d", grid->sudoku[i][j]);
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

// ToDo fehlerzüstände abfangen wenn zu viele Prozesse für das Sudokufeld gewählt wurden
// Eventuell Stack besser als ne linked List -> schneller
grids* initParallel(int processCount, int* listCount)
{
	

	struct grid rootGrid;

	memcpy(rootGrid.sudoku, g, sizeof(g));

	//printBoard(&rootGrid);

	// init Grid List with starting Sudoku Grid
	grids* gridList = create_grid(rootGrid, 0);
	*listCount = 1;

	int currentDepth = -1; // counter val for current depth of breadth search

	// breadth search till breadth = num or breadth < num
	while (*listCount < processCount)
	{
		// tmp variables for one breadth graph iteration in a depth
		int tmpListCount = 0;
		grids* tmpGridList = create_grid(rootGrid, 1);
		grids* currentGrid = gridList; // pointer to first Grid Element
		// Iterate one graph depth
		while (currentGrid != NULL)
		{
			solveSudoku2(&(currentGrid->grid), 0, 0, tmpGridList, &tmpListCount);
			currentGrid = currentGrid->next;
		}

		/*grids* test = tmpGridList->next;
		grids* test2 = test->next;*/

		delete_grids(gridList);
		gridList = tmpGridList;
		*listCount = tmpListCount;
		currentDepth++;
		//if (currentDepth == 500) break; // for debugging
	}
	// delete root sudoku
	/*if (processCount > 1)
	{
		grids* ptr;
		ptr = gridList;
		printBoard(&ptr->grid);
		gridList = gridList->next;
		free(ptr);
	}*/
	// Print found starting Sudokus and further information			
	//printf("Found %d Sudokus to parallelize", *listCount);
	//printf("\n");
	//printPartSudokus(gridList);
	return gridList;

	// free memory
	//delete_grids(p);
}

/*
int main()
{
	struct grid startingSudoku;
	int g[N][N] = {{ 3, 0, 6, 5, 0, 8, 4, 0, 0 },
				   { 5, 2, 0, 0, 0, 0, 0, 0, 0 },
			       { 0, 8, 7, 0, 0, 0, 0, 3, 1 },
			       { 0, 0, 3, 0, 1, 0, 0, 8, 0 },
			       { 9, 0, 0, 8, 6, 3, 0, 0, 5 },
			       { 0, 5, 0, 0, 9, 0, 6, 0, 0 },
			       { 1, 3, 0, 0, 0, 0, 2, 5, 0 },
			       { 0, 0, 0, 0, 0, 0, 0, 7, 4 },
			       { 0, 0, 5, 2, 0, 6, 3, 0, 0 } };
	initParallel(g);
	
	memcpy(startingSudoku.sudoku, g, sizeof(g));

	//printBoard(&startingSudoku);
		
	int s;
	grids *p = getStartSudokus(startingSudoku,83, &s);

	// do something with grids
	grids* b = p;
	while (b != NULL)
	{
		printBoard(&b->grid);
		b = b->next;
	}
	// free memory
	delete_grids(p);
	






	//if (solveSudoku(grid, 0, 0) == 1)
	//	print(grid);
	//else
	//	printf("No solution exists");

	//return 0;

}
*/