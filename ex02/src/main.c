#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 6

static inline void eatSlowly(void){
    for(long i = 0; i < 10000000; i++){
        // "Memory clobber" - tells the compiler optimizer that all the memory 
        // is being touched, and that therefore the loop cannot be optimized out
        asm volatile("" ::: "memory");
    }
}

pthread_mutex_t chpstcks[N];

void* fn(void* args) {
    int n = (int)args;

    for(;;) {
        pthread_mutex_lock(&chpstcks[n]);
        pthread_mutex_lock(&chpstcks[(n+1)%N]);

        printf("%d eating...\n", n);
        eatSlowly();

        pthread_mutex_unlock(&chpstcks[n]);
        pthread_mutex_unlock(&chpstcks[(n+1)%N]);
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t phils[N];
    
    for (int i = 0; i < N; i++) {
        pthread_create(&phils[i], NULL, fn, (void*)i);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(phils[i], NULL);
    }

    printf("Done!\n");

    return 0;
}