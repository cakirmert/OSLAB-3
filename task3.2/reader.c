#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHM_KEY 12345
#define SHM_SIZE 1024

typedef struct {
    int data_ready;
    int data_ack;
    char data[SHM_SIZE - 2 * sizeof(int)];
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

    while (1) {
        if (shm_ptr->data_ready) {
            printf("Data read from shared memory: %s\n", shm_ptr->data);
            shm_ptr->data_ready = 0;
            shm_ptr->data_ack = 1;
        }
        usleep(100); // Sleep to reduce CPU usage
    }

    if (shmdt(shm_ptr) == -1) {
        perror("shmdt error");
        return EXIT_FAILURE;
    }

    return 0;
}
