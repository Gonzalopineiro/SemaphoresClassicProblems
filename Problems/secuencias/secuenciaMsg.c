#include <stdio.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

#define A 1
#define B 2
#define C 3

struct mensaje {
    long msgT;
};

void * letraA(){
    key_t key = ftok("/tmp", 'C');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct mensaje msg;
    while(1){
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), A, 0);
        printf("A");
        msg.msgT = B;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
    }
}

void * letraB(){
    key_t key = ftok("/tmp", 'C');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct mensaje msg;
    while(1){
        for (int i = 0; i < 2; ++i) {
            msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), B, 0);
            printf("B");
            msg.msgT = C;
            msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        }
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), B, 0);
        msg.msgT = C;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
    }
}

void * letraC(){
    key_t key = ftok("/tmp", 'C');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct mensaje msg;
    while(1){
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), C, 0);
        printf("C");

        msg.msgT = A;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), C, 0);
        msg.msgT = A;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
        msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), C, 0);
        printf("C");
        sleep(2);
        msg.msgT = A;
        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
    }
}

int main(){
    key_t key = ftok("/tmp", 'C');
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct mensaje msg;
    msg.msgT = A;
    msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);

    setvbuf(stdout, NULL, _IONBF, 0);

    if(fork() == 0)
        letraA();
    else if(fork() == 0)
            letraB();
    else if(fork() == 0)
            letraC();


    for (int i = 0; i < 3; ++i) {
        wait(NULL);
    }

    msgctl(msgid, IPC_RMID, NULL);

    return 0;

}

