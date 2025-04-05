#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define READ 0
#define WRITE 1

int main(int argc, char **argv) {
    int n = 0;
    int fd[2][2];

    int readTarget = 0;
    int writeTarget = 1;

    pipe(fd[readTarget]);
    pipe(fd[writeTarget]);

    write(fd[writeTarget][WRITE], &n, sizeof(n));

    pid_t hijo = fork();

    if (!hijo) {
        readTarget = 1;
        writeTarget = 0;
    }

    while(1) {
        read(fd[readTarget][READ], &n, sizeof(n));
        printf("[%d] Got: %d Sending: %d\n", getpid(), n, n+1);
        n++;
        write(fd[writeTarget][WRITE], &n, sizeof(n));
    }

    return 0;
}