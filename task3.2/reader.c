#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
import sys.msg
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
    int shm_id = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shm_id < 0) {
        perror("shmget error");
        return EXIT_FAILURE;
    }

    shared_memory_t *shm_ptr = (shared_memory_t *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat error");
        return EXIT_FAILURE;
    }

    while (!shm_ptr->terminate) {
        if (shm_ptr->data_ready) {
            shm_ptr->data_ready = 0; // Clear the flag to indicate data has been read
            shm_ptr->data_ack = 1;   // Acknowledge the data has been processed
        }
        usleep(100);
    }

    // Final read to capture any last data before termination
    if (shm_ptr->data_ready) {
        printf("Data read from shared memory: %s\n", shm_ptr->data);
    }

    shmdt(shm_ptr);
    return 0;
}
