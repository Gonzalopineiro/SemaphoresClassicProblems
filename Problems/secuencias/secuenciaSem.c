#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

sem_t sem_A, sem_B, sem_C;

void * letraA(){
    while(1){
        sem_wait(&sem_A);
        printf("A");
        sem_post(&sem_B);
    }
}

void * letraB(){
    while(1){
        for (int i = 0; i < 2; ++i) {
            sem_wait(&sem_B);
            printf("B");
            sem_post(&sem_C);
        }
        sem_wait(&sem_B);
        sem_post(&sem_C);
    }
}

void * letraC(){
    while(1){
        sem_wait(&sem_C);
        printf("C");
        sem_post(&sem_A);
        sem_wait(&sem_C);
        sem_post(&sem_A);
        sem_wait(&sem_C);
        printf("C");
        sleep(2);
        sem_post(&sem_A);
    }
}

//Secuencia ABCABAC
int main(){
    setbuf(stdout, NULL);
    sem_init(&sem_A, 0, 1);
    sem_init(&sem_B, 0, 0);
    sem_init(&sem_C, 0, 0);

    pthread_t hiloA, hiloB, hiloC;
    pthread_create(&hiloA, NULL, letraA, NULL);
    pthread_create(&hiloB, NULL, letraB, NULL);
    pthread_create(&hiloC, NULL, letraC, NULL);

    pthread_join(hiloA, NULL);
    pthread_join(hiloB, NULL);
    pthread_join(hiloC, NULL);

    sem_destroy(&sem_A);
    sem_destroy(&sem_B);
    sem_destroy(&sem_C);

    return 0;
}