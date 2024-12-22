#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define sillas 5
#define clientesTotal 10

sem_t sillasEspera, sillaBarbero, semBarbero, cantidadClientes, controlCliente;


typedef struct {
    int id;
} cliente_t;


void * cliente(void * args){
    cliente_t * cliente = (cliente_t *)args;

    sem_wait(&controlCliente);//La modificacion de sillas se hace de a 1
        if(sem_trywait(&cantidadClientes) != 0)
            sem_post(&semBarbero);
        else
            sem_post(&cantidadClientes);
        if(sem_trywait(&sillasEspera) == 0) {

            sem_post(&cantidadClientes);
            printf("Cliente numero %d esperando en la sala de espera\n", cliente->id);
            fflush(stdout);
            sem_post(&controlCliente);//Libera el control de sillas

            sem_wait(&sillaBarbero);
            sem_post(&sillasEspera);
            printf("Cliente %d paso a cortarse el pelo\n", cliente->id);
            fflush(stdout);

            sem_wait(&controlCliente);
            if (sem_trywait(&cantidadClientes) != 0)
                sem_wait(&semBarbero);
            else
                sem_post(&cantidadClientes);
            sem_post(&controlCliente);
        }else{
            sem_post(&controlCliente);
            printf("Cliente %d se fue porque no habia lugar\n", cliente->id);
            fflush(stdout);
        }
    printf("Cliente %d se fue\n", cliente->id);
    fflush(stdout);
    pthread_exit(NULL);
}

void * peluqueria(void * args){
    int cortes = 0;
    while(1){
        fflush(stdout);

        if(sem_trywait(&cantidadClientes) != 0){
            printf("Barbero durmiendo\n");
            sem_wait(&cantidadClientes);
        }
        else
            sem_post(&cantidadClientes);
        printf("Barbero se desperto\n");
        fflush(stdout);

        sem_wait(&cantidadClientes);
        printf("Barbero cortando pelo\n");
        fflush(stdout);
        sleep(1);
        cortes++;
        printf("Barbero termino de cortar el pelo\n");
        fflush(stdout);
        sem_post(&sillaBarbero);
        printf("Barbero hizo %d cortes\n", cortes);
        fflush(stdout);
    }
}

int main(){
    sem_init(&sillasEspera, 0, sillas);
    sem_init(&sillaBarbero, 0, 1);
    sem_init(&semBarbero, 0, 0);
    sem_init(&cantidadClientes, 0, 0);
    sem_init(&controlCliente, 0, 1);

    pthread_t barbero;
    pthread_t clientes[clientesTotal];
    pthread_create(&barbero, NULL, peluqueria, NULL);

    sleep(1);

    setvbuf(stdout, NULL, _IONBF, 0);

    for(int i = 0; i < clientesTotal; i++){
        cliente_t * client = malloc(sizeof(cliente_t));
        client->id = i;
        pthread_create(&clientes[i], NULL, cliente, (void *)client);
    }

    for(int i = 0; i < clientesTotal; i++){
        pthread_join(clientes[i], NULL);
    }
    pthread_cancel(barbero);

    sem_destroy(&sillasEspera);
    sem_destroy(&sillaBarbero);
    sem_destroy(&semBarbero);
    sem_destroy(&cantidadClientes);
    sem_destroy(&controlCliente);

    printf("Cerro la pelu\n");

    return 0;

}