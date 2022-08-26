#include <omp.h>
#include "sudoku_seriell.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>

double solutionFindTime;
double start_time;

grids_queue* available_gridsQ = NULL;
int gridsNum;
int idlingProcessesNum = 0;
int threadNum;

int attemptGridCache(struct grid sudokuGrid)
{
	if (gridsNum < idlingProcessesNum)
	{
		#pragma omp critical
		{
			enqueue_cpy(&available_gridsQ, sudokuGrid);
			gridsNum++;
		}
		return 1;
	}
	else return 0;
}

int main(int argc, char** argv) {

	double end_time;
	double duration;
	double initDuration;

	threadNum = atoi(argv[1]);
	char* file = argv[2];
	omp_set_num_threads(threadNum);
	start_time = omp_get_wtime();
	
	/*
	if (1)
	{
	int t;
	scanf("%i", &t);
	}
	*/
	
	// CODE START

	if (threadNum == 0)
		return 0;

	#pragma omp parallel
	{
		// First Thread initializes first sudoku grids to compute
		#pragma omp single
		{
			double startInit_time = omp_get_wtime();
			available_gridsQ = initParallel(threadNum, &gridsNum, file);
			initDuration = omp_get_wtime() - startInit_time;
		}
		
		int isIdling = 0;
		double totalComputeTime = 0;
		do {
			struct grid nextSudoku;
			#pragma omp critical
				nextSudoku = dequeue(&available_gridsQ);
			if (nextSudoku.size > 0)
			{
				#pragma omp atomic
					gridsNum--;
				if (isIdling == 1) {
					isIdling = 0;
					#pragma omp atomic
						idlingProcessesNum--;
				}
				double t = omp_get_wtime();
				if (solveSudoku(&nextSudoku, 0, 0, NULL, NULL) == 1)
				{
					//printf("<<<<<<<<<<<<<<<<<Solution found on pid %d >>>>>>>>>>>>>>>>>>>>>\n", omp_get_thread_num());
					// Lösung ausgeben
					//printBoard(&grid);
					solutionFindTime = omp_get_wtime() - start_time;
				}
				t = omp_get_wtime() - t;
				#pragma omp atomic
					totalComputeTime += t;
				free(nextSudoku.sudoku);
			}
			else if (isIdling == 0) // TODO measure IDLING TIME
			{
				isIdling = 1;
				#pragma omp atomic
					idlingProcessesNum++;
				//printf("idling processes= %i grids= %i\n", idlingProcessesNum, gridsNum);
			}
		} while (idlingProcessesNum < threadNum);
		printf("took %f seconds to compute w thread %i \n", totalComputeTime, omp_get_thread_num());
	}

	end_time = omp_get_wtime();

	duration = end_time - start_time;
	printf("%f\n", duration);
	printf("%f\n", solutionFindTime);
	printf("%f\n", initDuration);

	return 0;
}
