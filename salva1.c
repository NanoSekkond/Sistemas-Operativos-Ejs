#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

void hijo(char **programa) {
	execvp(programa[0], programa);
}

static int run(char ***progs, size_t count)
{	
	int r, status;

	//Reservo memoria para el arreglo de pids
	//TODO: Guardar el PID de cada proceso hijo creado en children[i]
	pid_t *children = malloc(sizeof(*children) * count);

	//TODO: Pensar cuantos procesos necesito. Necesito count procesos.


	//TODO: Pensar cuantos pipes necesito. count - 1
	int fd[count - 1][2];

	for (int i = 0; i < count - 1; i++) {
		pipe(fd[i]);
	}

	//TODO: Para cada proceso hijo:
			//1. Redireccionar los file descriptors adecuados al proceso
			//2. Ejecutar el programa correspondiente
	
	for (int i = 0; i < count; i++) {
		pid_t pidOrZero = fork();
		children[i] = pidOrZero;
		// Si es hijo
		if (!pidOrZero) {
			for (int j = 0; j < count; j++) {
				if (j != i) {
					if (j > 0) {
						close(fd[j - 1][PIPE_READ]);
					}
					if (j < count - 1) {
						close(fd[j][PIPE_WRITE]);
					}
				}
			}
			if (i > 0) {
				dup2(fd[i - 1][PIPE_READ], STD_INPUT);
			}
			if (i < count - 1) {
				dup2(fd[i][PIPE_WRITE], STD_OUTPUT);
			}
			hijo(progs[i]);
		}
	}

	for (int i = 0; i < count - 1; i++) {
		close(fd[i][PIPE_READ]);
		close(fd[i][PIPE_WRITE]);
	}

	//Espero a los hijos y verifico el estado que terminaron
	for (int i = 0; i < count; i++) {
		waitpid(children[i], &status, 0);

		if (!WIFEXITED(status)) {
			fprintf(stderr, "proceso %d no terminó correctamente [%d]: ",
			    (int)children[i], WIFSIGNALED(status));
			perror("");
			return -1;
		}
	}
	r = 0;
	free(children);

	return r;
}


int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("El programa recibe como parametro de entrada un string con la linea de comandos a ejecutar. \n"); 
		printf("Por ejemplo ./mini-shell 'ls -a | grep anillo'\n");
		return 0;
	}
	int programs_count;
	char*** programs_with_parameters = parse_input(argv, &programs_count);

	printf("status: %d\n", run(programs_with_parameters, programs_count));

	fflush(stdout);
	fflush(stderr);

	return 0;
}

