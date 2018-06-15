#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>

#define SHELL_RL_BUFSIZE 1024
#define SHELL_TOK_BUFSIZE 64
#define SHELL_TOK_DELIM " \t\r\n\a"

/*
  Function Declarations for builtin shell commands:
 */
int shell_send(char **args);
int shell_help(char **args);
int shell_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "send",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &shell_send,
  &shell_help,
  &shell_exit
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/
int shell_send(char **args)
{
	if (args[1] == NULL) {
		fprintf(stderr, "shell: expected argument to \"send\"\n");
	} else {
		/* client send data*/
		/*if (chdir(args[1]) != 0) {
			perror("shell");
		}*/
	}
	return 1;
}

int shell_help(char **args)
{
	int i;
	printf("Type program names and arguments, and hit enter.\n");
	printf("Available commands:\n");

	for (i = 0; i < lsh_num_builtins(); i++) {
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

	for (i = 0; i < lsh_num_builtins(); i++) {
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

	int clientSocket;
	char buffer[1024];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	// Run command loop.
	shell_loop();
#if 0
	/*---- Create the socket. The three arguments are: ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	/*---- Configure settings of the server address struct ----*/
	/* Address family = Internet */
	serverAddr.sin_family = AF_INET;
	/* Set port number, using htons function to use proper byte order */
	serverAddr.sin_port = htons(7891);
	/* Set IP address to localhost */
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	/* Set all bits of the padding field to 0 */
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	/*---- Connect the socket to the server using the address struct ----*/
	addr_size = sizeof serverAddr;
	connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

	/*---- Read the message from the server into the buffer ----*/
	recv(clientSocket, buffer, 1024, 0);

	/*---- Print the received message ----*/
	printf("Data received: %s",buffer);
#endif
	return 0;
}