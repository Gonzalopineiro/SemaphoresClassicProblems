#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5
#define M 5

sem_t mutex, martilloOJabalina, corredor;

void * corredores(void *arg){
    int id = *(int *)arg;
    while(1){
        sem_wait(&mutex);
        if(sem_trywait(&corredor) == -1){
            printf("Lector %d esperando\n", id);
            fflush(stdout);
            sem_wait(&martilloOJabalina);
        }else
            sem_post(&corredor);
        sem_post(&corredor);
        sem_post(&mutex);


        //Region Critica
        printf("Lector %d leyendo\n", id);
        fflush(stdout);
        sleep(3);

        sem_wait(&mutex);
        sem_wait(&corredor);
        if(sem_trywait(&corredor) == -1){
            printf("Ultimo lector %d saliendo\n", id);
            fflush(stdout);
            sem_post(&martilloOJabalina);
        } else{
            printf("Lector %d saliendo \n", id);
            fflush(stdout);
            sem_post(&corredor);
        }
        sem_post(&mutex);
        sleep(3);
    }
}

void * escritores(void *arg){
    int id = *(int *)arg;
    while(1){
        printf("Escritor %d esperando\n", id);
        fflush(stdout);
        sem_wait(&martilloOJabalina);
        printf("Escritor %d escribiendo\n", id);
        fflush(stdout);
        sleep(3);
        sem_post(&martilloOJabalina);
        printf("Escritor %d saliendo\n", id);
        fflush(stdout);
    }
}

int main(){
    //Inicio los semaforos
    sem_init(&mutex, 0, 1);
    sem_init(&martilloOJabalina, 0, 1);
    sem_init(&corredor, 0, 0);


    pthread_t hilos[N+M];
    int id[N+M];
    for(int i = 0; i < N; i++){
        id[i] = i;
        pthread_create(&hilos[i], NULL, corredores, &id[i]);
    }

    for(int i = N; i < N+M; i++){
        id[i] = i;
        pthread_create(&hilos[i], NULL, escritores, &id[i]);
    }

    for(int i = 0; i < N+M; i++){
        pthread_join(hilos[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&martilloOJabalina);
    sem_destroy(&corredor);

    return 0;
}