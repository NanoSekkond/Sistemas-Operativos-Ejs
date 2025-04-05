#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    pid_t childPid = fork();
    if (!childPid) {
        execvp(argv[1], &argv[1]);
    }
    wait();
    printf("The other program finished.\n");
    return 0;
}