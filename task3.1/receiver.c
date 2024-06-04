#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define QUEUE_KEY 12345      // Define the key for the message queue
#define MESSAGE_TYPE 1       // Define the message type

// Define the message structure
struct message {
    long msg_type;
    char msg_text[100];
};

int main() {
    // Get the message queue ID
    int msg_id = msgget(QUEUE_KEY, 0666 | IPC_CREAT);
    if (msg_id < 0) {
        perror("msgget");
        exit(1);
    }

    // Receive the message
    struct message msg;
    if (msgrcv(msg_id, &msg, sizeof(msg.msg_text), MESSAGE_TYPE, 0) < 0) {
        perror("msgrcv");
        exit(1);
    }

    printf("Message received: %s\n", msg.msg_text);
    return 0;
}
