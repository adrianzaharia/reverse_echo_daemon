#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <netinet/in.h>

#include "config.h"

#define MAX_CONNECTIONS 10

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

	switch (c) {
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
		fprintf(stderr, "PORT: %d\n SERVER IP: %s\n", config.port, config.server_ip);
	} else {
		fprintf(stderr, "Error opening config file -> using default values(IP: %s PORT: %d).",
               DEFAULT_SERVER_IP, DEFAULT_PORT);
	}
	if(daemonize_flag)
		daemonize();

	/* server connection */
	int sockfd, clientsockfd;
	struct sockaddr_in serv_addr, client_addr;
	int client_add_len = sizeof(client_addr);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		fprintf(stderr, "ERROR opening socket");
		exit(1);
	}

	/* Initialize socket structure */
	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(config.server_ip);
	serv_addr.sin_port = htons(config.port);

	/* Now bind the host address using bind() call.*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "ERROR on binding");
		exit(1);
	}

	if(listen(sockfd, MAX_CONNECTIONS) < 0) {
		fprintf(stderr, "ERROR on listen\n");
	}

	while(1) {
		clientsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_add_len);

		if (clientsockfd < 0) {
			fprintf(stderr, "ERROR on accept");
			exit(1);
		}
		sleep(1);
	}
	return 0;
}

