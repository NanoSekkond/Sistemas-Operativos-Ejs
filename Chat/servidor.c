#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define PIPE_READ 0
#define PIPE_WRITE 1
#define CAPACIDAD 5

struct {
    int id;
    char buffer[256];
} typedef message;

int client_sockets[CAPACIDAD] = {-1, -1, -1, -1, -1};
int pipes[2];

int hay_espacio(int client_socket) {
    for (int i = 0; i < CAPACIDAD; i++) {
        if (client_sockets[i] == -1) {
            client_sockets[i] = client_socket;
            return 1;
        }
    }
    return 0;
}

void atender_cliente(int client_socket) {
    char buffer[256];
    while (read(client_socket, &buffer, sizeof(buffer))) {
        message msg;
        msg.id = client_socket;
        strcpy(msg.buffer, buffer);
        write(pipes[PIPE_WRITE], &msg, sizeof(msg));
        kill(getppid(), SIGUSR1);
    }
    close(client_socket);
    exit(0);
}

void broadcast_message(int sig) {
    message msg;
    read(pipes[PIPE_READ], &msg, sizeof(msg));
    for (int i = 0; i < CAPACIDAD; i++) {
        if (client_sockets[i] != -1 && msg.id != client_sockets[i]) {
            write(client_sockets[i], msg.buffer, sizeof(msg.buffer));
        }
    }
}

void server_loop(int server_socket) {
    int client_socket;
    pipe(pipes);
    signal(SIGUSR1, broadcast_message);
    while(1) {
        client_socket = accept(server_socket, NULL, NULL);
        if (hay_espacio(client_socket)) {
            pid_t pidOrZero = fork();
            if (!pidOrZero) {
                atender_cliente(client_socket);
            }
        }
        else {
            close(client_socket);
        }
    }
}

int main() {
    int server_socket;

    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;

    int slen = sizeof(server_addr);
    int clen = sizeof(client_addr);

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "unix_socket");
    unlink(server_addr.sun_path);

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    bind(server_socket, (struct sockaddr *) &server_addr, slen);
    listen(server_socket, 1);

    server_loop(server_socket);

    exit(0);
}