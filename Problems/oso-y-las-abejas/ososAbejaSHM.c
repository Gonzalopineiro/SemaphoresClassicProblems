#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>


#define cantConsumidores 1
#define cantProductores 1
#define cantItems 10

int * buffer;
int in = 0;
int puntero = 0;

sem_t *mutex, *items, *itemsConsumed;

void * producer(void * args){
    while(1){
        sem_wait(items);
        sem_wait(mutex);
        printf("Productor esta elaborando un item\n");
        int item = rand() % 100 +1;
        printf("Productor ha elaborado el item %d\n", item);
        buffer[(in + puntero) % cantItems] = item;
        in++;
        //Manejo para el item
        sleep(2);
        sem_post(mutex);
        sem_post(itemsConsumed);
        printf("Productor ha terminado de elaborar un item\n");
    }
}

void * consumer(void * args){
    while(1){
        sem_wait(itemsConsumed);
        sem_wait(mutex);
        printf("Consumidor esta consumiendo un item\n");
        int item = buffer[puntero];
        buffer[puntero] = 0;
        printf("Consumidor ha consumido el item %d\n", item);
        puntero = (puntero + 1) % cantItems;
        //Manejo para el item
        sem_post(mutex);
        sem_post(items);
        printf("Consumidor ha terminado de consumir un item\n");
    }
}

int main(){
    key_t key = ftok("shmfile", 65);
    int shid = shmget(key, sizeof(int)*cantItems, IPC_CREAT | 0666);
    buffer = (int *) shmat(shid, 0, 0);
    if(shid < 0){
        perror("shmget");
        return 1;
    }

    for (int i = 0; i < cantItems; ++i) {
        buffer[i] = 0;
    }

    mutex = sem_open("mutex", O_CREAT, 0666, 1);
    items = sem_open("items", O_CREAT, 0666, cantItems);
    itemsConsumed = sem_open("itemsConsumed", O_CREAT, 0666, 0);

    pid_t productores[cantProductores];
    pid_t consumidores[cantConsumidores];

    for(int i = 0; i < cantProductores; i++){
        if((productores[i] = fork()) == 0){
            producer(NULL);
        }
    }

    for(int i = 0; i < cantConsumidores; i++){
        if((consumidores[i] = fork()) == 0){
            consumer(NULL);
        }
    }

    for(int i = 0; i < cantProductores; i++){
        wait(NULL);
    }

    for(int i = 0; i < cantConsumidores; i++){
        wait(NULL);
    }
    if(shmdt(buffer) == -1){
        perror("shmdt");
        return 1;
    }
    if(shmctl(shid, IPC_RMID, NULL) == -1){
        perror("shmctl");
        return 1;
    }
    sem_unlink("mutex");
    sem_unlink("items");
    sem_unlink("itemsConsumed");


    return 0;
}