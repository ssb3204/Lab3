#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_SENTENCES 5
#define MAX_SENTENCE_LENGTH 100

// 문장들
const char sentences[MAX_SENTENCES][MAX_SENTENCE_LENGTH] = {
    "The quick brown fox jumps over the lazy dog.",
    "Programming is fun and rewarding.",
    "C programming is powerful and efficient.",
    "Practice makes perfect.",
    "Keep calm and code on."
};

// 대기 시간 (초)
#define WAIT_TIME 3

// 문장을 출력하고 사용자 입력을 받는 함수
int displayAndInput(const char *sentence) {
    printf("\n%s\n", sentence);

    char input[MAX_SENTENCE_LENGTH];
    fgets(input, sizeof(input), stdin);

    // 개행 문자 제거
    input[strcspn(input, "\n")] = '\0';

    return strcmp(sentence, input); // strcmp가 0이면 정확히 일치
}

int main() {
    printf("타자 연습을 시작합니다. 준비되면 엔터 키를 눌러주세요...\n");
    getchar(); // 사용자가 엔터 키를 누를 때까지 대기

    int totalTypos = 0;
    time_t startTime = time(NULL);

    for (int i = 0; i < MAX_SENTENCES; ++i) {
        const char *currentSentence = sentences[i];

        // 문장을 출력하고 사용자 입력을 받음
        int typos = displayAndInput(currentSentence);

        // 오타 횟수 업데이트
        totalTypos += typos;

        // 대기 시간
        if (i < MAX_SENTENCES - 1) {
            printf("준비 시간 %d초\n", WAIT_TIME);
            sleep(WAIT_TIME);
        }
    }

    // 총 걸린 시간 계산
    time_t endTime = time(NULL);
    double elapsedTime = difftime(endTime, startTime);

    // 평균 분당 타자수 계산
    double averageTypingSpeed = ((MAX_SENTENCES * strlen(sentences[0])) / elapsedTime) * 60;

    printf("\n타이핑이 완료되었습니다!\n");
    printf("총 오타 횟수: %d\n", totalTypos);
    printf("총 소요 시간: %.2f 초\n", elapsedTime);
    printf("평균 분당 타자수: %.2f\n", averageTypingSpeed);

    return 0;
}

