#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>

#include "timespec.h"

#define until(x) while(!(x))

void busy_wait(struct timespec t);
void busywaitB();

int main(int argc, char* argv[])
{
	struct timespec oneSec = {
		.tv_sec = 1,
		.tv_nsec = 0
	};

	if (strcmp(argv[1], "-sleep") == 0)
	{
		sleep(1);
	}
	else if (strcmp(argv[1], "-busyclock") == 0)
	{
		busy_wait(oneSec);
	}
	else if (strcmp(argv[1], "-busytimes") == 0)
	{
		busywaitB();
	}

	return 0;
}

void busy_wait(struct timespec t)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    struct timespec then = timespec_add(now, t);
    
    while(timespec_cmp(now, then) < 0)
    {
        for(int i = 0; i < 10000; i++) { }

        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

void busywaitB()
{
	struct tms start_cpu;
	clock_t start_time = times(&start_cpu);

	struct tms end_cpu;
	clock_t end_time;

	int oneSecond = sysconf(_SC_CLK_TCK);

    do
    {
        for(int i = 0; i < 10000; i++) { }

        end_time = times(&end_cpu);
    } while(end_time - start_time < oneSecond);
}
