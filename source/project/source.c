/********************************************************************

Project:

Original authors:
(sorted by lastname)
	Daitche Konstantin
	Jurczenko Kamil

	Parallelisierung Strategie 1

*********************************************************************/

#include <stdio.h>
#include <mpi.h>
#include "sudoku_seriell.h"

int main(int argc, char** argv) {
	int process_count;
	int rank; // rank var to identify procs

	double start_time;
	double end_time;
	double duration;

	int possibleSolutionsCount;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Seriell Anfangsudokus per Breitensuche abspeichern, um später damit zu parallelisieren
	grids* sudokuList = initParallel(process_count, &possibleSolutionsCount);

	start_time = MPI_Wtime();

	// CODE START

	// ToDo's breche ab wenn ein prozess ne lösung findet und gebe sie aus
	// Prozesse die keine lösung finden sollen den Sudokus zugewiesen werden die noch übrig sind (überprüfung ob sudokuList != NULL Pointer)

	if (process_count == 0)
		return 0;

	// Weise jedem prozess ein Startsudoku zu
	int solutionFound = solveSudoku(sudokuList->grid,0,0,NULL,NULL);

	// ToDo MPI_Send aktualisierte sudoku Liste zum nächsten Prozess

	// Lösung gefunden?
	if (solutionFound == 1) 
	{
		// Lösung ausgeben
		printBoard(&sudokuList->grid);

		// ToDo restlichen Prozesse vorzeitig beenden
	}
	/*else
	{
		// free erstes Element der Liste
		grids* ptr;
		ptr = sudokuList;
		sudokuList = sudokuList->next;
		free(ptr);
	}*/

	// CODE END

	end_time = MPI_Wtime();

	if (sudokuList != NULL) 
	{
		delete_grids(sudokuList);
		printf("ERROR: Not all Sudokus checked!\n");
	}


	if (rank == 0) {
		duration = end_time - start_time;
		printf("\\\\     //\n");
		printf(" \\\\   //\n");
		printf("  \\\\_// Duration: %f\n", duration);
	}

	MPI_Finalize();
	return 0;
}

// EOF