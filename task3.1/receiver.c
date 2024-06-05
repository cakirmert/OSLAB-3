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
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_messages>\n", argv[0]);
        exit(1);
    }

    int num_messages = atoi(argv[1]);

    int msg_id = msgget(QUEUE_KEY, 0666 | IPC_CREAT);
    if (msg_id < 0) {
        perror("msgget");
        exit(1);
    }

    struct message msg;
    for (int i = 0; i < num_messages; ++i) {
        ssize_t received_size = msgrcv(msg_id, &msg, sizeof(msg.msg_text), MESSAGE_TYPE, 0);
        if (received_size < 0) {
            perror("msgrcv");
            exit(1);
        }
        printf("Message received (size %ld): %s\n", received_size - sizeof(long), msg.msg_text);
    }

    return 0;
}
