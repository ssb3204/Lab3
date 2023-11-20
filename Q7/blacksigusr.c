#include <stdio.h>
#include <signal.h>
void handler1(int);
void handler2(int);
sigset_t set;
int main(){
	struct sigaction act;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = handler1;
	sigaction(SIGUSR1, &act, NULL);

	act.sa_handler = handler2;
	sigaction(SIGUSR2, &act, NULL);
	sigprocmask(SIG_BLOCK, &set, NULL);
	while(1) {
		pause();
		sigprocmask(SIG_UNBLOCK, &set, NULL);
	}
}

void handler1(int sig){
	printf("Got a SIGUSR1 signal\n");
}

void handler2(int sig){
	printf("Got a SIGUSR2 signal\n");
}
