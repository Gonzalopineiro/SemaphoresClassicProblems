//inlcuir librerias estandarm, de semaforos y threads
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

//declarar semaforos
sem_t semAB, semC, semD, semE;

//secuencia
// (A o B) CE (A o B)(A o B) DE 

 //A
void *A(void *arg){
    while(1){
        sem_wait(&semAB);
        sem_wait(&semAB);
        printf("A");
        sleep(1);
        sem_post(&semC);
        sem_post(&semD);
    }
}

//B
void *B(void *arg){
    while(1){
        sem_wait(&semAB);
        sem_wait(&semAB);
        printf("B");
        sleep(1);
        sem_post(&semC);
        sem_post(&semD);
    }
}


//C
void *C(void *arg){
    while(1){
        sem_wait(&semC);
        printf(" C");
        sleep(1);
        sem_post(&semE);
        sem_wait(&semC);
        sem_wait(&semC);
    }
}

//D
void *D(void *arg){
    while(1){
        sem_wait(&semD);
        sem_wait(&semD);
        sem_wait(&semD);
        printf(" D");
        sleep(1);
        sem_post(&semE);
    }
}

//E
void *E(void *arg){
    while(1){
        sem_wait(&semE);
        printf("E ");
        sleep(1);
        sem_post(&semAB);
        sem_post(&semAB);
        sem_post(&semAB);
    }
}


//main
int main(){
    setbuf(stdout, NULL);
    //inicializar semaforos
    sem_init(&semAB, 0, 3);
    sem_init(&semC, 0, 0);
    sem_init(&semD, 0, 0);
    sem_init(&semE, 0, 0);

    //declarar threads
    pthread_t threadA, threadB, threadC, threadD, threadE;

    //crear threads
    pthread_create(&threadA, NULL, A, NULL);
    pthread_create(&threadB, NULL, B, NULL);
    pthread_create(&threadC, NULL, C, NULL);
    pthread_create(&threadD, NULL, D, NULL);
    pthread_create(&threadE, NULL, E, NULL);

    //join threads
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);
    pthread_join(threadD, NULL);
    pthread_join(threadE, NULL);

    //destruir semaforos
    sem_destroy(&semAB);
    sem_destroy(&semC);
    sem_destroy(&semD);
    sem_destroy(&semE);

    return 0;
}