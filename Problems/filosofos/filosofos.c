#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
#define ITERATIONS 100

#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"

sem_t sem_stick[NUM_PHILOSOPHERS];

// semaforo para prevencion de deadlock
// siempre que un palillo quede libre, no se genera deadlock
sem_t sem_turns;

void *philosophers(void *args) {
    int id = *((int *) args); // Obtener el valor del puntero

    int left_stick = id % NUM_PHILOSOPHERS;
    int right_stick = (id + 4) % NUM_PHILOSOPHERS;

    // Si el filósofo es el 1, intercambiar los palillos
    if (id == 1) {
        int temp = left_stick;
        left_stick = right_stick;
        right_stick = temp;
    }

    while (1) {
        // filosofo pensando
        printf(BLUE "Filosofo %d pensando... \n" RESET, id);

        sem_wait(&sem_turns);

        sem_wait(&sem_stick[left_stick]);
        sem_wait(&sem_stick[right_stick]);

        // filosofo comiendo
        printf(RED "Filosofo %d come (s:%d s:%d)\n"RESET, id, left_stick, right_stick);
        sleep(1);

        sem_post(&sem_stick[left_stick]);
        sem_post(&sem_stick[right_stick]);
        sem_post(&sem_turns);
    }
    pthread_exit(NULL);
}

void main() {
    pthread_t philosophers_t[NUM_PHILOSOPHERS];
    
    int ids[NUM_PHILOSOPHERS]; // Array para almacenar los IDs

    sem_init(&sem_turns, 0, 4);

    // ---- palillos y filosofos ----

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_init(&sem_stick[i], 0, 1);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i + 1; // Asignar el ID
        pthread_create(&philosophers_t[i], NULL, philosophers, (void *) &ids[i]); // Pasar el puntero del ID
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers_t[i], NULL);
    }

    // ---- destruye los palillos ----

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&sem_stick[i]);
    }
}