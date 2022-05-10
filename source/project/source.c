/********************************************************************

Project:

Original authors:
(sorted by lastname)
	Daitche Konstantin
	Jurczenko Kamil
	Kose Martin
	Neufeld Florian

*********************************************************************/

#include <stdio.h>
#include <mpi.h>

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

	// ALMOST ALL OF YOUR CODE BELONGS IN HERE

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