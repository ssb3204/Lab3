#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#define MAX_MESSAGE_SIZE 1024
#define MESSAGE_TYPE 1

struct Message {
    long messageType;
    char text[MAX_MESSAGE_SIZE];
};

int main() {
    key_t key = ftok(".", 'a');
    if (key == -1) {
        perror("Error creating key");
        exit(EXIT_FAILURE);
    }

    int msgId = msgget(key, 0666 | IPC_CREAT);
    if (msgId == -1) {
        perror("Error creating message queue");
        exit(EXIT_FAILURE);
    }

    struct Message message;

    printf("Enter your message: ");
    fgets(message.text, MAX_MESSAGE_SIZE, stdin);
    message.text[strcspn(message.text, "\n")] = '\0';
    message.messageType = MESSAGE_TYPE;

    if (msgsnd(msgId, &message, sizeof(message.text), 0) == -1) {
        perror("Error sending message");
        exit(EXIT_FAILURE);
    }

    printf("Message sent: %s\n", message.text);

    return 0;
}

