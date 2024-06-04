#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHM_KEY 12345        // Define the key for the shared memory
#define SHM_SIZE 1024        // Define the size of the shared memory

int main() {
    // Get the shared memory ID
    int shm_id = shmget(SHM_KEY, SHM_SIZE, 0666);
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

    // Read from the shared memory
    printf("Data read from shared memory: %s\n", shm_ptr);

    // Detach from and remove the shared memory
    shmdt(shm_ptr);
    shmctl(shm_id, IPC_RMID, NULL);
    return 0;
}
