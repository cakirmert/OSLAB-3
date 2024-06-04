#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

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

    // Write to the shared memory
    strcpy(shm_ptr, "Hello from writer!");
    printf("Data written to shared memory: %s\n", shm_ptr);

    // Detach from the shared memory
    shmdt(shm_ptr);
    return 0;
}
