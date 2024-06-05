#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define QUEUE_KEY 12345
#define MESSAGE_TYPE 1

// Define the maximum message size
#define MAX_MESSAGE_SIZE 256

struct message {
    long msg_type;
    char msg_text[MAX_MESSAGE_SIZE];
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_messages> <delay>\n", argv[0]);
        exit(1);
    }

    int num_messages = atoi(argv[1]);
    int delay = atoi(argv[2]);

    if (num_messages <= 0) {
        fprintf(stderr, "Number of messages must be greater than 0\n");
        exit(1);
    }
    if (delay < 0) {
        fprintf(stderr, "Delay must be 0 or greater\n");
        exit(1);
    }

    int msg_id = msgget(QUEUE_KEY, 0666 | IPC_CREAT);
    if (msg_id < 0) {
        perror("msgget");
        exit(1);
    }

    struct message msg;
    for (int i = 0; i < num_messages; ++i) {
        ssize_t received_size = msgrcv(msg_id, &msg, MAX_MESSAGE_SIZE, MESSAGE_TYPE, 0);
        if (received_size < 0) {
            perror("msgrcv");
            exit(1);
        }
        printf("Message received (size %ld): %s\n", received_size, msg.msg_text);
        sleep(delay);
    }

    return 0;
}
