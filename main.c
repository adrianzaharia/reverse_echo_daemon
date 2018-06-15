#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "config.h"

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
	default:
		return 2;
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

	int c;
	static int daemonize_flag = 0;
	char config_file[BUF_SIZE];
	config_t config;

	strncpy(config_file, DEFAULT_CONFIG_FILENAME, BUF_SIZE);

	while (1) {
	  static struct option long_options[] =
		{
		  /* These options set a flag. */
		  {"daemon", no_argument,       &daemonize_flag, 1},
		  {"config",  required_argument, 0, 'c'},
		  {0, 0, 0, 0}
		};
	  /* getopt_long stores the option index here. */
	  int option_index = 0;

	  c = getopt_long (argc, argv, "dc::", long_options, &option_index);

	  /* Detect the end of the options. */
	  if (c == -1)
		break;

	  switch (c)
		{

		case 'c':
		  strncpy(config_file, optarg, BUF_SIZE);
		  break;


		case '?':
		  /* getopt_long already printed an error message. */
		  break;

		default:
		  exit (1);
		}
	} // end while(1) read of arguments

	if (get_config(config_file, &config) == 0) {
		printf ("PORT: %d\n SERVER IP: %s\n", config.port, config.server_ip);
	} else {
		printf("Error opening config file -> using default values(IP: %s PORT: %d).",
               DEFAULT_SERVER_IP, DEFAULT_PORT);
	}
	if(daemonize_flag)
		daemonize();
	while(1)
		sleep(1);
	return 0;
}

