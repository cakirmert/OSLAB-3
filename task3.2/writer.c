#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHM_KEY 12345
#define SHM_SIZE 1024

typedef struct {
    volatile int data_ready;
    volatile int data_ack;
    volatile int terminate;
    char data[SHM_SIZE - 3 * sizeof(int)];
} shared_memory_t;

int main() {
    int shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget error");
        return EXIT_FAILURE;
    }

    shared_memory_t *shm_ptr = (shared_memory_t *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat error");
        return EXIT_FAILURE;
    }

    // Initialize shared memory structure
    shm_ptr->data_ready = 0;
    shm_ptr->data_ack = 0;
    shm_ptr->terminate = 0;

    // Write data to shared memory
    strcpy(shm_ptr->data, "Hello, world!");
    shm_ptr->data_ready = 1;

    // Wait for acknowledgment
    while (!shm_ptr->data_ack) {
        usleep(100); // Small delay to reduce CPU usage
    }

    // Set terminate flag
    shm_ptr->terminate = 1;

    // Detach from shared memory
    shmdt(shm_ptr);
    return 0;
}
