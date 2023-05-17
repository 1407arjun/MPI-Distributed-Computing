#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

float average(float arr[], int n) {
    float sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return (float) sum/n;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Status status;

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start, end;

    int n = *argv[1] - '0';
    float arr[n], parr[n/size];
    if (rank == 0) {
        printf("Average of N numbers using Scatter and Reduce\n\n");

        for (int i = 0; i < n; i++) {
            // Random number between 1 and 1000
            arr[i] = rand()%1000 + 1;
        }

        printf("Random input: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", (int) arr[i]);
        }
        printf("\n");

        start = MPI_Wtime();
    }

    float avg, sum = 0, psum[n/size];
    // Scatter the elements across all processes
    // Each process gets n/size elements
    MPI_Scatter(&arr, n/size, MPI_FLOAT, &parr, n/size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    // Compute sum of the nth element of parr each process
    // Store the reduced sum at the nth position of psum
    MPI_Reduce(&parr, &psum, n/size, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) { 
        // Compute global sum of all reduced sums
        for (int i = 0; i < n/size; i++) {
            sum += psum[i];
        }
        printf("Total sum: %.2f\n", sum);

        // Compute average
        avg = (float) sum/n;
        printf("Average: %.2f\n", avg);

        end = MPI_Wtime();
        printf("Time taken using Scatter and Reduce: %f", end - start);
    }

    MPI_Finalize();
}