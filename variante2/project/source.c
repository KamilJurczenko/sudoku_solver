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

void fillZeros(int recGrid[N][N]) {
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			recGrid[i][j] = 0;
		}

	}
}

int main(int argc, char** argv) {
	int process_count;
	int rank; // rank var to identify procs

	int recGrid[N][N];
	int solution[N][N];
	fillZeros(recGrid);
	fillZeros(solution);

	double start_time;
	double end_time;
	double start_time_teilsudokus;
	double end_time_teilsudokus;
	double duration;

	int possibleSolutionsCount;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//printf("#procs %d, pid %d\n", process_count, rank);

	//mindest anzahl an teilsudokus
	//int teilsudokus = atoi(argv[1]);

	start_time = MPI_Wtime();
	start_time_teilsudokus = MPI_Wtime();
	// Seriell Anfangsudokus per Breitensuche abspeichern, um später damit zu parallelisieren
	//grids* sudokuList = initParallel(teilsudokus, &possibleSolutionsCount);
	grids* sudokuList = initParallel(process_count, &possibleSolutionsCount);
	//printPartSudokus(sudokuList);
	end_time_teilsudokus = MPI_Wtime();

	if (rank == 0) {
		duration = end_time_teilsudokus - start_time_teilsudokus;

	}
	//printf("teilsodukos %d", possibleSolutionsCount);

	if (process_count == 0)
		return 0;

	// Weise jedem prozess ein Startsudoku zu
	int solutionFound = 0;
	int xrank = rank;
	int count = 60;
	int rcount = 0;




	for (int i = 0; i < possibleSolutionsCount; i++) {
		//printf("Solution ...#procs %d, pid %d\n", process_count, rank);




		if (xrank == i) {
			//printf("Solution xrank == %i ...\n", i);
			xrank += process_count;
			solutionFound = solveSudoku2(&sudokuList->grid, 0, 0, NULL, NULL);

		}

		if (solutionFound == 1) {


			for (int i = 0; i < N; i++)
			{
				for (int j = 0; j < N; j++)
				{
					recGrid[i][j] = sudokuList->grid.sudoku[i][j];
				}
				;
			}
			if (process_count > 1 & rank != 0) {
				double end_time2 = MPI_Wtime();
				duration = end_time2 - start_time;
				//printf("  AAAAA procs %d, pid %d Lösunngs Duration: %f\n", process_count, rank, duration);
				//if (checkSolution(recGrid) != 1) {
				//	duration = 0;
				//}
				MPI_Send(&duration, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
			}

			//printf("\n");
			break;

		}



		if (sudokuList->next != NULL)
		{
			//printBoard(&sudokuList->grid);
			sudokuList = sudokuList->next;
		}
		else {
			break;
		}





	}

	if (sudokuList != NULL)
	{
		delete_grids(sudokuList);
		//printf("ERROR: Not all Sudokus checked!\n");
	}



	double solutionTime = 0;
	if (rank == 0 & process_count > 1 & solutionFound != 1) {
		//MPI_Recv(&solution, N* N, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&solutionTime, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	}
	else {
		double end_time2 = MPI_Wtime();
		solutionTime = end_time2 - start_time;
	}
	//printf("#procs %d, pid %d ->reduce\n", process_count, rank);
	MPI_Reduce(&recGrid, &solution, N * N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	end_time = MPI_Wtime();



	if (rank == 0) {

		duration = end_time - start_time;
		//printf("\\\\     //\n");
		//printf(" \\\\   //\n");
		//printf("  \\\\_// procs %d, pid %d Duration: %f\n", process_count, rank, duration);
		printf("%d,%d,%f,%f\n", process_count, possibleSolutionsCount, solutionTime, duration);
		//for (int i = 0; i < N; i++)
		//{
		//	for (int j = 0; j < N; j++)
		//	{
		//		printf(" %d", solution[i][j]);
		//	}
		//	printf("\n");
		//}
		//printf("\n");
	}

	MPI_Finalize();




	return 0;
}


// EOF