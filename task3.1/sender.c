#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define QUEUE_KEY 12345
#define MESSAGE_TYPE 1

// Define the maximum message size
#define MAX_MESSAGE_SIZE 80

struct message {
    long msg_type;
    char msg_text[MAX_MESSAGE_SIZE];
};

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <message_size> <num_messages> <delay>\n", argv[0]);
        exit(1);
    }

    int message_size = atoi(argv[1]);
    int num_messages = atoi(argv[2]);
    int delay = atoi(argv[3]);

    if (message_size <= 0 || message_size > MAX_MESSAGE_SIZE) {
        fprintf(stderr, "Message size must be between 1 and %d\n", MAX_MESSAGE_SIZE);
        exit(1);
    }
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
    msg.msg_type = MESSAGE_TYPE;
    memset(msg.msg_text, 'A', message_size);
    msg.msg_text[message_size] = '\0';

    for (int i = 0; i < num_messages; ++i) {
        if (msgsnd(msg_id, &msg, message_size, 0) < 0) {
            perror("msgsnd");
            exit(1);
        }
        printf("Message sent: %s\n", msg.msg_text);
        sleep(delay);
    }

    return 0;
}
