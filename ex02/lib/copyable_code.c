#include <pthread.h>

// Note the argument and return types: void*
void* fn(void* args){
    return NULL;
}

int main(){
    pthread_t threadHandle;
    pthread_create(&threadHandle, NULL, fn, NULL);    
    pthread_join(threadHandle, NULL);
}





#include <pthread.h>

int main(){
    pthread_mutex_t mtx;

    // 2nd arg is a pthread_mutexattr_t
    pthread_mutex_init(&mtx, NULL);

    pthread_mutex_lock(&mtx);
    // Critical section
    pthread_mutex_unlock(&mtx);

    pthread_mutex_destroy(&mtx);
}