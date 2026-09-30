#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
