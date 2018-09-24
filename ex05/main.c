#include <pthread.h>
#include <time.h>

#include "io.h"

void* brtt (void* args);
struct timespec timespec_normalized(time_t sec, long nsec);
struct timespec timespec_add(struct timespec lhs, struct timespec rhs);

int main (int argc, char* argv[]) {
	io_init();
}

void* brtt (void* args) {
	int n = (long int)args;
	const struct timespec five_us = timespec_normalized(0, 5000);
	for (;;) {
		if (io_read(DI(n))) {
			io_write(DO(n), 1);
			nanosleep(&five_us, NULL);
			io_write(DO(n), 0);
		}
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