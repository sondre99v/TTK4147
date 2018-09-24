#define _GNU_SOURCE

#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <sched.h>

#include "io.h"

#define N 3 // tests

void* brtt (void* args);
void* disturb (void* args);
struct timespec timespec_normalized(time_t sec, long nsec);
struct timespec timespec_add(struct timespec lhs, struct timespec rhs);
int set_cpu(int cpu_number);

int main (int argc, char* argv[]) {
	io_init();
	pthread_t threads[N];
	pthread_t disturb_threads[10];

	for (long int i = 0; i < N; i++) {
		io_write(i+1, 1);
        pthread_create(&threads[i], NULL, brtt, (void*)i);
    }
    for (long int i = 0; i < 10; i++) {
        //pthread_create(&disturb_threads[i], NULL, disturb, NULL);
    }
    for(;;);
}

void* brtt (void* args) {
	//set_cpu(0);

	long int n = (long int)args+1;

	set_cpu(n-1);

	struct timespec five_us = {.tv_sec = 0, .tv_nsec = 5*1000};
	struct timespec waketime;
	clock_gettime(CLOCK_REALTIME, &waketime);

	struct timespec period = {.tv_sec = 0, .tv_nsec = 1000*1000};

	while(1){
		if (!io_read(n)) {
			//printf("Test %ld\n", n);
			io_write(n, 0);
			nanosleep(&five_us, NULL);
			io_write(n, 1);
		}
		//nanosleep(&five_us, NULL);
	    waketime = timespec_add(waketime, period);
	    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
	}
}

void* disturb (void* args) {
	while (1) {
		asm volatile("" ::: "memory");
	}
}

struct timespec timespec_normalized(time_t sec, long nsec){
    while(nsec >= 1000000000){
        nsec -= 1000000000;
        ++sec;
    }
    while(nsec < 0){
        nsec += 1000000000;
        --sec;
    }
    return (struct timespec){sec, nsec};
}

struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}

int set_cpu(int cpu_number){
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}