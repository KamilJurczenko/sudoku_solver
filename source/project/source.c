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
		//printf("process %d has no Solution for its Grid\n", rank);
	}
}

struct grid recvGrid(int gridSize)
{
	MPI_Status status;
	size_t size = gridSize * gridSize * sizeof(int);
	int* recvGrid = malloc(size);
	// Warte blockierend auf ein Sudoku welches gelöst werden soll
	MPI_Recv(recvGrid, gridSize*gridSize, MPI_INT, 0, 8, MPI_COMM_WORLD, &status); // ToDo OPTIMIERUNG: vielleicht wäre hier ein MPI Datatype fürs grid struct besser
	struct grid g;
	g.sudoku = recvGrid;
	g.size = gridSize;
	//printf("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\n");
	//printBoard(&g);
	return g;
}

void recvAndCalc(int size, int rank) {
	struct grid receivedGrid = recvGrid(size);
	lookForSolution(receivedGrid, rank);
	free(receivedGrid.sudoku);
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

	int gridSize = 0;
	// For process 0
	grids* ptr = NULL;
	MPI_Request* sendRequests = NULL;
	MPI_Request* recvRequests = NULL;
	grids* sudokuList = NULL;

	if (rank == 0)
	{
		//int debugProcessNumbers = 2;
		// Seriell Anfangsudokus per Breitensuche suchen und abspeichern, um später damit zu parallelisieren
		sudokuList = initParallel(process_count);
	    gridSize = sudokuList->grid.size;

		int* data;
		data = (int*)malloc((process_count - 1) * sizeof(int));
	    recvRequests = (MPI_Request*)malloc((process_count - 1) * sizeof(MPI_Request));
		sendRequests = (MPI_Request*)malloc((process_count - 1) * sizeof(MPI_Request));
		ptr = sudokuList;

		// 60% COMPUTING TIME START
		for (int i = 1; i < process_count; i++)
		{
			//printf("Sending grids to other processes...\n");
			// Sudoku an jeden Prozess senden
			MPI_Isend(ptr->grid.sudoku, gridSize * gridSize, MPI_INT, i, 8, MPI_COMM_WORLD, &sendRequests[i - 1]);
			//MPI_Isend(ptr->grid.size, );
			// Antwort wenn prozess fertig
			MPI_Irecv(&data[i - 1], 1, MPI_INT, i, 2, MPI_COMM_WORLD, &recvRequests[i - 1]);
			ptr = ptr->next;
		}
		// 60% COMPUTING TIME END
		free(data);
	}
	
	// Broadcast grid size
	MPI_Bcast(&gridSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if(rank != 0)
	{
		recvAndCalc(gridSize, rank);
		// Neue Sudokus anfragen falls vorhanden
		int buf = 1;
		int sudokusAvailable = 0;
		// Neue Sudokus anfragen solange welche vorhanden
		do {
			//printf("Asking for new Sudokus...\n");
			MPI_Send(&buf, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
			MPI_Recv(&sudokusAvailable, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (sudokusAvailable == 1)
			{
				printf("New Sudokus Available! Get one...\n");
				recvAndCalc(gridSize, rank);
			}
		} while (sudokusAvailable == 1);
	}
	else if (rank == 0)
	{
		// Nehme selber ein Sudoku 
		if (ptr != NULL)
		{
			lookForSolution(ptr->grid, rank);
			ptr = ptr->next;
		}
		else printf("ERROR: No Sudokus left!\n");
		MPI_Status* sendStatuses = (MPI_Status*)malloc((process_count - 1) * sizeof(MPI_Status));
		MPI_Waitall(process_count - 1, sendRequests, sendStatuses);
		//MPI_Request* sendRequestsNew = (MPI_Request*)malloc((process_count - 1) * sizeof(MPI_Request));
		//MPI_Request* replyReq = (MPI_Request*)malloc((process_count - 1) * sizeof(MPI_Request));
		// Sudokus übrig zum abgeben 
		while (ptr != NULL)
		{
			MPI_Status status;
			int index;
			int reply = 1;
			// Warte auf ein fertigen Prozess
			//printf("Waiting for one process to give new task...\n");
			MPI_Waitany(process_count - 1, recvRequests, &index, &status);
			MPI_Send(&reply, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
			MPI_Send(ptr->grid.sudoku, gridSize * gridSize, MPI_INT, status.MPI_SOURCE, 8, MPI_COMM_WORLD);
			//MPI_Isend(1, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD, &replyReq[status.MPI_SOURCE]);
			//MPI_Isend(ptr->grid.sudoku, gridSize * gridSize, MPI_INT, status.MPI_SOURCE, 8, MPI_COMM_WORLD, &sendRequestsNew[status.MPI_SOURCE]);
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
				//MPI_Request request;
				int reply = 0;
				MPI_Send(&reply, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
				//MPI_Isend(&buf, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &request);
				//MPI_Request_free(&request);
			}
			//MPI_Request_free(recvRequests);
		}
		free(recvRequests);
		free(sendStatuses);
		free(sendRequests);
		delete_grids(sudokuList);

		printf("No Sudokus left! The End...\n");
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