# Average of N numbers using Scatter and Gather

### Question

Develop a MPI program to compute the average of numbers using the MPI_Scatter and MPI_Gather functions and compute the time required for the same. [Solution File](./solution.c)

### Solution (C)

```C
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
        printf("Average of N numbers using Scatter and Gather\n\n");

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

    float avg, pavg, pavgs[size];
    // Scatter the elements across all processes
    // Each process computes average of n/size elements
    MPI_Scatter(&arr, n/size, MPI_FLOAT, &parr, n/size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Compute average of each subarray
    pavg = average(parr, n/size);

    // Gather subaverages from all processes and store in pavgs array
    MPI_Gather(&pavg, 1, MPI_FLOAT, &pavgs, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Compute global average of all subaverages
        avg = average(pavgs, size);
        printf("Average: %.2f\n", avg);

        end = MPI_Wtime();
        printf("Time taken using Scatter and Gather: %f", end - start);
    }

    MPI_Finalize();
}
```
