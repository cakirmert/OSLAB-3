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

    const char *message = "Hello from writer!";
    strncpy(shm_ptr->data, message, sizeof(shm_ptr->data) - 1);
    shm_ptr->data[sizeof(shm_ptr->data) - 1] = '\0';

    while (1) {
        if (!shm_ptr->data_ack) {
            shm_ptr->data_ready = 1;
            printf("Data written to shared memory: %s\n", shm_ptr->data);
            while (!shm_ptr->data_ack) {
                usleep(100); // Wait for acknowledgment
            }
            shm_ptr->data_ready = 0;
            shm_ptr->data_ack = 0;
        }
        usleep(100); // Sleep to reduce CPU usage
    }

    if (shmdt(shm_ptr) == -1) {
        perror("shmdt error");
        return EXIT_FAILURE;
    }

    return 0;
}
