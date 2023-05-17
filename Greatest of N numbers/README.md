# Greatest of N numbers

### Question

MPI_Bsend is the asynchronous blocking send (with user provided buffering), it will block until a copy of the buffer is passed. Write a MPI program to compute the biggest of ‘n’ numbers using point to point buffered send communication. [Solution File](./solution.c)

### Solution (C)

```C
#include <mpi.h>
#include <stdio.h>

int find_max(int numbers[], int start, int len) {
    int max = numbers[start];
    for (int i = start + 1; i < start + len; i++) {
        if (numbers[i] > max) max = numbers[i];
    }
    return max;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Status status;

    int n = *argv[1] - '0', nums[n];
    for (int i = 0; i < n; i++) {
        nums[i] = *argv[i + 2] - '0';
    }

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int q = n/size, r = n%size;

    int start, len;
    int max, local, smax;
    if (rank == 0) {
        printf("Greatest of N numbers\n\n");

        printf("Input: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", nums[i]);
        }
        printf("\n\n");

        // Distributing remainder elements to some processes
        for (int i = 1; i < r; i++) {
            len = q + 1;
            start = i*q + i;
            MPI_Bsend(&len, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Bsend(&nums, len, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Equal distribution after add reminders have been distributed
        for (int i = r; i < size; i++) {
            len = q;
            start = i*q + r;
            MPI_Bsend(&len, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Bsend(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Bsend(&nums, n, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Calculation for the master process itself
        if (r == 0) len = q;
        else len = q + 1;
        max = find_max(nums, 0, len);

        // Receive local maximums from other processes
        for (int i = 1; i < size; i++) {
            MPI_Recv(&local, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            if (local > max) max = local;
        }

    } else {
        // Receive length and subarray
        MPI_Recv(&len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&nums, n, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        // Find and send back the local maximum to the master process
        smax = find_max(nums, start, len);
        MPI_Bsend(&smax, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        printf("The maximum number is %d", max);
    }

    MPI_Finalize();
}
```
