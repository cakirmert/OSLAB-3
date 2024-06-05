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
    int terminate;
    char data[SHM_SIZE - 3 * sizeof(int)];
} shared_memory_t;

int main() {
    int shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget error");
        exit(EXIT_FAILURE);
    }

    shared_memory_t *shm_ptr = (shared_memory_t *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat error");
        exit(EXIT_FAILURE);
    }

    strcpy(shm_ptr->data, "Hello, world!");
    shm_ptr->terminate = 0;

    while (!shm_ptr->terminate) {
        if (!shm_ptr->data_ready) {
            shm_ptr->data_ready = 1;
            while (!shm_ptr->data_ack) {
                usleep(100);
            }
            shm_ptr->data_ack = 0;  // Reset acknowledgment
        }
        usleep(100);  // Reduce CPU usage
    }

    shmdt(shm_ptr);
    return 0;
}
