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

    // 발신자와 수신자의 번호 입력
    printf("Enter your user number: ");
    scanf("%d", &message.sender);
    printf("Enter the recipient's user number: ");
    scanf("%d", &message.receiver);

    printf("Enter your message: ");
    getchar(); // Consume the newline character left by scanf
    fgets(message.text, MAX_MESSAGE_SIZE, stdin);
    message.text[strcspn(message.text, "\n")] = '\0';
    message.messageType = MESSAGE_TYPE;

    if (msgsnd(msgId, &message, sizeof(struct Message) - sizeof(long), 0) == -1) {
        perror("Error sending message");
        exit(EXIT_FAILURE);
    }

    printf("Message sent to user %d: %s\n", message.receiver, message.text);

    return 0;
}

