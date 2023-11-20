#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

void list_files_recursive(const char *path, int depth) {
    DIR *dir;
    struct dirent *entry;

    // 디렉토리 열기
    if (!(dir = opendir(path))) {
        perror("opendir");
        return;
    }

    // 현재 디렉토리 정보 출력
    printf("%*s%s:\n", depth * 2, "", path);

    // 디렉토리 내의 파일과 디렉토리 나열
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            // 디렉토리인 경우 재귀 호출
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char new_path[1024];
                snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);
                list_files_recursive(new_path, depth + 1);
            }
        } else {
            // 파일인 경우 출력
            printf("%*s%s/%s\n", (depth + 1) * 2, "", path, entry->d_name);
        }
    }

    // 디렉토리 닫기
    closedir(dir);
}

int main() {
    // 현재 실행 중인 프로그램의 경로 얻기
    char program_path[1024];
    if (readlink("/proc/self/exe", program_path, sizeof(program_path)) == -1) {
        perror("readlink");
        return 1;
    }

    // 경로에서 프로그램 이름 제거
    char *last_slash = strrchr(program_path, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
    }

    // 디렉토리 내의 파일과 디렉토리를 재귀적으로 나열
    list_files_recursive(program_path, 0);

    return 0;
}

