#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdint.h>

#define BUFF_SIZE 5    /* total number of slots */
#define NP        3    /* total number of producers */
#define NC        3    /* total number of consumers */
#define NITERS    4    /* number of items produced/consumed */

typedef struct {
    int buf[BUFF_SIZE];    /* shared var */
    int in;                /* buf[in%BUFF_SIZE] is the first empty slot */
    int out;               /* buf[out%BUFF_SIZE] is the first full slot */
    sem_t full;            /* keep track of the number of full spots */
    sem_t empty;           /* keep track of the number of empty spots */
    sem_t mutex;           /* enforce mutual exclusion to shared data */
} sbuf_t;

sbuf_t shared;

void *Producer(void *arg)
{
    int i, item;
    intptr_t index = (intptr_t)arg;

    for (i = 0; i < NITERS; i++) {

        /* Produce item */
        item = i;

        /* Prepare to write item to buf */

        /* If there are no empty slots, wait */
        sem_wait(&shared.empty);
        /* If another thread uses the buffer, wait */
        sem_wait(&shared.mutex);

        shared.buf[shared.in] = item;
        shared.in = (shared.in + 1) % BUFF_SIZE;
        printf("[P%ld] Producing %d ...\n", index, item); 
        fflush(stdout);

        /* Release the buffer */
        sem_post(&shared.mutex);
        /* Increment the number of full slots */
        sem_post(&shared.full);

        /* Interleave producer and consumer execution */
        if (i % 2 == 1) sleep(1);
    }
    return NULL;
}

void *Consumer(void *arg)
{
    int i, item;
    intptr_t index = (intptr_t)arg;

    for (i = 0; i < NITERS; i++) {

        /* Esperar si no hay elementos disponibles en el buffer */
        sem_wait(&shared.full);
        /* Adquirir acceso exclusivo a la sección crítica */
        sem_wait(&shared.mutex);

        /* Retirar el ítem del buffer circular */
        item = shared.buf[shared.out];
        shared.out = (shared.out + 1) % BUFF_SIZE;

        printf("-------> [C%ld] consumed %d\n", index, item);
        fflush(stdout);

        /* Liberar la sección crítica */
        sem_post(&shared.mutex);
        /* Incrementar el contador de huecos libres */
        sem_post(&shared.empty);

        /* Alternar la ejecución */
        if (i % 2 == 1) sleep(1);
    }
    return NULL;
}
