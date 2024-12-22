#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define REPETITIONS 100  // Número de repeticiones 

// Declaración de semáforos
sem_t ox_vacio, ox_lleno, hid_vacio, hid_lleno;
pthread_mutex_t mutex_ox, mutex_hid;

void* gen_oxigeno(void* arg) {
    for (int i = 0; i < REPETITIONS; ++i) {
      
        sem_wait(&ox_vacio); 
            pthread_mutex_lock(&mutex_ox);
                
                printf("Generé Oxígeno\n"); 

            pthread_mutex_unlock(&mutex_ox);
        sem_post(&ox_lleno);

        sleep(2);
    }
    return NULL;
}

void* gen_hidrogeno(void* arg) {
    for (int i = 0; i < REPETITIONS; ++i) {
      
        sem_wait(&hid_vacio); 
            pthread_mutex_lock(&mutex_hid);
                   
                printf("Generé Hidrogeno\n"); 

            pthread_mutex_unlock(&mutex_hid);
        sem_post(&hid_lleno);

        sleep(2);
    }
    return NULL;
}

void* gen_agua(void* arg) {
    for (int i = 0; i < REPETITIONS; ++i) {
        
//oxigeno
        sem_wait(&ox_lleno);
            pthread_mutex_lock(&mutex_ox);
                                                  //acceso al buffer
                 printf("Tomé un oxigeno\n");   

            pthread_mutex_unlock(&mutex_ox);
        sem_post(&ox_vacio);

//dos hidrógenos

     for (int j = 0; j < 2; ++j) {
        sem_wait(&hid_lleno);
            pthread_mutex_lock(&mutex_hid);
                                                    //acceso al buffer
                 printf("Tomé un hidrogeno\n");

            pthread_mutex_unlock(&mutex_hid);
        sem_post(&hid_vacio);
     }      
   
        printf("Se generó una molécula de agua!!\n"); 
        sleep(1);
    
    }
    return NULL;
}

int main() {
    pthread_t thread_a, thread_b, thread_c;

    // Inicialización de los semáforos
    sem_init(&ox_vacio, 0, 20); 
    sem_init(&ox_lleno, 0, 0); 
    sem_init(&hid_vacio, 0, 20); 
    sem_init(&hid_lleno, 0, 0);
    
    //inicialización de mutex's'
    pthread_mutex_init(&mutex_ox, NULL);
    pthread_mutex_init(&mutex_hid, NULL);


    // Creación de los hilos
    pthread_create(&thread_a, NULL,gen_oxigeno, NULL);
    pthread_create(&thread_b, NULL, gen_hidrogeno, NULL);
    pthread_create(&thread_c, NULL, gen_agua, NULL);

    // Espera a que los hilos terminen
    pthread_join(thread_a, NULL);
    pthread_join(thread_b, NULL);
    pthread_join(thread_c, NULL);

    // Destrucción de los semáforos

    sem_destroy(&ox_vacio);
    sem_destroy(&ox_lleno);
    sem_destroy(&hid_vacio);
    sem_destroy(&hid_lleno);

    printf("\n");  // Salto de línea final
    return 0;
}