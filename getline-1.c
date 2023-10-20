#include "shell.h"

void str_tok(char *token, char *arguements[], int *argCount);
void create_newProcess(char *arguements[]);
void printEnv(char **env_command);
void exit_handler(char **arguements, char *buffer);

/**
  * printEnv - Prints the current environment - (built in env)
  * @env_command: current environment
  * Return: void
  */
void printEnv(char **env_command)
{
	unsigned int i = 0;

	while (env_command[i] != NULL)
	{
		write(STDOUT_FILENO, env_command[i], strlen(env_command[i]));
		write(STDOUT_FILENO, "\n", 2);
		i++;
	}
}

/**
  * str_tok - breaks string into array of tokens
  * @token: pointer to string to tokenize
  * @arguements: pointer to array of arguments
  * @argCount: Number of arguments
  * Return: void
  */
void str_tok(char *token, char *arguements[], int *argCount)
{

	while (token != NULL)
	{
		arguements[*argCount] = token;
		(*argCount)++;
		token = strtok(NULL, " ");
	}
}
/**
  * create_newProcess - function that creates a fork
  * @arguements: Pointer to array of arguments
  * Return: void
  */
void create_newProcess(char *arguements[])
{
	pid_t newProcess;
	int status;
	char *full_path;

	newProcess = fork();

	if (newProcess == 0)
	{
		/*Full path to the command*/
		full_path = get_path(arguements[0]);

		if (full_path == NULL)
		{
			write(STDOUT_FILENO, arguements[0], _strlen(arguements[0]));
			write(STDOUT_FILENO, ": not found\n", 12);
			exit(EXIT_FAILURE);
		}
		if (execve(full_path, arguements, NULL) == -1)
		{
			perror("error");
			exit(EXIT_FAILURE);
		}
	}
	else if (newProcess == -1)
	{
		write(STDERR_FILENO, "Error (fork)", 12);
		exit(EXIT_FAILURE);
	}
	else
	{
		wait(&status);
	}
}
/**
 * exit_handler - Function that handles the exit builtin
 * @arguements: arguements passed to the exit builtin
 * Return: void
 */
void exit_handler(char **arguements, char *buffer)
{
	int exit_code;
	if (arguements[1] != NULL)
	{
		if (!is_valid_integer(arguements[1]))
		{
			write(STDOUT_FILENO, "./hsh: 1: exit: Illegal number: ", 32);
			write(STDOUT_FILENO, arguements[1], _strlen(arguements[1]));
			write(STDOUT_FILENO, "\n", 2);
			exit(2); /**Exit code for an illegal arguement*/
		}
		exit_code = _atoi(arguements[1]);
		if (exit_code < 0 || exit_code > 255)
		{
			write(STDOUT_FILENO, "exit: Illegal number: ", 22);
			write(STDOUT_FILENO, arguements[1], _strlen(arguements[1]));
			write(STDOUT_FILENO, "\n", 2);
			exit(2); /**Exit code for an illegal arguement*/
		}
		else
		{
			exit(exit_code);
		}
	}
	free(buffer);
	exit(0); /**Exit code if no arguement is passed*/
}
/**
  * main - Function that reads a line of text input from the stdin
  * and stores it as a string
  * @argc: number of aarguments passed
  * @argv: Pointer to array of arguments
  * @env_command: current environment
  * Return: 0 on success
  */
int main(int argc __attribute__((unused)),
		char *argv[] __attribute__((unused)), char **env_command)
{
	char *buffer = NULL, *prompt = "$ ", *arguements[1024], *token;
	int argCount;
	size_t n = 0;
	ssize_t read;
	bool interactive = isatty(STDIN_FILENO);
	while (1)
	{
		argCount = 0;
		if (interactive)
		{
			write(STDOUT_FILENO, prompt, 2);
			read = getline(&buffer, &n, stdin);
			if (read == -1)
			{
				write(STDERR_FILENO, "Exit", 4);
				write(STDERR_FILENO, "\n", 2);
				exit(EXIT_FAILURE);
			}
			else if (read == 1)
				continue;
		}
		else
		{
			read = getline(&buffer, &n, stdin);
			if (read == -1)
				break;
		}
		/* replace the new line with ndull terminator*/
		if (buffer[read - 1] == '\n')
			buffer[read - 1] = '\0';
		token = strtok(buffer, " ");
		/**Save commands to an array*/
		str_tok(token, arguements, &argCount);
		arguements[argCount] = NULL;
		/**Exit the shell if user enters the exit command*/
		if (s_strcmp(arguements[0], "exit") == 0)
		{
			exit_handler(arguements, buffer);
		}
		/**Prints current environment - builtin env*/
		if (s_strcmp(arguements[0], "env") == 0)
			printEnv(env_command);
		/**Create a fork */
		create_newProcess(arguements);
		argCount = 0;
	}
	free(buffer);
	return (0);
}
