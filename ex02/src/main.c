#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 7

const char* names[N] = { "Mathisen", "Reklev", "Petersen", "Henriksen", "Pedersen", "Lofstad", "Lervik" };

static inline void eatSlowly(void){
    for(long i = 0; i < 100000000; i++){
        // "Memory clobber" - tells the compiler optimizer that all the memory 
        // is being touched, and that therefore the loop cannot be optimized out
        asm volatile("" ::: "memory");
    }
}

pthread_mutex_t chpstcks[N];
pthread_mutex_t pickupMutex;

void* fn(void* args) {
    int n = (int)args;

    int eaten = 0;

    for(;;) {
        pthread_mutex_lock(&pickupMutex);
        {
            pthread_mutex_lock(&chpstcks[n]);
            pthread_mutex_lock(&chpstcks[(n+1)%N]);
        }
        pthread_mutex_unlock(&pickupMutex);

        printf("(%d) %s eating...\n", eaten++, names[n]);
        eatSlowly();

        pthread_mutex_unlock(&chpstcks[n]);
        pthread_mutex_unlock(&chpstcks[(n+1)%N]);
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    pthread_t phils[N];
    pthread_mutex_init(&pickupMutex, NULL);


    for(int i = 0; i < N; i++) {
        pthread_mutex_init(&chpstcks[i], NULL);
    }
    
    for (int i = 0; i < N; i++) {
        pthread_create(&phils[i], NULL, fn, (void*)i);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(phils[i], NULL);
    }

    printf("Done!\n");

    return 0;
}