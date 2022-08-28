#include <stdio.h>
#include <mpi.h>
#include "sudoku_seriell.h"
#include <stdlib.h>
#include <Windows.h>
#include <math.h>
#include <direct.h>

#define MANAGER_RECV_GRIDS 11
#define COMPUT_RECV_GRIDS 12
#define COMPUT_STATUS 13
#define MANAGER_STATUS 14
#define SOLUTION_TIME 15
#define MANAGER_IDLING_STATUS 16

#define GRIDS_EMPTY 0
#define GRIDS_FULL 1
#define GRIDS_AVAILABLE 2
#define SOLUTION_FOUND 55

double solutionFindTime = 0;
double start_time;
int pid;
int gridWidth;
int gridSize;
int solutionFound;

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
}

void delete_grids(grids_queue* oldGrids) {
	if (dequeue(&oldGrids).size != NULL)
		delete_grids(oldGrids);
}

int isSafe(struct grid g, int row,int col, int num)
{
	for (int x = 0; x <= g.size - 1; x++)
		if (g.sudoku[row * g.size + x] == num)
			return 0;

	for (int x = 0; x <= g.size - 1; x++)
		if (g.sudoku[x * g.size + col] == num)
			return 0;

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

int getManagerStatus() {
	// Send Compute Status
	int computeStatus = 1;
	MPI_Send(&computeStatus, 1, MPI_INT, 0, COMPUT_STATUS, MPI_COMM_WORLD);
	// Get Manager Status
	int managerStatus;
	MPI_Recv(&managerStatus, 1, MPI_INT, 0, MANAGER_STATUS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	return managerStatus;
}

// Caches first grid without doing a Breadth search first
int solveSudokuNoBreadthSearch(struct grid* grid, int row, int col, grids_queue** gridQueue, int* gridCount)
{
	int index = row * grid->size + col;

	if (row == grid->size - 1 && col == grid->size)
		return 1;

	if (col == grid->size)
	{
		row++;
		col = 0;
	}

	if (grid->sudoku[index] > 0)
		return solveSudoku(grid, row, col + 1, gridQueue, gridCount);

	// First for loop to check for multiple next possiblities to attempt to cache one
	for (int num = 1; num <= grid->size; num++)
	{
		if (isSafe(*grid, row, col, num) == 1)
		{

			grid->sudoku[index] = num;
			if (solutionFound == 1)
				break;

			// Cache grid if starting Grid needed and return to deny backtracking
			if (gridQueue != NULL && &gridCount != NULL)
			{
				// Add grid to list
				enqueue_cpy(gridQueue, *grid);
				// Set Counter
				*gridCount += 1;
			}
			else
			{
				// attempt cache grid
				if (pid != 0)
				{
					// Get manager status
					int managerStatus = getManagerStatus();
					// Cancel computation
					if (managerStatus == SOLUTION_FOUND)
					{
						solutionFound = 1;
						break;
					}
					else if (managerStatus != GRIDS_FULL)
					{
						// Send current Grid
						MPI_Send(grid->sudoku, gridSize, MPI_INT, 0, MANAGER_RECV_GRIDS, MPI_COMM_WORLD);
					}
					else if (solveSudoku(grid, row, col + 1, NULL, NULL) == 1)
						return 1;
				}
				else if (solveSudoku(grid, row, col + 1, NULL, NULL) == 1)
					return 1;
			}

		}
	}

	grid->sudoku[index] = 0;
	return 0;
}


int solveSudoku(struct grid* grid, int row, int col, grids_queue** gridQueue, int* gridCount)
{
	int index = row * grid->size + col;

	if (row == grid->size - 1 && col == grid->size)
		return 1;

	if (col == grid->size)
	{
		row++;
		col = 0;
	}

	if (grid->sudoku[index] > 0)
		return solveSudoku(grid, row, col + 1, gridQueue, gridCount);

	num_queue* numsQueue = NULL;
	int numsQueueSize = 0;
	// First for loop to check for multiple next possiblities to attempt to cache one
	// for (int num = grid->size; num !=0; num--) //iterate num backwards
	for (int num = 1; num <= grid->size; num++)
	{
		if (isSafe(*grid, row, col, num) == 1)
		{
			intEnqueue(&numsQueue, num);
			numsQueueSize++;
		}
	}
	int gridIsCached = 0;
	int num;
	while ((num = intDequeue(&numsQueue, 1)) > 0)
	{
		grid->sudoku[index] = num;
		if (solutionFound == 1)
			break;

		// Cache grid if starting Grid needed and return to deny backtracking
		if (gridQueue != NULL && &gridCount != NULL)
		{
			// Add grid to list
			enqueue_cpy(gridQueue, *grid);
			// Set Counter
			*gridCount += 1;
		}
		else
		{
			// cache grid
			if (gridIsCached == 0 && numsQueueSize > 1 && pid != 0)
			{
				// Get manager status
				int managerStatus = getManagerStatus();
				// Cancel computation
				if (managerStatus == SOLUTION_FOUND)
				{
					solutionFound = 1;
					break;
				}
				else if (managerStatus != GRIDS_FULL)
				{
					// Send current Grid
					MPI_Send(grid->sudoku, gridSize, MPI_INT, 0, MANAGER_RECV_GRIDS, MPI_COMM_WORLD);
					gridIsCached = 1;
				}
				else if (solveSudoku(grid, row, col + 1, NULL, NULL) == 1)
					return 1;
			}
			else if (solveSudoku(grid, row, col + 1, NULL, NULL) == 1)
				return 1;
		}
		numsQueueSize--;
	}
	grid->sudoku[index] = 0;
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

grids_queue* initParallel(int processCount, int* sudokusCount, char* sudokuFile)
{
	struct grid rootGrid;
	int gridSize;

	rootGrid = readGridFromFile(sudokuFile);

	//printf("Starting Sudoku: \n");
	//printBoard(&rootGrid);

	// init Grid Queue with starting Sudoku Grid
	grids_queue* starting_gridQueue = NULL;
	enqueue(&starting_gridQueue, rootGrid);
	*sudokusCount = 1;

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
			solveSudoku(&mGrid, 0, 0, &tmp_gridQueue, &tmpListCount, 0);
			ptr = starting_gridQueue;
			//printBoard(&tmpGridList->grid);
		}

		delete_grids(starting_gridQueue);
		starting_gridQueue = tmp_gridQueue;
		*sudokusCount = tmpListCount;
	}
	// Print found starting Sudokus and further information			
	D(printf("Found %d Sudokus to parallelize\n", *sudokusCount));
	printf("%d\n", *sudokusCount);
	//printPartSudokus(starting_gridQueue); // for debugging
	return starting_gridQueue;
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
		//printf("<<<<<<<<<<<<<<<<<Solution found on pid %d >>>>>>>>>>>>>>>>>>>>>\n", pid);
		//fflush(stdout);
		// Lösung ausgeben
		//printBoard(&grid);
		double end_time = MPI_Wtime();
		solutionFindTime = end_time - start_time;
	    solutionFound = 1;
		int solutionMsg = SOLUTION_FOUND;
		if (pid != 0)
		{
			MPI_Send(&solutionMsg, 1, MPI_INT, 0, COMPUT_STATUS, MPI_COMM_WORLD);
			MPI_Send(&solutionFindTime, 1, MPI_DOUBLE, 0, SOLUTION_TIME, MPI_COMM_WORLD);
		}
		return;
		// Cancel Program
	}
	free(grid.sudoku);
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
	start_time = MPI_Wtime();

	// DEBUGGING
	
	/*
	if (pid == 0)
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
	solutionFound = 0;

	// Manager Process Initialization
	if (pid == 0)
	{
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
	if(pid == 0 && process_count != 1)
	{
		for (int i = 1; i < process_count; i++)
		{
			// Sudoku an jeden Prozess senden
			struct grid g = dequeue(&manager_gridQueue);
			MPI_Send(g.sudoku, gridSize, MPI_INT, i, COMPUT_RECV_GRIDS, MPI_COMM_WORLD);
			free(g.sudoku);
			sudokuListSize--;
		}
		// Sudokulist dynamisch auffüllen und, bei Anfrage, abschicken
		int idlingProcessesCount = 0;
		int computationFinished = 0;
		int solutionReplyCount = 0;
		num_queue* idlingPid_queue = NULL;
		do {
			// Statusabfrage
			int msg;
			MPI_Status status;
			MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, COMPUT_STATUS, MPI_COMM_WORLD, &status);

			// Solution found!
			if (msg == SOLUTION_FOUND)
			{
				solutionFound = 1;
				int solutionMsg = SOLUTION_FOUND;
				MPI_Recv(&solutionFindTime, 1, MPI_DOUBLE, MPI_ANY_SOURCE, SOLUTION_TIME, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				solutionReplyCount++;
				// Manage Idling processes
				int q_pid;
				while ((q_pid = intDequeue(&idlingPid_queue, 1)) > 0)
				{
					solutionReplyCount++;
					MPI_Send(&solutionMsg, 1, MPI_INT, q_pid, MANAGER_IDLING_STATUS, MPI_COMM_WORLD);
				}
				if (solutionReplyCount == (process_count - 1))
				{
					computationFinished = 1;
					// PID 0 finished
					break;
				}
				else if ((process_count - 2) > 0)
					continue;
				else
					break;
			}

			// Status senden
			int reply;
			if (solutionFound == 1)
				reply = SOLUTION_FOUND;
			else if (sudokuListSize <= 0)
				reply = GRIDS_EMPTY;
			else if (sudokuListSize < (process_count - 1))
				reply = GRIDS_AVAILABLE;
			else
				reply = GRIDS_FULL;

			MPI_Send(&reply, 1, MPI_INT, status.MPI_SOURCE, MANAGER_STATUS, MPI_COMM_WORLD);
			if (reply == SOLUTION_FOUND)
			{
				//printf("FINISHED\n");
				//fflush(stdout);
				solutionReplyCount++;
				if (solutionReplyCount == (process_count - 1))
				{
					computationFinished = 1;
					// PID 0 finished
					break;
				}
			}
			// Erhalte Antwort
			// Grid zum Erhalt verfügbar 
			else if (msg == 1 && reply != GRIDS_FULL) {
				//printBoard(&manager_gridQueue->item);
				enqueue(&manager_gridQueue, recvGrid(status.MPI_SOURCE, MANAGER_RECV_GRIDS));
				// Check for idling pids
				int pid = intDequeue(&idlingPid_queue, 1);
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
					// All processes are idling and no more work left. -> no solution found
					// end mpi program
					if (idlingProcessesCount == (process_count - 1)) 
					{
						computationFinished = 1;
						int pid;
						int noWorkMsg = 0;
						while ((pid = intDequeue(&idlingPid_queue, 1)) > 0)
							MPI_Send(&noWorkMsg, 1, MPI_INT, pid, MANAGER_IDLING_STATUS, MPI_COMM_WORLD);
					}
				}
			}
		} while (computationFinished == 0);

		D(printf("No Sudokus left! The End...\n"));
	}
	else if(pid != 0)
	{
		// init sudokus first
		computeGrid(recvGrid(0, COMPUT_RECV_GRIDS), 1);
		// sudokus from dynamic queue
		int manager_idlingStatus = 1;
		if (solutionFound == 0)
		{
			do {
				// ask for new work
				int computeStatus = 0;
				int managerStatus;
				MPI_Send(&computeStatus, 1, MPI_INT, 0, COMPUT_STATUS, MPI_COMM_WORLD);
				// Get Manager Status
				MPI_Recv(&managerStatus, 1, MPI_INT, 0, MANAGER_STATUS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

				if (managerStatus == SOLUTION_FOUND)
				{
					solutionFound = 1;
					break;
				}
				else if (managerStatus != GRIDS_EMPTY)
					computeGrid(recvGrid(0, COMPUT_RECV_GRIDS), 1);
				// Wait for idling process status from manager
				else
				{
					// Idling wait for more work...
					MPI_Recv(&manager_idlingStatus, 1, MPI_INT, 0, MANAGER_IDLING_STATUS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					if (manager_idlingStatus == SOLUTION_FOUND)
					{
						solutionFound = 1;
						break;
					}
					else if (manager_idlingStatus == 1)
						computeGrid(recvGrid(0, COMPUT_RECV_GRIDS), 1);
				}
			} while (solutionFound == 0);
		}
		//printf("Solution found %i at %i\n", solutionFound,pid);
		//fflush(stdout);
	}
	else if (pid == 0 && process_count == 1) {
		computeGrid(dequeue(&manager_gridQueue), 0);
	}

	// CODE END
	MPI_Barrier(MPI_COMM_WORLD); // Block to calc entire duration
	if (pid == 0) {
		end_time = MPI_Wtime();
		duration = end_time - start_time;
		printf("%f\n", duration);
		printf("%f\n", solutionFindTime);
		printf("%f\n", initDuration);
	}
	MPI_Finalize();
	return 0;
}

// EOF