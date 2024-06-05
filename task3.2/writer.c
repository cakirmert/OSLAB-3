#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_KEY 12345
#define SHM_SIZE 1024

typedef struct {
    int data_ready;  // Writer sets this to 1 when data is ready
    int data_ack;    // Reader sets this to 1 when data is read
    char data[SHM_SIZE - 2 * sizeof(int)];  // Data buffer
} shared_memory_t;

int main() {
    int shm_id = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }

    shared_memory_t *shm_ptr = (shared_memory_t *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (shared_memory_t *)-1) {
        perror("shmat");
        exit(1);
    }

    // Initialize the flags
    shm_ptr->data_ready = 0;
    shm_ptr->data_ack = 0;

    // Write to shared memory
    strcpy(shm_ptr->data, "Hello from writer!");
    shm_ptr->data_ready = 1;  // Set the data ready flag

    // Wait for the reader to acknowledge reading the data
    while (shm_ptr->data_ack == 0) {
        sleep(1);
    }

    // Reset the acknowledgment flag
    shm_ptr->data_ack = 0;

    shmdt(shm_ptr);
    return 0;
}
