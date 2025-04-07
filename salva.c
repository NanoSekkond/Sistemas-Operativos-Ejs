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
	while(1) {
		printf("[%d] Voy a leer\n", getpid());
		read(fdEntrada[PIPE_READ], &entrada, sizeof(entrada));
		printf("[%d] Recibi %d\n", getpid(), entrada);
		entrada++;
		write(fdSalida[PIPE_WRITE], &entrada, sizeof(entrada));
	}
}

void hijoElegido(int* fdEntrada, int* fdSalida, int* fdPadre, int c) {
	int numeroSecreto;
	numeroSecreto = generate_random_number();

	printf("[%d] Soy el elegido, recibi %d, y genere %d\n", getpid(), c, numeroSecreto);
	int entrada = c;
	while(1) {
		entrada++;
		printf("[%d] Voy a escribir en %d\n", getpid(), fdSalida);
		write(fdSalida[PIPE_WRITE], &entrada, sizeof(entrada));
		printf("[%d] Voy a leer\n", getpid());
		read(fdEntrada[PIPE_READ], &entrada, sizeof(entrada));
		printf("[%d] Recibi %d\n", getpid(), entrada);
		if (entrada >= numeroSecreto) {
			write(fdPadre[PIPE_WRITE], &entrada, sizeof(entrada));
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

	for (int i = 0; i < n-1; i++) {
		pipe(fd[i]);
	}

	int fdInicialAPadre[2];
	pipe(fdInicialAPadre);

	int hijos[n];

	for (int i = 0; i < n; i++) {

		pid_t pidOrZero = fork();
		hijos[i] = pidOrZero;
		int prev_i = (i-1+n)%n;
		int next_i = (i+1) % n;
		// Si es hijo
		if (!pidOrZero) {
			if (i == start) {
				hijoElegido(fd[prev_i], fd[next_i], fdInicialAPadre, buffer);
			}
			else {
				hijo(fd[prev_i], fd[next_i]);
			}
		}
	}

	int resultado;
	read(fdInicialAPadre[PIPE_READ], &resultado, sizeof(resultado));
	printf("Resultado: %d\n", resultado);

    /* COMPLETAR */
}

