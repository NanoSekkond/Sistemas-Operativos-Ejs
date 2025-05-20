#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

void rvrstr(char* input) {
    int length = strlen(input);
    for (int i = 0; i < length / 2 + 1; i++) {
        char temp = toupper(input[i]);
        input[i] = toupper(input[length - i - 1]);
        input[length - i - 1] = temp;
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Input should look like this: ./Ejercicio10 4 'Hola que tal como que va'\n");
        exit(1);
    }

    int n = atoi(argv[1]);
    char* text = strdup(argv[2]);
    char* word = strtok(text, " ");
    char* res = malloc(strlen(text) * sizeof(char));

    // Walk through other tokens
    while (word != NULL) {
        if (strlen(word) < n) {
            rvrstr(word);
        }
        strcat(res, word);
        strcat(res, " ");
        word = strtok(NULL, " ");
    }
    printf("%s\n", res);

    exit(0);
}