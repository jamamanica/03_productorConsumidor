#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXPRODUCING 10
#define MAXAPPENDING 10
#define MAXTAKING 5
#define MAXCONSUMING 5

#define _wait(a) sleep(rand() % a)
#define out(s) \
    printf(s); \
    fflush(stdout)
#define outi(s, n) \
    printf(s, n); \
    fflush(stdout)

int n;
sem_t s, delay;

void produce();
void append();
void consume();
void take();

void *producer(void *data)
{
    while (1)
    {
        produce();
        sem_wait(&s);
        append();
        n = n + 1;
        outi("[P] \t \t item: %d\n", n);
        if (n == 1)
            sem_post(&delay);
        sem_post(&s);
    }
    pthread_exit(0);
}

// the order of semaphores was changed
void *consumer(void *data)
{
    sem_wait(&s); // <-- line interchanged
    while (1)
    {
        sem_wait(&delay); // <-- line interchanged
        take();
        outi("[C] \t \t item: %d\n", n);
        n = n - 1;
        sem_post(&s);
        consume();
        if (n == 0)
            sem_wait(&delay);
    }
    pthread_exit(0);
}
