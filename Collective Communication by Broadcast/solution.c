#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Status status;

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start, end;

    // Assume process 0 wants to broadcast some data
    // Data is randomly generated each time
    int n = *argv[1] - '0', arr[n];
    if (rank == 0) {
        printf("Collective Communication by Broadcast\n\n");

        for (int i = 0; i < n; i++) {
            // Random number between 1 and 10
            arr[i] = rand()%10 + 1;
        }

        printf("Data to be broadcasted by process %d: ", rank);
        for (int i = 0; i < n; i++) {
            printf("%d ", (int) arr[i]);
        }
        printf("\n\n");
    }

    // Send-Receive
    if (rank == 0) {
        start = MPI_Wtime();

        // Send data to all except current process
        printf("Process %d is broadcasting using Send-Receive\n", rank);
        
        for (int i = 0; i < size; i++) {
            if (i != rank) 
                MPI_Send(&arr, n, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // All other processes receive broadcasted data
        MPI_Recv(&arr, n, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        printf("Process %d received data from process 0: ", rank);
        for (int i = 0; i < n; i++) {
            printf("%d ", (int) arr[i]);
        }
        printf("\n");
    }
    
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        end = MPI_Wtime();
        printf("Time taken for collective communication using Send-Receive: %f\n\n", end - start);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    
    // Bcast
    if (rank == 0) {
        start = MPI_Wtime();

        printf("Process 0 is broadcasting using Bcast\n");
    }

    // Send data to all including root process 0
    MPI_Bcast(&arr, n, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Print received data
    printf("Process %d received data from process 0: ", rank);
    for (int i = 0; i < n; i++) {
        printf("%d ", (int) arr[i]);
    }
    printf("\n");
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == 0) {
        end = MPI_Wtime();
        printf("Time taken for collective communication using Bcast: %f", end - start);
    }

    MPI_Finalize();
}