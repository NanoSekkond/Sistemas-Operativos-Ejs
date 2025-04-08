#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

int generate_random_number(){
	return (rand() % 50);
}

void hijo(int* fdEntrada, int* fdSalida) {
	int entrada;
	while(read(fdEntrada[PIPE_READ], &entrada, sizeof(entrada))) {
		//printf("[%d] Voy a leer desde: %d\n", getpid(), fdEntrada[PIPE_READ]);
		printf("[%d] Recibi %d\n", getpid(), entrada);
		entrada++;
		write(fdSalida[PIPE_WRITE], &entrada, sizeof(entrada));
	}
	printf("[%d] no me va a llegar nada, chau\n", getpid());
	close(fdSalida[PIPE_WRITE]);
	exit(0);
}

void hijoElegido(int* fdEntrada, int* fdSalida, int* fdPadre, int c) {
	int numeroSecreto;
	numeroSecreto = generate_random_number();

	printf("[%d] Soy el elegido, recibi: %d, y genere el numero secreto: %d\n", getpid(), c, numeroSecreto);
	int entrada = c;
	while(1) {
		entrada++;
		write(fdSalida[PIPE_WRITE], &entrada, sizeof(entrada));
		read(fdEntrada[PIPE_READ], &entrada, sizeof(entrada));
		printf("[%d] Recibi %d\n", getpid(), entrada);
		if (entrada >= numeroSecreto) {
			write(fdPadre[PIPE_WRITE], &entrada, sizeof(entrada));
			close(fdSalida[PIPE_WRITE]);
			exit(0);
		}
	}
}

int main(int argc, char **argv)
{	
	//Funcion para cargar nueva semilla para el numero aleatorio
	srand(time(NULL));

	int status, pid, n, start, buffer;
	n = atoi(argv[1]);
	buffer = atoi(argv[2]);
	start = atoi(argv[3]);

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
  	/* COMPLETAR */
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start);
    
	int fd[n][2];

	for (int i = 0; i < n; i++) {
		pipe(fd[i]);
	}

	int fdInicialAPadre[2];
	pipe(fdInicialAPadre);

	for (int i = 0; i < n; i++) {

		pid_t pidOrZero = fork();
		int next_i = (i+1) % n;
		// Si es hijo
		if (!pidOrZero) {
			for(int j = 0; j < n; j++) {
				if (next_i != j) {
					close(fd[j][PIPE_WRITE]);
				}
			}
			if (i == start) {
				hijoElegido(fd[i], fd[next_i], fdInicialAPadre, buffer);
			}
			else {
				hijo(fd[i], fd[next_i]);
			}
		}
	}

	for(int i = 0; i < n; i++) {
		close(fd[i][PIPE_WRITE]);
	}

	for (int i = 0; i < n; i++) {
		wait(NULL);
	}

	int resultado;
	read(fdInicialAPadre[PIPE_READ], &resultado, sizeof(resultado));
	printf("Resultado: %d\n", resultado);

	exit(0);
    /* COMPLETAR */
}
