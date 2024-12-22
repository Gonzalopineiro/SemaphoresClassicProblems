#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define CONSUMIDORES 5
#define PRODUCTORES 3

#define TAM_BUFFER 5

int buffer[TAM_BUFFER];
int in = 0;
int out = 0;

sem_t mutex, full, empty;

void *productor(void *arg) {
    while (1) {
        sem_wait(&empty);
        sem_wait(&mutex);

        buffer[in] = rand() % 100;
        printf("Productor produce %d\n", buffer[in]);
        fflush(stdout);
        in = (in + 1) % TAM_BUFFER;
        fflush(stdout);

        sem_post(&mutex);
        sem_post(&full);
    }
}

void *consumidor(void *arg) {
    while (1) {
        sem_wait(&full);
        sem_wait(&mutex);

        printf("Consumidor consume %d\n", buffer[out]);
        fflush(stdout);
        out = (out + 1) % TAM_BUFFER;

        sem_post(&mutex);
        sem_post(&empty);
    }
}

int main(){
    pthread_t productores[PRODUCTORES], consumidores[CONSUMIDORES];
    int i;

    srand(time(NULL));

    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, TAM_BUFFER);

    for (i = 0; i < PRODUCTORES; i++) {
        pthread_create(&productores[i], NULL, productor, (void *) i);
    }

    for (i = 0; i < CONSUMIDORES; i++) {
        pthread_create(&consumidores[i], NULL, consumidor, (void *) i);
    }

    for (i = 0; i < PRODUCTORES; i++) {
        pthread_join(productores[i], NULL);
    }

    for (i = 0; i < CONSUMIDORES; i++) {
        pthread_join(consumidores[i], NULL);
    }

    return 0;
}