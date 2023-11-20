#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int printpos(const char *string, int filedes){
        off_t pos;
        if((pos =lseek(filedes,0,SEEK_CUR)) == -1)
                perror("lseek failed");
        printf("%s:%ld\n", string, pos);
}


int main(){
	int fd;
	pid_t pid;
	char buf[10];

	if((fd = open("data", O_RDONLY)) == -1)
		perror("open failed");
	read(fd,buf,10);

	printpos("Befoire fork", fd);

	switch(pid = fork()){
		case -1:
			perror("fork failed");
			break;
		case 0:
			printpos("Child before read",fd);
			read(fd, buf, 10);
			printpos("Child after read", fd);
			break;
		default:
			wait((int*)0);
			printpos("Parent after wait", fd);
		}
}

