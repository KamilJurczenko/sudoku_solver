#include <omp.h>
#include "sudoku_seriell.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

int solution_found = 0;

// sucht eine Lösung für das übergebene Sudoku Feld
void lookForSolution(grids* sudoku, int rank)
{
	if (solution_found)
		return;

	if (solveSudoku(&sudoku->grid, 0, 0, NULL, NULL) == 1)
	{
		solution_found = 1;
		D(printf("Solution found on tid %d \n", rank));
		// Lösung ausgeben
		D(printBoard(&sudoku->grid));
		// ToDo OPTIMIERUNG: restlichen Prozesse vorzeitig beenden
	}
	/*
	else
	{
		printf("Thread %d has no Solution for its Grid\n", rank);
	}
	*/
}

int main(int argc, char** argv) {
	int thread_num;

	double start_time;
	double end_time;
	double duration;

	thread_num = atoi(argv[1]);
	char* file = argv[2];
	omp_set_num_threads(thread_num);
	start_time = omp_get_wtime();
	char sudokus[] = "sudokuNormal9, sudokuNormal16, sudokuHard16, sudokuExtreme25, sudoku100";
	D(printf("Available Sudokus: %s\n", sudokus));
	// CODE START

	if (thread_num == 0)
		return 0;

	grids* sudokuList = NULL;
	grids* ptr = NULL;
	int sudokuListSize = 0;
	int i = 1;

#pragma omp parallel
{
		// Erster Thread erstellt Sudokus
#pragma omp single
		{
			sudokuList = initParallel(thread_num, &sudokuListSize, file); // Jeder Thread wird rechnen!
			ptr = sudokuList;
			//lookForSolution(omp_get_thread_num());
		}
#pragma omp for
		for (i = 0; i < sudokuListSize; i++)
		{
			//printf("%d\n", omp_get_thread_num());
			int j = i;
			int jj = 0;
			grids* tmpPtr = ptr;
			while (jj < j) {
				tmpPtr = tmpPtr->next;
				jj++;
			}
			lookForSolution(tmpPtr, omp_get_thread_num());
		}
}

	delete_grids(sudokuList);
	D(printf("No Sudokus left! The End...\n"));

	// CODE END

	end_time = omp_get_wtime();

	duration = end_time - start_time;
	D(printf("\\\\     //\n"));
	D(printf(" \\\\   //\n"));
	D(printf("  \\\\_// Duration: %f\n", duration));
	printf("%f\n", duration);

	return 0;
}
