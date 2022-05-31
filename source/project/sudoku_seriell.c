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

// N is the size of the 2D matrix N*N
#define N 9

// global variables

int grid[N][N] = { { 3, 0, 6, 5, 0, 8, 4, 0, 0 },
			   { 5, 2, 0, 0, 0, 0, 0, 0, 0 },
			   { 0, 8, 7, 0, 0, 0, 0, 3, 1 },
			   { 0, 0, 3, 0, 1, 0, 0, 8, 0 },
			   { 9, 0, 0, 8, 6, 3, 0, 0, 5 },
			   { 0, 5, 0, 0, 9, 0, 6, 0, 0 },
			   { 1, 3, 0, 0, 0, 0, 2, 5, 0 },
			   { 0, 0, 0, 0, 0, 0, 0, 7, 4 },
			   { 0, 0, 5, 2, 0, 6, 3, 0, 0 } };

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
int isSafe(int grid[N][N], int row,
	int col, int num)
{

	// Check if we find the same num
	// in the similar row , we return 0
	for (int x = 0; x <= N - 1; x++)
		if (grid[row][x] == num)
			return 0;

	// Check if we find the same num in the
	// similar column , we return 0
	for (int x = 0; x <= N - 1; x++)
		if (grid[x][col] == num)
			return 0;

	// Check if we find the same num in the
	// particular 3*3 matrix, we return 0
	int startRow = row - row % (int)sqrt(N),
		startCol = col - col % (int)sqrt(N);

	for (int i = 0; i < (int)sqrt(N); i++)
		for (int j = 0; j < (int)sqrt(N); j++)
			if (grid[i + startRow][j +
				startCol] == num)
				return 0;

	return 1;
}

/* Takes a partially filled-in grid and attempts
to assign values to all unassigned locations in
such a way to meet the requirements for
Sudoku solution (non-duplication across rows,
columns, and boxes) */
int solveSudoku(int grid[N][N], int row, int col, bool returnOnFind, int **grid_ptr)
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
	if (grid[row][col] > 0)
		return solveSudoku(grid, row, col + 1, returnOnFind, grid_ptr);

	if (returnOnFind)
		*grid_ptr = malloc(sizeof(int) * N);

	for (int num = 1; num <= N; num++)
	{
		// Check if it is safe to place
		// the num (1-9) in the
		// given row ,col ->we move to next column
		if (isSafe(grid, row, col, num) == 1)
		{
			/* assigning the num in the
			current (row,col)
			position of the grid
			and assuming our assigned num
			in the position
			is correct	 */
			grid[row][col] = num;

			// Cache grid if starting Grid needed and return to deny backtracking
			if (returnOnFind)
			{
				*grid_ptr[num - 1] = &grid;
				free(grid_ptr);
				continue;
			}

			// Checking for next possibility with next
			// column
			if (solveSudoku(grid, row, col + 1, false, NULL) == 1)
				return 1;
		}

		// Removing the assigned num ,
		// since our assumption
		// was wrong , and we go for next
		// assumption with
		// diff num value
		grid[row][col] = 0;
	}
	return 0;
}

// ToDo Get Array of starting Sudoku Grids
int** getStartSudokus(int num)
{
	//int tmpGrid[N][N] = grid;
	int *tmpGrid_ptr = malloc(sizeof(int));
	tmpGrid_ptr = &grid;
	int tmpGridLength = sizeof(tmpGrid_ptr);
	int depth = -1; // counter val for current depth of breadth search
	int breadth = 1;

	// breadth search till breadth = num or breadth < num
	while (breadth <= num) 
	{
		int *gridArr;
		//tmpGrid_ptr = calloc(500, sizeof(grid));
		for (int i = 0; i < tmpGridLength; i++)
		{
			solveSudoku(*(tmpGrid_ptr + i), 0, 0, true, &gridArr); // ToDo Cache rows,cols for better performance
			for (int j = 0; j < sizeof(gridArr); j++) 
			{
				*(tmpGrid_ptr + j) = *(gridArr + j);
			}
		}
		tmpGridLength = sizeof(tmpGrid_ptr);
		breadth = sizeof(gridArr);
		depth++;
		if (depth == 500) break; // for debugging
	}

	//free(tmpGrid_ptr);
	return tmpGrid_ptr;
}
 
//Abwandlung von solveSudoku.
//Printet alle möglichen Spielfeld Kombination für maxStellen aus.
int teilSudokus(int grid[N][N], int row, int col, int maxStellen, int stelle)
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
	if (grid[row][col] > 0)
		return teilSudokus(grid, row, col + 1, maxStellen, stelle);

	for (int num = 1; num <= N; num++)
	{

		// Check if it is safe to place
		// the num (1-9) in the
		// given row ,col ->we move to next column
		if (isSafe(grid, row, col, num) == 1)
		{
			grid[row][col] = num;
			if (stelle == maxStellen) {
				printf("\n");
				print(grid);
			}


			//Prüfen für das vorzeitiges Abbrechen
			if (stelle < maxStellen) {
				// Checking for next possibility with next
				// column
				if (teilSudokus(grid, row, col + 1, maxStellen, stelle + 1) == 1)
					return 1;
			}


		}
		// Removing the assigned num ,
		// since our assumption
		// was wrong , and we go for next
		// assumption with
		// diff num value
		grid[row][col] = 0;

	}

	return 0;
}

int main()
{
	// 0 means unassigned cells


	//if (solveSudoku(grid, 0, 0) == 1)
	//	print(grid);
	//else
	//	printf("No solution exists");

	//return 0;

	//teilSudokus(grid, 0, 0, 45, 1);

}