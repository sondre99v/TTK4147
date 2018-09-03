#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

long global = 0;
sem_t sem;

void* fn(void* args) {
    long local = 0;
    for (int i = 0; i < 50e6; i++) {
        sem_wait(&sem);
        global++;
        sem_post(&sem);
        local++;
    }
    printf("Thread: %s, Global: %ld, Local: %ld\n",
        (char*)args, global, local);
    fflush(stdout);
    return NULL;
}

int main(int argc, char* argv[]) {
    sem_init(&sem, 0, 1);
    pthread_t threadHandle1;
    pthread_t threadHandle2;
    pthread_create(&threadHandle1, NULL, fn, "1");
    pthread_create(&threadHandle2, NULL, fn, "2");
    pthread_join(threadHandle1, NULL);
    pthread_join(threadHandle2, NULL);
    return 0;
}