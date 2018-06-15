#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define LOCK_FILE "/tmp/reverse_echo.lock"

int file_lock(int p_cmd) {
    int p_lock;
    char str[10];
    p_lock = open(LOCK_FILE,O_RDWR|O_CREAT,0640);
    if(p_lock < 0)
        return 1;
    if(lockf(p_lock,p_cmd,0) < 0)
        return 1;

    switch (p_cmd) {
	case F_TLOCK:
	    /* write pid in lock file*/
	    sprintf(str,"%d\n",getpid());
	    write(p_lock,str,strlen(str));
	    break;
	case F_ULOCK:
	    /* close and remove lock file */
	    close(p_lock);
	    remove(LOCK_FILE);
	    break;
    }
    return 0;
}

void signal_handler(int sig){
    switch(sig){
	case SIGTERM:
	    exit(file_lock(F_ULOCK));
	    break;
    }
}

void daemonize(){
    int pid;
    char str[10];
    if(getppid() == 1)
        return;
    pid = fork();

    if(pid < 0)
        exit(1);
    /* fork success. exit parent */
    if(pid > 0)
        exit(0);
    setsid();

    if(file_lock(F_TLOCK) != 0)
        exit(1);

    signal(SIGTERM,signal_handler);
}

int main(int argc,char **argv){
    daemonize();
    while(1)
        sleep(1);
}

