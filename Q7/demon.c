#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/resource.h>

void daemonize(char *cmd){
	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;

	umask(0);

	if(getrlimit(RLIMIT_NOFILE, &rl)<0)
		perror("getlimit");

	if((pid=fork())<0)
		exit(1);
	else if(pid != 0)
		exit(0);
	setsid();
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		perror("sigaction: can't ignore SIGHUP");
	if ((pid = fork()) < 0)
		exit (1);
	else if (pid != 0)
		exit(0) ; 
	chdir("/") ;
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		pclose(i);
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
       	fd2 = dup(0);
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected filedescriptors %d %d %d", fd0, fd1, fd2);
		exit(1);
	}	
}

int main(int argc, char* argv[]){
	daemonize(argv[1]);
	sleep(10);
	syslog(LOG_INFO, "THIS is a daemon test process");
	sleep(100);
}

