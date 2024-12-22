#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <stdlib.h>

#define mutex 5
#define escritor 6
#define lector 7

#define N 5
#define M 5

struct msg{
    long type;
};

void * lectores(void *arg){
    int id = *(int *)arg;
    key_t key = ftok("/tmp", 'D');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg msg;

    while(1){
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), mutex, 0);
        if(msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), lector, IPC_NOWAIT) == -1){
            printf("Lector %d esperando\n", id);
            fflush(stdout);
            msg.type = escritor;
            msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), escritor, 0);
        }else{
            msg.type = lector;
            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        }
        msg.type = lector;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        msg.type = mutex;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);

        //Region Critica
        printf("Lector %d leyendo\n", id);
        fflush(stdout);
        sleep(3);

        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), mutex, 0);
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), lector, 0);
        if(msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), lector, IPC_NOWAIT) == -1){
            printf("Ultimo lector %d saliendo\n", id);
            fflush(stdout);
            msg.type = escritor;
            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        } else{
            printf("Lector %d saliendo \n", id);
            fflush(stdout);
            msg.type = lector;
            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        }
        msg.type = mutex;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        sleep(3);
    }
}

void * escritores(void *arg){
    int id = *(int *)arg;
    key_t key = ftok("/tmp", 'D');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg msg;
    while(1){
        printf("Escritor %d esperando\n", id);
        fflush(stdout);
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), escritor, 0);
        printf("Escritor %d escribiendo\n", id);
        fflush(stdout);
        sleep(3);
        msg.type = escritor;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        printf("Escritor %d saliendo\n", id);
        fflush(stdout);
    }
}

int main(){
    //Inicializo la estructura de mensajes
    key_t key = ftok("/tmp", 'D');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg msg;

    //Inicio los semaforos
    msg.type = mutex;
    msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
    msg.type = escritor;
    msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);

    setvbuf(stdout, NULL, _IONBF, 0);

    for(int i = 0; i < N; i++){
        if(fork() == 0){
            lectores(&i);
        }
    }
    for(int i = N; i < N+M; i++){
        if(fork() == 0){
            escritores(&i);
        }
    }

    for(int i = 0; i < N+M; i++){
        wait(NULL);
    }

    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar la cola de mensajes");
        exit(1);
    }

    return 0;
}