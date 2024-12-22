#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_SILLAS 8

sem_t semSillasVacias, semSillasOcupadas, semInicioTurno, semFinTurno;

void* cliente(int nro_cli){

    //sleep(1);

    if(sem_trywait(&semSillasVacias)==0){
        printf("Cliente %d: ocupe silla\n", nro_cli);
        sem_post(&semSillasOcupadas);
        sem_wait(&semInicioTurno); //espera su turno
        printf("Cliente %d: me estoy cortando el pelo\n");
        sem_wait(&semFinTurno);
        sem_post(&semSillasVacias);
        printf("Cliente %d: termine de cortarme el pelo, me fui\n");
        
    }else{
        printf("Cliente %d: no hay sillas libres, me fui\n", nro_cli);
    }

    pthread_exit(NULL);
}

void* barbero(){
    while(1){

        sem_wait(&semSillasOcupadas);
        sem_post(&semInicioTurno);
        printf("Barbero: estoy trabajando.\n");
        sleep(2); //esta trabajando
        printf("Barbero: termine de cortar el pelo.\n");
        sem_post(&semFinTurno);

        sleep(1);//descansa
    }
}

int main(int argc, char* argv[]){
    int num=0;

    srand(time(NULL));

    sem_init(&semInicioTurno, 0, 0);
    sem_init(&semFinTurno, 0, 0);
    sem_init(&semSillasVacias, 0, NUM_SILLAS);
    sem_init(&semSillasOcupadas, 0, 0);

    //creacion de threads
    pthread_t th_barbero;
    pthread_create(&th_barbero, NULL, (void*)&barbero, NULL); //thread barbero
    
    while(1){
        sleep(1);
        num++;

        pthread_t th_cliente;
        pthread_create(&th_cliente, NULL, (void*)&cliente, num); //threads clientes

        //printf("cree hilo %d\n",num);
    }
        
    

}