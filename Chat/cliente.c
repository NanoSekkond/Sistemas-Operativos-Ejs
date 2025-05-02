#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <signal.h>

int server_socket;
pid_t pidOrZero;

void endConnection() {
	close(server_socket);
    kill(pidOrZero, 9);
    printf("\n");
	exit(0);
}

void cliente_loop() {
	signal(SIGINT, endConnection);
    while (1) {
		char mensaje[256];
		fgets(mensaje, sizeof(mensaje), stdin);
		write(server_socket, mensaje, sizeof(mensaje));
	}
}

void leer_server() {
    char mensaje[256];
    while (read(server_socket, &mensaje, sizeof(mensaje))) {
        printf("%s", mensaje);
    }
    printf("No me acepto el server\n");
    kill(getppid(), 9);
    exit(0);
}

int main() {
    struct sockaddr_un server_addr;

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "unix_socket");

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error");
        exit(1);
    }

    pidOrZero = fork();

    if (!pidOrZero) {
        leer_server();
    }

    cliente_loop();
}