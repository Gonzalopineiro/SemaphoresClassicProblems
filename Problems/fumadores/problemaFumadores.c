#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

sem_t semaforoAgente, tabaco, papel, cerillas;

void * fumadorTabaco(void * args){
    while(1){
        sem_wait(&tabaco);
        printf("Fumador con tabaco fumando\n");
        sleep(1);
        printf("Termino de fumar y le avisa al agente\n");
        sem_post(&semaforoAgente);
    }
}

void * fumadorPapel(void * args){
    while(1){
        sem_wait(&papel);
        printf("Fumador con papel fumando\n");
        sleep(1);
        printf("Termino de fumar y le avisa al agente\n");
        sem_post(&semaforoAgente);
    }
}

void * fumadorCerillas(void * args){
    while(1){
        sem_wait(&cerillas);
        printf("Fumador con cerillas fumando\n");
        sleep(1);
        printf("Termino de fumar y le avisa al agente\n");
        sem_post(&semaforoAgente);
    }
}

void * agente(void * args){
    srand(time(NULL));
    while(1){
        sem_wait(&semaforoAgente);
        printf("Agente elige dos ingredientes para poner en la mesa\n");
        int ingrediente = rand() % 3;
        switch(ingrediente){
            case 0:
                printf("Agente pone tabaco y cerillas\n");
                sem_post(&papel);
                break;
            case 1:
                printf("Agente pone papel y cerillas\n");
                sem_post(&tabaco);
                break;
            case 2:
                printf("Agente pone papel y tabaco\n");
                sem_post(&cerillas);
                break;
        }
    }
}


int main (){
    sem_init(&semaforoAgente, 0, 1);
    sem_init(&tabaco, 0, 0);
    sem_init(&papel, 0, 0);
    sem_init(&cerillas, 0, 0);

    pthread_t fumador1, fumador2, fumador3, agente1;
    pthread_create(&fumador1, NULL, fumadorTabaco, NULL);
    pthread_create(&fumador2, NULL, fumadorPapel, NULL);
    pthread_create(&fumador3, NULL, fumadorCerillas, NULL);
    pthread_create(&agente1, NULL, agente, NULL);

    pthread_join(fumador1, NULL);
    pthread_join(fumador2, NULL);
    pthread_join(fumador3, NULL);
    pthread_join(agente1, NULL);

    sem_destroy(&semaforoAgente);
    sem_destroy(&tabaco);
    sem_destroy(&papel);
    sem_destroy(&cerillas);
    return 0;
}