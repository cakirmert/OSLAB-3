#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>

#define SHM_KEY 12345        // Define the key for the shared memory
#define SHM_SIZE 1024        // Define the size of the shared memory

int main() {
    // Get the shared memory ID
    int shm_id = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach to the shared memory
    char *shm_ptr = (char *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    // Define the data size for the test
    size_t data_size = 256; // Modify this to test different sizes
    memset(shm_ptr, 'A', data_size);
    shm_ptr[data_size] = '\0';

    // Measure the data transfer rate
    clock_t start = clock();
    for (int i = 0; i < 100000; ++i) {
        memcpy(shm_ptr, shm_ptr, data_size);
    }
    clock_t end = clock();

    // Calculate and print the transfer rate
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Data transfer rate for %zu bytes: %f MB/s\n", data_size, (data_size * 100000.0) / (1024 * 1024 * time_spent));

    // Detach from the shared memory
    shmdt(shm_ptr);
    return 0;
}
