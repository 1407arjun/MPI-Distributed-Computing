#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Status status;

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = *argv[1] - '0', arr[n], sum, psum;
    if (rank == 0) {
        printf("Sum of N numbers\n\n");

        for (int i = 0; i < n; i++) {
            // Random number between 1 and 1000
            arr[i] = rand()%1000 + 1;
        }

        printf("Random input: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
    }

    double start, end , std, ready, syn;
    // Standard Send
    if (rank == 0) {
        start = MPI_Wtime();
        sum = 0;

        // Send the other half subarray to process 1
        MPI_Send(&arr[n%2 == 0 ? n/2 : n/2 + 1], n/2, MPI_INT, 1, 0, MPI_COMM_WORLD);

        // Calculate the partial sum for the first half subarray
        for (int i = 0; i < (n%2 == 0 ? n/2 : n/2 + 1); i++) {
            sum += arr[i];
        }

        // Receive the partial sum of the other subarray from process 1
        MPI_Recv(&psum, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        
        // Add the partial sum to the global sum
        sum += psum;
        
        // Print the total sum from both processes
        printf("\nSum of the %d random numbers is %d\n", n, sum);

        end = MPI_Wtime();
        std = end - start;
        printf("\nTime taken by Standard Send operation %f", std);
    } else {
        // Receive the other half subarray from process 0
        psum = 0;
        MPI_Recv(&arr[n%2 == 0 ? n/2 : n/2 + 1], n/2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        // Calulate partial sum of the received subarray
        for (int i = (n%2 == 0 ? n/2 : n/2 + 1); i < n; i++) {
            psum += arr[i];
        }

        // Send the partial sum back to process 0
        MPI_Send(&psum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // Ready Send
    if (rank == 0) {
        start = MPI_Wtime();
        sum = 0;

        // Send the other half subarray to process 1
        MPI_Rsend(&arr[n%2 == 0 ? n/2 : n/2 + 1], n/2, MPI_INT, 1, 0, MPI_COMM_WORLD);

        // Calculate the partial sum for the first half subarray
        for (int i = 0; i < (n%2 == 0 ? n/2 : n/2 + 1); i++) {
            sum += arr[i];
        }

        // Receive the partial sum of the other subarray from process 1
        MPI_Recv(&psum, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        
        // Add the partial sum to the global sum
        sum += psum;
        
        // Print the total sum from both processes
        //printf("Sum of the %d random numbers is %d\n", n, sum);

        end = MPI_Wtime();
        ready = end - start;
        printf("\nTime taken by Ready Send operation %f", ready);
    } else {
        // Receive the other half subarray from process 0
        psum = 0;
        MPI_Recv(&arr[n%2 == 0 ? n/2 : n/2 + 1], n/2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        // Calulate partial sum of the received subarray
        for (int i = (n%2 == 0 ? n/2 : n/2 + 1); i < n; i++) {
            psum += arr[i];
        }

        // Send the partial sum back to process 0
        MPI_Rsend(&psum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // Synchronous Send
    if (rank == 0) {
        start = MPI_Wtime();
        sum = 0;

        // Send the other half subarray to process 1
        MPI_Ssend(&arr[n%2 == 0 ? n/2 : n/2 + 1], n/2, MPI_INT, 1, 0, MPI_COMM_WORLD);

        // Calculate the partial sum for the first half subarray
        for (int i = 0; i < (n%2 == 0 ? n/2 : n/2 + 1); i++) {
            sum += arr[i];
        }

        // Receive the partial sum of the other subarray from process 1
        MPI_Recv(&psum, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        
        // Add the partial sum to the global sum
        sum += psum;
        
        // Print the total sum from both processes
        //printf("Sum of the %d random numbers is %d\n", n, sum);

        end = MPI_Wtime();
        syn = end - start;
        printf("\nTime taken by Synchronous Send operation %f", syn);
    } else {
        // Receive the other half subarray from process 0
        psum = 0;
        MPI_Recv(&arr[n%2 == 0 ? n/2 : n/2 + 1], n/2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        // Calulate partial sum of the received subarray
        for (int i = (n%2 == 0 ? n/2 : n/2 + 1); i < n; i++) {
            psum += arr[i];
        }

        // Send the partial sum back to process 0
        MPI_Ssend(&psum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        printf("\n\n");
        if (std < ready) {
            if (ready < syn)
                printf("Standard ");
            else {
                if (std < syn)
                    printf("Standard");
                else
                    printf("Synchronous");
            }
        } else {
            if (ready < syn)
                printf("Ready");
            else
                printf("Synchronous");
        }
        printf(" Send is the optimal operation with the least time");
    }

    MPI_Finalize();
}