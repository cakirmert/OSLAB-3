#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
        fprintf(stderr, "Usage: %s <message_size> <num_messages>\n", argv[0]);
        exit(1);
    }

    int message_size = atoi(argv[1]);
    int num_messages = atoi(argv[2]);

    if (message_size <= 0 || message_size > MAX_MESSAGE_SIZE) {
        fprintf(stderr, "Message size must be between 1 and %d\n", MAX_MESSAGE_SIZE);
        exit(1);
    }

    int msg_id = msgget(QUEUE_KEY, 0666 | IPC_CREAT);
    if (msg_id < 0) {
        perror("msgget");
        exit(1);
    }

    struct message msg;
    msg.msg_type = MESSAGE_TYPE;
    for (int i = 0; i < num_messages; ++i) {
        memset(msg.msg_text, 'A', message_size);
        msg.msg_text[message_size] = '\0';
        if (msgsnd(msg_id, &msg, message_size + sizeof(long), 0) < 0) {
            perror("msgsnd");
            exit(1);
        }
        printf("Message sent (size %d): %s\n", message_size, msg.msg_text);
        sleep(1);  // Ensure there's a delay between messages
    }

    return 0;
}
