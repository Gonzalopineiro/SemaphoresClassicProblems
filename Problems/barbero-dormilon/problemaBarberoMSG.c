#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <wait.h>

#define sillasEspera 1
#define clientesTotal 10
#define sillaBarbero 3
#define barbero 4
#define controlCliente 5
#define cantidadClientes 6

struct mensaje {
    long mtype;
} ;

void clientes(){
    key_t key = ftok("/tmp", 'A');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct mensaje msg;

    // Espera por el control del cliente
    msgrcv(msgid, &msg, sizeof(msg)- sizeof(long ), controlCliente, 0);

    // Verifica si hay clientes en espera
    if(msgrcv(msgid, &msg, sizeof(msg)- sizeof(long ), cantidadClientes, IPC_NOWAIT) == -1) {
        // Si no hay clientes, despierta al barbero
        msg.mtype = barbero;
        msgsnd(msgid, &msg, sizeof(msg)- sizeof(long ), 0);
    }else{
        // Si hay clientes, actualiza la cantidad de clientes
        msg.mtype = cantidadClientes;
        msgsnd(msgid, &msg, sizeof(msg)- sizeof(long ), 0);
    }

    // Verifica si hay sillas de espera disponibles
    if(msgrcv(msgid, &msg, sizeof(msg)- sizeof(long ), sillasEspera, IPC_NOWAIT) != -1) {
        // Si hay sillas, actualiza la cantidad de clientes y libera el control del cliente
        msg.mtype = cantidadClientes;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        msg.mtype = controlCliente;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);

        // Espera a que la silla del barbero esté disponible
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), sillaBarbero, 0);
        // Libera una silla de espera
        msg.mtype = sillasEspera;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        printf("Cliente %d paso a cortarse el pelo\n", getpid());
        fflush(stdout);

        // Espera por el control del cliente
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), controlCliente, 0);
        // Verifica si hay clientes en espera
        if (msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), cantidadClientes, IPC_NOWAIT) == -1) {
            // Si no hay clientes, espera al barbero
            msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), barbero, 0);
        } else {
            // Si hay clientes, actualiza la cantidad de clientes
            msg.mtype = cantidadClientes;
            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        }
        // Libera el control del cliente
        msg.mtype = controlCliente;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
    } else{
        // Si no hay sillas de espera disponibles, libera el control del cliente
        msg.mtype = controlCliente;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        printf("Cliente %d se fue porque no habia lugar\n", getpid());
        fflush(stdout);
    }
    printf("Cliente %d se fue\n", getpid());
    fflush(stdout);
    exit(0);
}

void peluqueria(){
    int cortes = 1;
    key_t key = ftok("/tmp", 'A');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct mensaje msg;
    while(1) {
        // Verifica si hay clientes en espera
        if (msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), cantidadClientes, IPC_NOWAIT) == -1) {
            printf("Barbero durmiendo\n");
            // Espera a que lleguen clientes
            msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), cantidadClientes, 0);
            fflush(stdout);
        } else {
            // Si hay clientes, actualiza la cantidad de clientes
            msg.mtype = cantidadClientes;
            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        }
        printf("Barbero se desperto\n");
        fflush(stdout);

        // Espera a que lleguen clientes
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), cantidadClientes, 0);
        printf("Barbero cortando pelo\n");
        fflush(stdout);
        sleep(1);
        cortes++;
        printf("Barbero termino de cortar el pelo\n");
        fflush(stdout);
        // Libera la silla del barbero
        msg.mtype = sillaBarbero;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        printf("Barbero hizo %d cortes\n", cortes);
        fflush(stdout);
    }
}

int main(){
    key_t key = ftok("/tmp", 'A');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if(msgid == -1){
        msgctl(msgid, IPC_RMID, NULL);
        msgid = msgget(key, 0666 | IPC_CREAT);
    }

    setvbuf(stdout, NULL, _IONBF, 0);

    struct mensaje msg;

    // Inicializa las sillas de espera
    msg.mtype = sillasEspera;
    for (int i = 0; i < 5; ++i) {
        msgsnd(msgid, &msg, sizeof(msg)- sizeof(long ), 0);
    }
    // Inicializa la silla del barbero
    msg.mtype = sillaBarbero;
    msgsnd(msgid, &msg, sizeof(msg)- sizeof(long ), 0);

    // Inicializa el control del cliente
    msg.mtype = controlCliente;
    msgsnd(msgid, &msg, sizeof(msg)- sizeof(long ), 0);

    // Crea el proceso del barbero
    pid_t pidBarbero = fork();
    if(pidBarbero == 0){
        peluqueria();
    }

    sleep(1);

    // Crea los procesos de los clientes
    pid_t pidClientes[clientesTotal];
    for (int i = 0; i < clientesTotal; ++i) {
        if((pidClientes[i] = fork()) == 0){
            clientes();
        }
    }

    // Espera a que todos los clientes terminen
    for (int i = 0; i < clientesTotal; ++i) {
        wait(NULL);
    }

    //kill(pidBarbero, SIGKILL);

    // Elimina la cola de mensajes
    if(msgctl(msgid, IPC_RMID, NULL) == -1)
        printf("Error al borrar la cola de mensajes\n");

    printf("Cerro la peluqueria\n");
    return 0;
}