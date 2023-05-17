#include <mpi.h>
#include <stdio.h>
#include <math.h>

// Utility function to perform binary addition
long inc(long b1, long b2) {
    long sum[4], b3 = 0;
    int i = 0, rem = 0;

    while (b1 != 0 || b2 != 0) {
        sum[i++] =(b1%10 + b2%10 + rem)%2;
        rem =(b1%10 + b2%10 + rem)/2;
        b1 = b1/10;
        b2 = b2/10;
    }
    if (rem != 0)
        sum[i++] = rem;
    i--;
    
    int n = i;
    while (i >= 0) {
        b3 += sum[i]*pow(10, i);
        i--;
    }
    return b3;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Status status;

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Adjustment for 3 processes (size - 1)
    long adj = 10;
    // Initialize to 001
    long binary = 1, max = 111;
    if (rank == 0) {
        printf("Rotating Binary Addition\n\n");
        printf("Initially sending 1 from process 0\n");
        MPI_Send(&binary, 1, MPI_LONG, 1, 0, MPI_COMM_WORLD);
    }

    while (1) {
        if (rank == 0) {
            // Receive the binary number from process 2
            MPI_Recv(&binary, 1, MPI_LONG, 2, 0, MPI_COMM_WORLD, &status);
            
            // Increment
            binary = inc(binary, 1);
            printf("Incremented to %ld by process %d\n", binary, rank);
            
            // Send the binary number to process 1
            MPI_Send(&binary, 1, MPI_LONG, 1, 0, MPI_COMM_WORLD);

            // If there is no chance of this process receiving a number
            // to increment hereafter, exit the loop
            if (inc(binary, adj) >= max) break;
        } else if (rank == 1) {
            // Receive the binary number from process 0
            MPI_Recv(&binary, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, &status);
            
            // Increment
            binary = inc(binary, 1);
            printf("Incremented to %ld by process %d\n", binary, rank);
            
            // Send the binary number to process 2
            MPI_Send(&binary, 1, MPI_LONG, 2, 0, MPI_COMM_WORLD);

            // If there is no chance of this process receiving a number
            // to increment hereafter, exit the loop
            if (inc(binary, adj) >= max) break;   
        } else if (rank == 2) {
            // Receive the binary number from process 1
            MPI_Recv(&binary, 1, MPI_LONG, 1, 0, MPI_COMM_WORLD, &status);

            // Increment
            binary = inc(binary, 1);
            printf("Incremented to %ld by process %d\n", binary, rank);

            // Send the binary number to process 0
            MPI_Send(&binary, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);

            // If there is no chance of this process receiving a number
            // to increment hereafter, exit the loop
            if (inc(binary, adj) >= max) break;
        }
    }

    // Process having the final binary value of 111 will only print
    if (binary == max)
        printf("\nFinal binary value %ld obtained at process %d", binary, rank);
    MPI_Finalize();
}