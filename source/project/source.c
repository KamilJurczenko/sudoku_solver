/********************************************************************

Project:

Original authors:
(sorted by lastname)
	Daitche Konstantin
	Jurczenko Kamil

*********************************************************************/

#include <stdio.h>
#include <mpi.h>
#include "sudoku_seriell.h"
#include <stdlib.h>

// sucht eine Lösung für das übergebene Sudoku Feld
void lookForSolution(struct grid grid, int rank)
{
	int solutionFound = solveSudoku(&grid, 0, 0, NULL, NULL);

	if (solutionFound == 1)
	{
		printf("Solution found on pid %d \n", rank);
		printf("Solution Sudoku: \n");
		// Lösung ausgeben
		printBoard(&grid);

		// ToDo OPTIMIERUNG: restlichen Prozesse vorzeitig beenden
	}
	else
	{
		printf("process %d has no Solution for its Grid\n", rank);
	}
}

struct grid recvGrid()
{
	MPI_Status status;
	int receivedGrid[N][N];
	// Warte blockierend auf ein Sudoku welches gelöst werden soll
	MPI_Recv(&receivedGrid, N * N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); // ToDo OPTIMIERUNG: vielleicht wäre hier ein MPI Datatype fürs grid struct besser
	struct grid g;
	memcpy(g.sudoku, receivedGrid, sizeof(receivedGrid));
	return g;
}

int main(int argc, char** argv) {
	int process_count;
	int rank;

	double start_time;
	double end_time;
	double duration;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	start_time = MPI_Wtime();

	// CODE START

	// Prozesse die keine lösung finden bekommen vom Prozess 0 ein neues Sudoku zum Abarbeiten
	// Prozess 0 als "Manager"

	if (process_count == 0)
		return 0;

	if (rank == 0)
	{
		grids* sudokuList;
		//int debugProcessNumbers = 2;

		// Seriell Anfangsudokus per Breitensuche suchen und abspeichern, um später damit zu parallelisieren
		sudokuList = initParallel(process_count);

		int* data;
		data = (int*)malloc((process_count - 1) * sizeof(int));
		MPI_Request* recvRequests = (MPI_Request*)malloc((process_count - 1) * sizeof(MPI_Request));
		MPI_Request* sendRequests = (MPI_Request*)malloc((process_count - 1) * sizeof(MPI_Request));
		grids* ptr;
		ptr = sudokuList;

		// 60% COMPUTING TIME START
		for (int i = 1; i < process_count; i++)
		{
			printf("Sending grids to other processes...\n");
			// Sudoku an jeden Prozess senden
			MPI_Isend(ptr->grid.sudoku, N * N, MPI_INT, i, 0, MPI_COMM_WORLD, &sendRequests[i - 1]);
			// Antwort wenn prozess fertig
			MPI_Irecv(&data[i - 1], 1, MPI_INT, i, 2, MPI_COMM_WORLD, &recvRequests[i - 1]);
			ptr = ptr->next;
		}
		// 60% COMPUTING TIME END
		// 
		// Nehme selber ein Sudoku 
		if (ptr != NULL)
		{
			lookForSolution(ptr->grid, rank);
			ptr = ptr->next;
		}
		else printf("ERROR: No Sudokus left!\n");
		MPI_Status* sendStatuses = (MPI_Status*)malloc((process_count - 1) * sizeof(MPI_Status));
		MPI_Waitall(process_count - 1, sendRequests, sendStatuses);
		MPI_Request* sendRequestsNew = (MPI_Request*)malloc((process_count - 1) * sizeof(MPI_Request));

		// Sudokus übrig zum abgeben 
		while (ptr != NULL)
		{
			MPI_Status status;
			int index;
			// Warte auf ein fertigen Prozess
			printf("Wait for other process to finish...\n");
			MPI_Waitany(process_count - 1, recvRequests, &index, &status);

			//MPI_Isend(ptr->grid.sudoku, N * N, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &sendRequestsNew[status.MPI_SOURCE]);
			ptr = ptr->next;
		}
		// Warte auf restlichen Prozesse
		if (process_count > 1)
		{
			// Auf Antwort restlicher Prozesse warten und mitteilen dass keine Sudokus übrig
			MPI_Status* statuses = (MPI_Status*)malloc((process_count - 1) * sizeof(MPI_Status));
			MPI_Waitall(process_count - 1, recvRequests, statuses);
			for (int i = 1; i < process_count; i++)
			{
				MPI_Request request;
				int buf = 0;
				MPI_Isend(&buf, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
				MPI_Request_free(&request);
			}
			//MPI_Request_free(recvRequests);
		}
		free(recvRequests);
		free(sendStatuses);
		free(sendRequests);
		free(data);
		delete_grids(sudokuList);
		printf("No Sudokus left!\n");
	}
	else
	{
		lookForSolution(recvGrid(), rank);
		// Neue Sudokus anfragen falls vorhanden
		int buf = 1;
		MPI_Send(&buf, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		int sudokusAvailable = 0;
		MPI_Recv(&sudokusAvailable, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (sudokusAvailable == 1)
		{
			lookForSolution(recvGrid(), rank);
		}
	}

	// CODE END

	end_time = MPI_Wtime();

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