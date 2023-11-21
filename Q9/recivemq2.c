#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#define MAX_MESSAGE_SIZE 1024
#define MESSAGE_TYPE 1

struct Message {
    long messageType;
    int sender;
    int receiver;
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

    // 수신자의 번호 입력
    int receiverNumber;
    printf("Enter your user number: ");
    scanf("%d", &receiverNumber);

    while (1) {
        // 수신한 메시지를 수신자의 번호와 비교하여 출력
        if (msgrcv(msgId, &message, sizeof(struct Message) - sizeof(long), MESSAGE_TYPE, 0) == -1) {
            perror("Error receiving message");
            exit(EXIT_FAILURE);
        }

        if (message.receiver == receiverNumber) {
            printf("Received message from user %d: %s\n", message.sender, message.text);
        }
    }

    return 0;
}

