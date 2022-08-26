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
#include <Windows.h>

#define MANAGER_RECV_GRIDS 11
#define COMPUT_RECV_GRIDS 12
#define COMPUT_STATUS 13
#define MANAGER_STATUS 14
#define SOLUTION_TIME 15
#define MANAGER_IDLING_STATUS 16

#define GRIDS_EMPTY 0
#define GRIDS_FULL 1
#define GRIDS_AVAILABLE 2

double solutionFindTime = 0;
double start_time;
int pid;
int gridWidth;
int gridSize;
//int debug_computedGrids = 0;

int attemptGridCache(struct grid sudokuGrid) {
	// Send Compute Status
	int computeStatus = 1;
	MPI_Send(&computeStatus, 1, MPI_INT, 0, COMPUT_STATUS, MPI_COMM_WORLD);
	// Get Manager Status
	int managerStatus;
	MPI_Recv(&managerStatus, 1, MPI_INT, 0, MANAGER_STATUS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	// Send grids to cache in manager process
	if (managerStatus != GRIDS_FULL)
	{
		MPI_Send(sudokuGrid.sudoku, gridSize, MPI_INT, 0, MANAGER_RECV_GRIDS, MPI_COMM_WORLD);
		return 1;
	}
	return 0;
}
struct grid recvGrid(int srcpid, int tag)
{
	int* recvGrid = (int*)malloc(gridSize * sizeof(int));
	// Warte blockierend auf ein Sudoku welches gelöst werden soll
	MPI_Recv(recvGrid, gridSize, MPI_INT, srcpid, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	struct grid g;
	g.sudoku = recvGrid;
	g.size = gridWidth;
	return g;
}
// sucht eine Lösung für das übergebene Sudoku Feld
void computeGrid(struct grid grid, int cacheGrids)
{
	//printBoard(&grid);
	if (solveSudoku(&grid, 0, 0, NULL, NULL, cacheGrids) == 1)
	{
		printf("<<<<<<<<<<<<<<<<<Solution found on pid %d >>>>>>>>>>>>>>>>>>>>>\n", pid);
		//fflush(stdout);
		// Lösung ausgeben
		//printBoard(&grid);

		double end_time = MPI_Wtime();
		solutionFindTime = end_time - start_time;
	}
	//debug_computedGrids++;
	free(grid.sudoku);
	/*
	else
	{
		//printf("process %d has no Solution for its Grid\n", rank);
	}
	*/

}

int main(int argc, char** argv) {
	int process_count;

	double end_time;
	double duration;
	double initDuration;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	char* file = argv[1];
	//process_count = 3;
	//char* file = "..\\..\\..\\sudokus\\sudokuNormal9.txt"; // used for debugging
	char sudokus[] = "sudokuNormal9, sudokuNormal16, sudokuHard16, sudokuExtreme25, sudoku100";
	D(printf("Available Sudokus: %s\n", sudokus)); 
	start_time = MPI_Wtime();

	/*if (pid == 0)
	{
		int t;
		scanf("%i", &t);
	}

	MPI_Barrier(MPI_COMM_WORLD);*/
	// CODE START

	// Prozesse die keine lösung finden bekommen vom Prozess 0 ein neues Sudoku zum Abarbeiten
	// Prozess 0 als "Manager"

	if (process_count == 0)
		return 0;

	// For process 0
	grids_queue* manager_gridQueue = NULL;
	int sudokuListSize = 0;

	// Manager Process Initialization
	if (pid == 0)
	{
		//int debugProcessNumbers = 2;
		// Seriell Anfangsudokus per Breitensuche suchen und abspeichern, um später damit zu parallelisieren
		double startInit_time = MPI_Wtime();
		manager_gridQueue = initParallel(process_count - 1, &sudokuListSize, file);
		//printPartSudokus(manager_gridQueue);
		initDuration = MPI_Wtime() - startInit_time;
		gridWidth = manager_gridQueue->item.size;
	}
	// Broadcast grid size
	MPI_Bcast(&gridWidth, 1, MPI_INT, 0, MPI_COMM_WORLD);
	gridSize = gridWidth * gridWidth;
	//MPI_Barrier(MPI_COMM_WORLD);
	if(pid == 0 && process_count != 1)
	{
		for (int i = 1; i < process_count; i++)
		{
			//printf("Sending grids to other processes...\n");
			// Sudoku an jeden Prozess senden
			struct grid g = dequeue(&manager_gridQueue);
			MPI_Send(g.sudoku, gridSize, MPI_INT, i, COMPUT_RECV_GRIDS, MPI_COMM_WORLD);
			free(g.sudoku);
			sudokuListSize--;
		}
		// Sudokulist dynamisch auffüllen und, bei Anfrage, abschicken
		int idlingProcessesCount = 0;
		int computationFinished = 0;
		processes_queue* idlingPid_queue = NULL;
		do {
			// Statusabfrage
			int msg;
			MPI_Status status;
			MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, COMPUT_STATUS, MPI_COMM_WORLD, &status);
			// Status senden
			int reply;
			if (sudokuListSize <= 0)
			{
				reply = GRIDS_EMPTY;
				//printf("GRIDS EMPTY!\n");
				//fflush(stdout);
			}
			else if (sudokuListSize < (process_count - 1))
				reply = GRIDS_AVAILABLE;
			else {
				reply = GRIDS_FULL;
				//Sleep(1000);
				//printf("GRIDS FULL!\n");
				//fflush(stdout);
			}

			MPI_Send(&reply, 1, MPI_INT, status.MPI_SOURCE, MANAGER_STATUS, MPI_COMM_WORLD);

			// Erhalte Antwort
			// Grid zum Erhalt verfügbar 
			if (msg == 1 && reply != GRIDS_FULL) {
				//printBoard(&manager_gridQueue->item);
				enqueue(&manager_gridQueue, recvGrid(status.MPI_SOURCE, MANAGER_RECV_GRIDS));
				// Check for idling pids
				int pid = intDequeue(&idlingPid_queue);
				if (pid != -1)
				{
					struct grid g = dequeue(&manager_gridQueue);
					//printBoard(&g);
					int hasGrid = 1;
					MPI_Send(&hasGrid, 1, MPI_INT, pid, MANAGER_IDLING_STATUS, MPI_COMM_WORLD);
					MPI_Send(g.sudoku, gridSize, MPI_INT, pid, COMPUT_RECV_GRIDS, MPI_COMM_WORLD);
					free(g.sudoku);
					idlingProcessesCount--;
				}
				else sudokuListSize++;
			}
			// Prozess fertig. Idling...
			else if (msg == 0) 
			{
				//printf("IDLING\n");
				//fflush(stdout);
				if (reply != GRIDS_EMPTY) {
					// Schicke neue Grids/Work aus worklist/sudokulist falls verfügbar
					struct grid g = dequeue(&manager_gridQueue);
					MPI_Send(g.sudoku, gridSize, MPI_INT, status.MPI_SOURCE, COMPUT_RECV_GRIDS, MPI_COMM_WORLD);
					free(g.sudoku);
					sudokuListSize--;
				}
				// No Grids Available wait for more processes to check for completion
				else {
					idlingProcessesCount++;
					intEnqueue(&idlingPid_queue, status.MPI_SOURCE);
					// All processes are idling and no more work left.
					// end mpi program
					if (idlingProcessesCount == (process_count - 1)) 
					{
						computationFinished = 1;
						int pid;
						int noWorkMsg = 0;
						while ((pid = intDequeue(&idlingPid_queue)) > 0)
							MPI_Send(&noWorkMsg, 1, MPI_INT, pid, MANAGER_IDLING_STATUS, MPI_COMM_WORLD);
					}
				}
			}
		} while (computationFinished == 0);
		if (solutionFindTime == 0)
			MPI_Recv(&solutionFindTime, 1, MPI_DOUBLE, MPI_ANY_SOURCE, SOLUTION_TIME, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		D(printf("No Sudokus left! The End...\n"));
	}
	else if(pid != 0)
	{
		// init sudokus first
		computeGrid(recvGrid(0, COMPUT_RECV_GRIDS), 1);
		// sudokus from dynamic queue
		int managerStatus;
		int manager_idlingStatus = 1;

		do {
			// ask for new work
			int computeStatus = 0;
			MPI_Send(&computeStatus, 1, MPI_INT, 0, COMPUT_STATUS, MPI_COMM_WORLD);
			// Get Manager Status
			MPI_Recv(&managerStatus, 1, MPI_INT, 0, MANAGER_STATUS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (managerStatus != GRIDS_EMPTY) {
				//struct grid g = recvGrid(0, COMPUT_RECV_GRIDS);
				//printBoard(&g);
				computeGrid(recvGrid(0, COMPUT_RECV_GRIDS), 1);
			}
			// Wait for idling process status from manager
			else 
			{
				MPI_Recv(&manager_idlingStatus, 1, MPI_INT, 0, MANAGER_IDLING_STATUS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				if (manager_idlingStatus == 1)
					computeGrid(recvGrid(0, COMPUT_RECV_GRIDS), 1);
			}
			//Sleep(100);
		} while (manager_idlingStatus == 1);
		if (pid != 0)
			MPI_Send(&solutionFindTime, 1, MPI_DOUBLE, 0, SOLUTION_TIME, MPI_COMM_WORLD);
		//printf("pid %i idling...", pid);
		//fflush(stdout);
	}
	else if (pid == 0 && process_count == 1) {
		computeGrid(dequeue(&manager_gridQueue), 0);
	}

	// CODE END

	if (pid == 0) {
		end_time = MPI_Wtime();
		duration = end_time - start_time;
		D(printf("\\\\     //\n"));
		D(printf(" \\\\   //\n"));
		D(printf("  \\\\_// Duration: %f\n", duration));
		printf("%f\n", duration);
		printf("%f\n", solutionFindTime);
		printf("%f\n", initDuration);
	}
	//MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}

// EOF