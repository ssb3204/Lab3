#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

void list_files_recursive(const char *path, int depth) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path))) {
        perror("opendir");
        return;
    }

    printf("%*s%s: \n", depth *2, "", path);

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
    		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char new_path[1024];
                snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);
                printf("%s\n", new_path);
                list_files_recursive(new_path, depth + 1);
            }
        } else {
            
            printf("%*s%s/%s\n", (depth + 1) *2, "", path, entry->d_name);
        }
    }

    closedir(dir);
}

int main() {

    char program_path[1024];	
    if(readlink("/proc/self/exe", program_path, sizeof(program_path)) == -1) {
	    perror("readlink");
	    return 1;
	}

    char *last_slash = strrchr(program_path, '/');
    if(last_slash != NULL){
	    *last_slash = '\0';
    }

    list_files_recursive(program_path,0);

    return 0;
}

