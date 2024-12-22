#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define REPETITIONS 100  // Número de repeticiones 

// Declaración de semáforos
sem_t sem_ox, sem_hid, sem_h2o;

void* gen_oxigeno(void* arg) {
    for (int i = 0; i < REPETITIONS; ++i) {
       for (int j = 0; j < 15; ++j) {

            sem_post(&sem_ox);   
            printf("Generé Oxígeno\n"); 
            sleep(1);
        }
        sleep(5);
    }
    return NULL;
}

void* gen_hidrogeno(void* arg) {
    for (int i = 0; i < REPETITIONS; ++i) {
       for (int j = 0; j < 20; ++j) {

            sem_post(&sem_hid);   
            printf("Generé Hidrógeno\n"); 
            sleep(1);
        }
        sleep(8);
    }
    return NULL;
}

void* gen_agua(void* arg) {
    for (int i = 0; i < REPETITIONS; ++i) {
        sem_wait(&sem_ox);  
        sem_wait(&sem_hid);
        sem_wait(&sem_hid);  
               
        sem_post(&sem_h2o);
        printf("Se generó una molécula de agua!!\n"); 
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t thread_a, thread_b, thread_c;

    // Inicialización de los semáforos
    sem_init(&sem_ox, 0, 0);  
    sem_init(&sem_hid, 0, 0);  
    sem_init(&sem_h2o, 0, 0); 


    // Creación de los hilos
    pthread_create(&thread_a, NULL,gen_oxigeno, NULL);
    pthread_create(&thread_b, NULL, gen_hidrogeno, NULL);
    pthread_create(&thread_c, NULL, gen_agua, NULL);

    // Espera a que los hilos terminen
    pthread_join(thread_a, NULL);
    pthread_join(thread_b, NULL);
    pthread_join(thread_c, NULL);

    // Destrucción de los semáforos
    sem_destroy(&sem_ox);
    sem_destroy(&sem_hid);
    sem_destroy(&sem_h2o);

    printf("\n");  // Salto de línea final
    return 0;
}