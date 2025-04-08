#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int calcular(const char *expresion) {
    int num1, num2, resultado;
    char operador;

    // Usamos sscanf para extraer los dos números y el operador de la expresión
    if (sscanf(expresion, "%d%c%d", &num1, &operador, &num2) != 3) {
        printf("Formato incorrecto\n");
        return 0;  // En caso de error, retornamos 0.
    }

    // Realizamos la operación según el operador
    switch (operador) {
        case '+':
            resultado = num1 + num2;
            break;
        case '-':
            resultado = num1 - num2;
            break;
        case '*':
            resultado = num1 * num2;
            break;
        case '/':
            if (num2 != 0) {
                resultado = num1 / num2;
            } else {
                printf("Error: División por cero\n");
                return 0;  // Si hay división por cero, retornamos 0.
            }
            break;
        default:
            printf("Operador no reconocido\n");
            return 0;  // Si el operador no es válido, retornamos 0.
    }

    return resultado;
}

void atenderCliente(int client_socket) {
    char expresion[255]; 
    printf("[%d] Arranco mi cliente\n", getpid());
    while (recv(client_socket, &expresion, sizeof(expresion), 0)) {
        printf("[%d] Servidor: recibí %s del cliente!\n", getpid() ,expresion);
    
        int resultado = calcular(expresion);
        printf("El resultado de la operación es: %d\n", resultado);
        
        send(client_socket, &resultado, sizeof(resultado), 0);
    }
    printf("[%d] Ya termino mi cliente\n", getpid());
    exit(0);
}

int main() {
     
    // COMPLETAR. Este es un ejemplo de funcionamiento básico.
    // La expresión debe ser recibida como un mensaje del cliente hacia el servidor.
    // Seteamos el socket
    int server_socket;
    int client_socket;

    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;

    uint slen = sizeof(server_addr);
    uint clen = sizeof(client_addr);

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "unix_socket");
    unlink(server_addr.sun_path);

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0); // Se crea el socket
    bind(server_socket, (struct sockaddr *) &server_addr, slen); // Se bindea el socket al socket address
    listen(server_socket, 1); // Se setea para que quede escuchando pasivamente

    printf("Server listening...\n");
    while(1) {
        client_socket = accept(server_socket, NULL, NULL); // Bloquea hasta tener una conexión
        pid_t pidOrZero = fork();
        // Si es hijo
        if (!pidOrZero) {
            atenderCliente(client_socket);
        }
    }
    exit(0);
}

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

