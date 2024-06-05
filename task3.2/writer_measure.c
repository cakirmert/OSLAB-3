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
    volatile int data_ready;
    volatile int data_ack;
    volatile int terminate;
    char data[SHM_SIZE - 3 * sizeof(int)];
} shared_memory_t;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <data_size>\n", argv[0]);
        exit(1);
    }

    int data_size = atoi(argv[1]);
    if (data_size <= 0 || data_size > (SHM_SIZE - 3 * sizeof(int))) {
        fprintf(stderr, "Data size must be between 1 and %lu\n", (unsigned long)(SHM_SIZE - 3 * sizeof(int)));
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

    shm_ptr->terminate = 0;
    int count = 1000; // Limit the number of messages

    clock_t start = clock();
    for (int i = 0; i < count; ++i) {
        while (shm_ptr->data_ready) {
            usleep(100); // Wait until the reader has processed the data
        }
        snprintf(shm_ptr->data, data_size, "Message %d", i);
        shm_ptr->data_ready = 1;
        while (!shm_ptr->data_ack) {
            usleep(100); // Wait for acknowledgment
        }
        shm_ptr->data_ack = 0;  // Reset acknowledgment
    }
    clock_t end = clock();

    // Set terminate flag
    while (shm_ptr->data_ready) {
        usleep(100);
    }
    shm_ptr->terminate = 1;

    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Data transfer rate for %d bytes: %f MB/s\n", data_size, (data_size * count) / (1024 * 1024 * time_spent));

    shmdt(shm_ptr);
    return 0;
}
