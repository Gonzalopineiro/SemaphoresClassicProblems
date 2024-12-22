#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <pthread.h>

#define mutex 11
#define full 10
#define empty 9

#define CONSUMIDORES 5
#define PRODUCTORES 3

#define TAM_BUFFER 5

// Estructura de la memoria compartida
struct shared_memory {
    int buffer[TAM_BUFFER];  // Buffer circular
    int in;                  // Índice para producir
    int out;                 // Índice para consumir
};

struct mensaje {
    long type;
};

void *productor(void *shm_ptr) {
    key_t key = ftok("/tmp", 'D');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct mensaje msg;

    struct shared_memory *shared = (struct shared_memory *)shm_ptr;

    while (1) {
        // Espera hasta que haya espacio vacío (empty) y acceso (mutex)
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), empty, 0);
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), mutex, 0);

        // Produce un valor aleatorio
        shared->buffer[shared->in] = rand() % 100;
        printf("Productor produce %d\n", shared->buffer[shared->in]);
        shared->in = (shared->in + 1) % TAM_BUFFER;
        fflush(stdout);
        sleep(1);

        // Libera el semáforo de mutex y marca que hay un nuevo ítem (full)
        msg.type = mutex;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        msg.type = full;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
    }
}

void *consumidor(void *shm_ptr) {
    key_t key = ftok("/tmp", 'D');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct mensaje msg;

    struct shared_memory *shared = (struct shared_memory *)shm_ptr;

    while (1) {
        // Espera hasta que haya ítems llenos (full) y acceso (mutex)
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), full, 0);
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), mutex, 0);

        // Consume el valor del buffer
        printf("Consumidor consume %d\n", shared->buffer[shared->out]);
        shared->out = (shared->out + 1) % TAM_BUFFER;
        sleep(1);

        // Libera el semáforo de mutex y marca que hay un nuevo espacio (empty)
        msg.type = mutex;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        msg.type = empty;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
    }
}

int main() {
    key_t key = ftok("/tmp", 'D');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct mensaje msg;

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

    srand(time(NULL));

    // Inicializar los semáforos mutex y empty en la cola de mensajes
    msg.type = mutex;
    msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);  // Inicializar mutex a 1

    // Inicializar empty con el tamaño del buffer
    msg.type = empty;
    for (int j = 0; j < TAM_BUFFER; ++j) {
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);  // Enviar empty TAM_BUFFER veces
    }

    // Crear procesos productores
    for (int i = 0; i < PRODUCTORES; i++) {
        if (fork() == 0) {
            productor((void *)shm_ptr);  // Inicia el productor en el proceso hijo
            exit(0);
        }
    }

    // Crear procesos consumidores
    for (int i = 0; i < CONSUMIDORES; i++) {
        if (fork() == 0) {
            consumidor((void *)shm_ptr);  // Inicia el consumidor en el proceso hijo
            exit(0);
        }
    }

    // Esperar a que todos los procesos terminen
    for (int i = 0; i < PRODUCTORES + CONSUMIDORES; i++) {
        wait(NULL);  // Espera que cada proceso hijo termine
    }

    // Desvincular y eliminar la memoria compartida
    shmdt(shm_ptr);
    shmctl(shm_id, IPC_RMID, NULL);

    // Limpiar la cola de mensajes
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}
