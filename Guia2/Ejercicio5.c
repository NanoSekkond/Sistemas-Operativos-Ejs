#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>

int main() {
    printf("Abraham\n");
    pid_t homero = fork();

    // Si es hijo
    if (!homero) {
        printf("Homero\n");

        pid_t bart = fork();
        if (!bart) {
            printf("Bart\n");
            printf("Soy Bart y me voy a matar\n");
            return 0;
        }

        pid_t lisa = fork();
        if (!lisa) {
            printf("Lisa\n");
            printf("Soy Lisa y me voy a matar\n");
            return 0;
        }

        pid_t maggie = fork();
        if (!maggie) {
            printf("Maggie\n");
            printf("Soy Maggie y me voy a matar\n");
            return 0;
        }
        waitpid(bart);
        waitpid(lisa);
        waitpid(maggie);

        printf("Soy Homero y me voy a matar\n");
        return 0;
    }
    waitpid(homero);
    printf("Soy Abraham y me voy a matar\n");
    return 0;
}