#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define N 10
#define M 5

int mutex[2], escritor[2], lector[2];

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void * lectores(void *arg){
    int id = *(int *)arg;
    char c;
    set_nonblocking(lector[0]);
    while(1){
        read(lector[0], &c, sizeof(char));
        if(read(lector[0], &c, sizeof(char)) == -1){
            printf("Lector %d esperando\n", id);
            fflush(stdout);
            read(escritor[0], &c, sizeof(char));
        }else
            write(lector[1], "L", sizeof(char));
        write(lector[1], "L", sizeof(char));
        write(mutex[1], "M", sizeof(char));


        //Region Critica
        printf("Lector %d leyendo\n", id);
        fflush(stdout);
        sleep(3);

        read(mutex[0], &c, sizeof(char));
        read(lector[0], &c, sizeof(char));
        if(read(lector[0], &c, sizeof(char)) == -1){
            printf("Ultimo lector %d saliendo\n", id);
            fflush(stdout);
            write(escritor[1], "E", sizeof(char));
        } else{
            printf("Lector %d saliendo \n", id);
            fflush(stdout);
            write(lector[1], "L", sizeof(char));
        }
        write(mutex[1], "M", sizeof(char));
        sleep(3);
    }
}

void * escritores(void *arg){
    int id = *(int *)arg;
    char c;
    close(lector[1]);
    close(lector[0]);
    while(1){
        printf("Escritor %d esperando\n", id);
        fflush(stdout);
        read(escritor[0], &c, sizeof(char));
        printf("Escritor %d escribiendo\n", id);
        fflush(stdout);
        sleep(3);
        write(escritor[1], "E", sizeof(char));
        printf("Escritor %d saliendo\n", id);
        fflush(stdout);
    }
}

int main(){
    pipe(mutex);
    pipe(escritor);
    pipe(lector);

    write(escritor[1], "E", sizeof(char));
    write(mutex[1], "M", sizeof(char));

    for(int i = 0; i < N; i++){
        if(fork() == 0){
            corredores(&i);
        }
    }

    for(int i = N; i < N+M; i++){
        if(fork() == 0){
            escritores(&i);
        }
    }

    close(mutex[0]);
    close(mutex[1]);
    close(escritor[0]);
    close(escritor[1]);
    close(lector[0]);
    close(lector[1]);

    for(int i = 0; i < N+M; i++){
        wait(NULL);
    }

    return 0;
}