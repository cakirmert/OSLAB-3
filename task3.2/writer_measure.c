#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>

#define SHM_KEY 12345
#define SHM_SIZE 1024

typedef struct {
    int data_ready;
    int data_ack;
    char data[SHM_SIZE - 2 * sizeof(int)];
} shared_memory_t;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <data_size>\n", argv[0]);
        exit(1);
    }

    int data_size = atoi(argv[1]);
    if (data_size <= 0 || data_size > (SHM_SIZE - 2 * sizeof(int))) {
        fprintf(stderr, "Data size must be between 1 and %lu\n", (unsigned long)(SHM_SIZE - 2 * sizeof(int)));
        exit(1);
    }

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

    memset(shm_ptr->data, 'A', data_size);
    shm_ptr->data[data_size] = '\0';
    shm_ptr->data_ready = 0;
    shm_ptr->data_ack = 0;

    clock_t start = clock();
    for (int i = 0; i < 100000; ++i) {
        shm_ptr->data_ready = 1;
        while (!shm_ptr->data_ack) {
            usleep(100); // Wait for acknowledgment
        }
        shm_ptr->data_ack = 0;
    }
    clock_t end = clock();

    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Data transfer rate for %d bytes: %f MB/s\n", data_size, (data_size * 100000.0) / (1024 * 1024 * time_spent));

    if (shmdt(shm_ptr) == -1) {
        perror("shmdt error");
        return EXIT_FAILURE;
    }

    return 0;
}
