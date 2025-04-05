#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int count = 0;

char** params;

void respuesta() {
	count++;
	printf("ya va!\n");
	if (count == 5) {
		kill(getppid(), SIGINT);
		execvp(params[0],params);
	}
}

void esperaSuHijo() {
	wait(NULL);
	exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
	//pid_t padre = getpid();
	pid_t hijo = fork();
	params = argv + 1;

	if (hijo == 0){  // Si soy el hijo
		signal(SIGURG, respuesta);
		while(1){
			//pause();
		}
	}

	signal(SIGINT, esperaSuHijo);

	for (int i = 0; i < 5; i++) {
		sleep(1);
		printf("sup!\n");
		kill(hijo, SIGURG);
	}

	while(1){
		
	}

	return 0;
}

