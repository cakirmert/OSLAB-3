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

    shm_ptr->terminate = 0;
    int count = 1000; // Limit the number of iterations

    for (int i = 0; i < count; ++i) {
        while (shm_ptr->data_ready) {
            usleep(100); // Wait until the reader has processed the data
        }
        sprintf(shm_ptr->data, "Message %d", i);
        shm_ptr->data_ready = 1;
    }

    // Set terminate flag and ensure the reader reads the last message
    while (shm_ptr->data_ready) {
        usleep(100);
    }
    shm_ptr->terminate = 1;
    shm_ptr->data_ready = 1;

    shmdt(shm_ptr);
    return 0;
}
