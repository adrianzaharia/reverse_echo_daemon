#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>

#include "config.h"

#define SHELL_RL_BUFSIZE 1024
#define SHELL_TOK_BUFSIZE 64
#define SHELL_TOK_DELIM " \t\r\n\a"

int clientSocket;
struct sockaddr_in serverAddr;
socklen_t addr_size;

/*
  Function Declarations for builtin shell commands:
 */
int shell_send(char **args);
int shell_connect(char **args);
int shell_help(char **args);
int shell_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "send",
  "connect",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &shell_send,
  &shell_connect,
  &shell_help,
  &shell_exit
};

int shell_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

int shell_connect(char **args)
{
	if (args[1] == NULL) {
		fprintf(stderr, "shell: expected ip argument to \"connect\"\n");
	} else if (args[2] == NULL) {
		fprintf(stderr, "shell: expected port argument to \"connect\"\n");
	} else {
		/*---- Create the socket. The three arguments are: ----*/
		/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
		clientSocket = socket(PF_INET, SOCK_STREAM, 0);

		/*---- Configure settings of the server address struct ----*/
		/* Address family = Internet */
		serverAddr.sin_family = AF_INET;
		/* Set port number, using htons function to use proper byte order */
		serverAddr.sin_port = htons(atoi(args[2]));
		/* Set IP address to localhost */
		serverAddr.sin_addr.s_addr = inet_addr(args[1]);
		/* Set all bits of the padding field to 0 */
		memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

		/*---- Connect the socket to the server using the address struct ----*/
		addr_size = sizeof serverAddr;
		if (connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size) < 0) {
			fprintf(stderr, "shell: failed to connect to %s %s\n", args[1], args[2]);
			return 1;
		} else {
			printf("Connected.\n");
		}
		/* TODO add thread for listening messages from server */
		#if 0
		/*---- Read the message from the server into the buffer ----*/
		recv(clientSocket, buffer, 1024, 0);

		/*---- Print the received message ----*/
		printf("Data received: %s",buffer);
		#endif
	}
	return 1;
}

int shell_send(char **args)
{
	if (args[1] == NULL) {
		fprintf(stderr, "shell: expected argument to \"send\"\n");
	} else {
		char message[BUF_SIZE+1];
		int size = strlen(args[1]);

		if(size > BUF_SIZE) {
			fprintf(stderr, "Message size too long\n");
			return 1;
		}

		strncpy(message, args[1], BUF_SIZE);
		message[size] = 0xD; //CR
		fprintf(stderr, "sending %d bytes, ", size+1);
		if (send(clientSocket, message ,  size+1 , 0) < 0) {
			fprintf(stderr, "ERROR writing to socket\n");
			return 1;
		}
		memset(message, 0x0, BUF_SIZE+1);
		int len = 0;
		if ((len = recv(clientSocket , message , BUF_SIZE , 0)) < 0 ) {
			fprintf(stderr, "recv error \n");
		}
		message[len-1] = '\0'; //replace CR with '\0'
		fprintf(stderr, "\n\nresponse %d bytes = '%s'\n", len, message);
	}
	return 1;
}

int shell_help(char **args)
{
	int i;
	printf("Type program names and arguments, and hit enter.\n");
	printf("Available commands:\n");

	for (i = 0; i < shell_num_builtins(); i++) {
		printf("  %s\n", builtin_str[i]);
	}

	return 1;
}

int shell_exit(char **args)
{
	return 0;
}

char *shell_read_line(void)
{
	char *line = NULL;
	ssize_t bufsize = 0; // have getline allocate a buffer for us
	getline(&line, &bufsize, stdin);
	return line;
}

char **shell_split_line(char *line)
{
	int bufsize = SHELL_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if (!tokens) {
		fprintf(stderr, "shell: allocation error\n");
		exit(1);
	}

	token = strtok(line, SHELL_TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += SHELL_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "shell: allocation error\n");
				exit(1);
			}
		}

		token = strtok(NULL, SHELL_TOK_DELIM);
	}

	tokens[position] = NULL;
	return tokens;
}

int shell_launch(char **args)
{
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0) {
		// Child process
		if (execvp(args[0], args) == -1) {
			perror("shell");
		}
		exit(1);
	} else if (pid < 0) {
		// Error forking
		perror("shell");
	} else {
		// Parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

int shell_execute(char **args)
{
	int i;

	if (args[0] == NULL) {
		// An empty command was entered.
		return 1;
	}

	for (i = 0; i < shell_num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}

	return shell_launch(args);
}

void shell_loop(void) {
	char *line;
	char **args;
	int status;

	do {
	printf("> ");
	line = shell_read_line();
	args = shell_split_line(line);
	status = shell_execute(args);

	free(line);
	free(args);
	} while (status);
}

int main(){
	char buffer[1024];

	// Run command loop.
	shell_loop();

	return 0;
}