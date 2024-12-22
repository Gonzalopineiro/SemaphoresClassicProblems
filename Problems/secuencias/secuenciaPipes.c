#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int pipeA[2], pipeB[2], pipeC[2];

void letraA() {
    close(pipeA[1]);  // Cerrar el extremo de escritura del pipeA
    close(pipeB[0]);  // Cerrar el extremo de lectura del pipeB
    char c;
    while (1) {
        read(pipeA[0], &c, sizeof(char));  // Leer de pipeA
        printf("A");
        fflush(stdout);
        write(pipeB[1], "B", sizeof(char));  // Escribir 'B' en pipeB
    }
}

void letraB() {
    close(pipeB[1]);  // Cerrar el extremo de escritura del pipeB
    close(pipeC[0]);  // Cerrar el extremo de lectura del pipeC
    char c;
    while (1) {
        for (int i = 0; i < 2; ++i) {
            read(pipeB[0], &c, sizeof(char));  // Leer de pipeB
            printf("B");
            fflush(stdout);
            write(pipeC[1], "C", sizeof(char));  // Escribir 'C' en pipeC
        }
        read(pipeB[0], &c, sizeof(char));  // Leer de pipeB
        write(pipeC[1], "C", sizeof(char));  // Escribir 'C' en pipeC
    }
}

void letraC() {
    close(pipeC[1]);  // Cerrar el extremo de escritura del pipeC
    close(pipeA[0]);  // Cerrar el extremo de lectura del pipeA
    char c;
    while (1) {
        read(pipeC[0], &c, sizeof(char));  // Leer de pipeC
        printf("C");
        fflush(stdout);
        write(pipeA[1], "A", sizeof(char));  // Escribir 'A' en pipeA
        read(pipeC[0], &c, sizeof(char));  // Leer de pipeB
        write(pipeA[1], "A", sizeof(char));
        read(pipeC[0], &c, sizeof(char));  // Leer de pipeC
        printf("C");
        fflush(stdout);
        sleep(2);
        write(pipeA[1], "A", sizeof(char));  // Escribir 'A' en pipeA
    }
}

int main() {
    // Inicio los pipes
    pipe(pipeA);
    pipe(pipeB);
    pipe(pipeC);

    // Escribo en el pipe A para comenzar la secuencia
    write(pipeA[1], "A", sizeof(char));

    // Creo los procesos hijos
    if (fork() == 0) {
        letraA();
    } else if (fork() == 0) {
        letraB();
    } else if (fork() == 0) {
        letraC();
    }

    // El proceso principal cierra los extremos de los pipes que no necesita
    close(pipeA[0]);
    close(pipeA[1]);
    close(pipeB[0]);
    close(pipeB[1]);
    close(pipeC[0]);
    close(pipeC[1]);

    // Esperar a que terminen los procesos hijos
    for (int i = 0; i < 3; ++i) {
        wait(NULL);
    }

    return 0;
}
