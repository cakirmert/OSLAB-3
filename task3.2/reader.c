#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHM_KEY 12345
#define SHM_SIZE 1024

typedef struct {
    int data_ready;  // Writer sets this to 1 when data is ready
    int data_ack;    // Reader sets this to 1 when data is read
    char data[SHM_SIZE - 2 * sizeof(int)];  // Data buffer
} shared_memory_t;

int main() {
    int shm_id = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }

    shared_memory_t *shm_ptr = (shared_memory_t *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (shared_memory_t *)-1) {
        perror("shmat");
        exit(1);
    }

    for (int i = 0; i < 100000; ++i) {
        // Wait for the writer to set the data ready flag
        while (shm_ptr->data_ready == 0) {
            // Busy wait
        }

        // Read from shared memory and clear the flag
        shm_ptr->data_ready = 0;  // Clear the data ready flag
        shm_ptr->data_ack = 1;    // Set the acknowledgment flag
    }

    shmdt(shm_ptr);
    return 0;
}
