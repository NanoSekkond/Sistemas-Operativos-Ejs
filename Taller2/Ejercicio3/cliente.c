#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <signal.h>

int server_socket;

void endConnection() {
	printf("\n");
	close(server_socket);
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

	signal(SIGINT, endConnection);

	while (1) {
		printf("Escribi una expresion: ");
		char expresion[255];
		scanf("%s", &expresion);
		if (expresion == "exit") {
			endConnection();
		}
		send(server_socket, &expresion, sizeof(expresion), 0);

		int res;
		recv(server_socket, &res, sizeof(res), 0);
		printf("Cliente: recibí %d del servidor!\n", res);
	}
}