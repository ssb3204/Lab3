#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>

#define SHM_SIZE 1024
#define SEM_KEY 1234

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void P(int semId) {
    struct sembuf sb = {0, -1, 0};
    if (semop(semId, &sb, 1) == -1) {
        perror("P: semop");
        exit(EXIT_FAILURE);
    }
}

void V(int semId) {
    struct sembuf sb = {0, 1, 0};
    if (semop(semId, &sb, 1) == -1) {
        perror("V: semop");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    key_t key = ftok(".", 'a');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // 공유 메모리 생성
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // 세마포어 생성 또는 얻기
    int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1) {
        // 이미 세마포어가 존재하면 얻기
        semid = semget(key, 1, 0);
    } else {
        // 세마포어가 새로 생성되었으면 초기화
        union semun su;
        su.val = 1;
        if (semctl(semid, 0, SETVAL, su) == -1) {
            perror("semctl");
            exit(EXIT_FAILURE);
        }
    }

    // 공유 메모리에 파일 데이터 복사
    int pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // 자식 프로세스
        P(semid); // 세마포어 잠금

        // 공유 메모리에 저장된 데이터를 읽어서 파일에 쓰기
        FILE *destFile = fopen(argv[2], "w");
        if (destFile == NULL) {
            perror("fopen");
            V(semid); // 세마포어 잠금 해제
            exit(EXIT_FAILURE);
        }

        char *shmaddr = (char *)shmat(shmid, NULL, 0);
        if (shmaddr == (void *)-1) {
            perror("shmat");
            fclose(destFile);
            V(semid); // 세마포어 잠금 해제
            exit(EXIT_FAILURE);
        }

        fprintf(destFile, "%s", shmaddr);

        if (shmdt(shmaddr) == -1) {
            perror("shmdt");
            fclose(destFile);
            V(semid); // 세마포어 잠금 해제
            exit(EXIT_FAILURE);
        }

        fclose(destFile);
        V(semid); // 세마포어 잠금 해제

        exit(EXIT_SUCCESS);
    } else { // 부모 프로세스
        P(semid); // 세마포어 잠금

        // 파일에서 데이터를 읽어서 공유 메모리에 쓰기
        FILE *srcFile = fopen(argv[1], "r");
        if (srcFile == NULL) {
            perror("fopen");
            V(semid); // 세마포어 잠금 해제
            exit(EXIT_FAILURE);
        }

        char *shmaddr = (char *)shmat(shmid, NULL, 0);
        if (shmaddr == (void *)-1) {
            perror("shmat");
            fclose(srcFile);
            V(semid); // 세마포어 잠금 해제
            exit(EXIT_FAILURE);
        }

        size_t bytesRead = fread(shmaddr, 1, SHM_SIZE - 1, srcFile);
        shmaddr[bytesRead] = '\0';

        if (shmdt(shmaddr) == -1) {
            perror("shmdt");
            fclose(srcFile);
            V(semid); // 세마포어 잠금 해제
            exit(EXIT_FAILURE);
        }

        fclose(srcFile);
        V(semid); // 세마포어 잠금 해제

        wait(NULL);
    }

    // 공유 메모리 및 세마포어 제거
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    if (semctl(semid, 0, IPC_RMID, NULL) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    return 0;
}

