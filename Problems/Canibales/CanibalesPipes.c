#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define PORCIONES 10
#define CANIBALES 5

// Pipes
int porciones[2], emptyPot[2], fullPot[2];

// Función del caníbal
void canibal(int id) {
    close(fullPot[1]); // Cierra el extremo de escritura de fullPot
    close(emptyPot[0]); // Cierra el extremo de lectura de emptyPot

    char c;
    while (1) {
        if (read(porciones[0], &c, sizeof(char)) == -1) { // Intenta tomar una porción
            // Si no hay porciones, avisa al cocinero
            write(emptyPot[1], "E", sizeof(char));
            printf("LLame al cocinero");
            read(fullPot[0], &c, sizeof(char)); // Espera que el cocinero llene la olla
        } else {
            printf("Caníbal %d comió una porción\n", id);
            write(porciones[1], "P", sizeof(char)); // Avisa que comió una porción
        }
    }
}

// Función del cocinero
void cocinero() {
    close(emptyPot[1]); // Cierra el extremo de escritura de emptyPot
    close(fullPot[0]); // Cierra el extremo de lectura de fullPot

    char c;
    while (1) {
        read(emptyPot[0], &c, sizeof(char)); // Espera que el caníbal avise que la olla está vacía
        printf("Cocinero llenando la olla...\n");
        for (int i = 0; i < PORCIONES; i++) {
            write(porciones[1], "P", sizeof(char)); // Llena una porción
            printf("Porción %d añadida\n", i + 1);
        }
        write(fullPot[1], "F", sizeof(char)); // Avisa que la olla está llena
    }
}

int main() {
    setbuf(stdout, NULL);

    // Crear pipes
    pipe(porciones);
    pipe(emptyPot);
    pipe(fullPot);

    write(emptyPot[1], "E", sizeof(char)); // Avisa que la olla está vacía

    // Crear procesos de caníbales
    for (int i = 0; i < CANIBALES; i++) {
        if (fork() == 0) {
            canibal(i);
            exit(0); // Asegura que el proceso hijo termine
        }
    }

    // Crear proceso del cocinero
    if (fork() == 0) {
        cocinero();
        exit(0); // Asegura que el proceso hijo termine
    }

    // Cierra los extremos de los pipes en el proceso padre
    close(porciones[0]);
    close(porciones[1]);
    close(emptyPot[0]);
    close(emptyPot[1]);
    close(fullPot[0]);
    close(fullPot[1]);

    // Espera a que terminen los procesos hijos
    for (int i = 0; i < CANIBALES + 1; i++) {
        wait(NULL);
    }

    return 0;
}