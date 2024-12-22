#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <pthread.h>

#define CONSUMIDORES 5
#define PRODUCTORES 3

#define TAM_BUFFER 5

struct shared_memory {
    int buffer[TAM_BUFFER];
    int in;
    int out;
};

int mutex[2], full[2], empty[2];

void *productor(void *shm_ptr) {
    struct shared_memory *shared = (struct shared_memory *)shm_ptr;
    char c;
    close(empty[1]);
    close(full[0]);
    while (1) {
        read(empty[0], &c, sizeof(char));
        read(mutex[0], &c, sizeof(char));

        shared->buffer[shared->in] = rand() % 100;
        printf("Productor produce %d\n", shared->buffer[shared->in]);
        shared->in = (shared->in + 1) % TAM_BUFFER;
        fflush(stdout);
        sleep(1);

        write(mutex[1], "M", sizeof(char));
        write(full[1], "F", sizeof(char));
    }
}

void *consumidor(void *shm_ptr) {
    struct shared_memory *shared = (struct shared_memory *)shm_ptr;
    char c;
    close(empty[0]);
    close(full[1]);
    while (1) {
        read(full[0], &c, sizeof(char));
        read(mutex[0], &c, sizeof(char));

        printf("Consumidor consume %d\n", shared->buffer[shared->out]);
        shared->out = (shared->out + 1) % TAM_BUFFER;
        sleep(1);

        write(mutex[1], "M", sizeof(char));
        write(empty[1], "E", sizeof(char));
    }
}

int main(){
    // Crear la memoria compartida
    int shm_id = shmget(IPC_PRIVATE, sizeof(struct shared_memory), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Error creando la memoria compartida");
        exit(1);
    }

    // Mapeo de la memoria compartida
    struct shared_memory *shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("Error mapeando la memoria compartida");
        exit(1);
    }

    // Inicializar el buffer y los índices compartidos
    shm_ptr->in = 0;
    shm_ptr->out = 0;

    pipe(mutex);
    pipe(full);
    pipe(empty);

    write(mutex[1], "M", sizeof(char));
    for (int i = 0; i < TAM_BUFFER; ++i) {
        write(empty[1], "E", sizeof(char));
    }

    srand(time(NULL));

    for (int i = 0; i < PRODUCTORES; i++) {
        if (fork() == 0) {
            productor((void *)shm_ptr);
        }
    }

    for (int i = 0; i < CONSUMIDORES; i++) {
        if (fork() == 0) {
            consumidor((void *)shm_ptr);
        }
    }

    close(mutex[0]);
    close(mutex[1]);
    close(full[0]);
    close(full[1]);
    close(empty[0]);
    close(empty[1]);

    for (int i = 0; i < PRODUCTORES + CONSUMIDORES; i++) {
        wait(NULL);
    }

    shmdt(shm_ptr);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}