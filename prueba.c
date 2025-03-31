#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define PIPE_READ 0
#define PIPE_WRITE 1
#define STD_INPUT 0
#define STD_OUTPUT 1
#define VIVO -1

int n;
int indice;
int numero_maldito;
int pipe_fd[2];

int generate_random_number(){
    //Funcion para cargar nueva semilla para el numero aleatorio
    srand(time(NULL) ^ getpid());
    return (rand() % n);
}

void jugar(){
    int estoyVivo = VIVO;
    int randomN = generate_random_number();
    printf("[%d] Estoy jugando y me salio %d\n", indice, randomN);
    if (randomN == numero_maldito){
        printf("[%d] Estas son mis ultimas palabras\n", indice);
        write(pipe_fd[PIPE_WRITE], &indice, sizeof(indice));
        exit(EXIT_SUCCESS);
    }
    else {
        write(pipe_fd[PIPE_WRITE], &estoyVivo, sizeof(indice));
    }
}

int main(int argc, char const *argv[]){
    n = atoi(argv[1]);
	int rondas = atoi(argv[2]);
	numero_maldito = atoi(argv[3]);

    pid_t hijos[n];
    pipe(pipe_fd);

    for (int i = 0; i < n; i++){
        signal(SIGTERM, jugar);
        indice = i;
        printf("Procesando indice %d\n", indice);
        pid_t pidOrZero = fork();
        

        // Si es el padre
        if (pidOrZero != 0) {
            hijos[i] = pidOrZero;
        } else {  // Si soy hijo.
            while(1){
                pause();
            }
        }
    }

    int cantidad_hijos_vivos = n;

    for (int k = 0; k < rondas; k++) {
        printf("Empieza ronda %d\n", k);
        for(int i = 0; i < n; i++){
            if(hijos[i] != -1) {  // Si el hijo esta vivo.
                sleep(1);
                kill(hijos[i], SIGTERM);
            }
        }
        int hijos_vivos_pre_ronda = cantidad_hijos_vivos;
        for (int i = 0; i < hijos_vivos_pre_ronda; i++) {
            int resultado;
            read(pipe_fd[PIPE_READ], &resultado, sizeof(int));
            if (resultado != VIVO) {
                hijos[resultado] = -1;
                cantidad_hijos_vivos--;
            }
        }
    }

    for(int i = 0; i<n; i++){
        if(hijos[i] != -1){
            printf("Sobrevivio indice: %d con PID: %d\n", i, hijos[i]);
            kill(hijos[i], 9);
        }
    }

    for(int i=0; i<n; i++){
        wait(NULL);
    }

    exit(0);
}

