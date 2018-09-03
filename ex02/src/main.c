#include <stdio.h>
#include <pthread.h>

long global = 0;

void* fn(void* args) {
    long local = 0;
    for (int i = 0; i < 50e6; i++) {
        global++;
        local++;
    }
    printf("Thread: %s, Global: %ld, Local: %ld\n",
        (char*)args, global, local);
    fflush(stdout);
    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t threadHandle1;
    pthread_t threadHandle2;
    pthread_create(&threadHandle1, NULL, fn, "1");
    pthread_create(&threadHandle2, NULL, fn, "2");
    pthread_join(threadHandle1, NULL);
    pthread_join(threadHandle2, NULL);
    return 0;
}