#include <unistd.h>
#include <string.h>
#include <sys/times.h>
#include "timespec.h"

void busy_wait(struct timespec t);
void busy_wait2();

int main(int argc, char* argv[])
{
	struct timespec one_sec = {
		.tv_sec = 1,
		.tv_nsec = 0
	};

	if (strcmp(argv[1], "-sleep") == 0)
	{
		sleep(1);
	}
	else if (strcmp(argv[1], "-busyclock") == 0)
	{
		busy_wait(one_sec);
	}
	else if (strcmp(argv[1], "-busytimes") == 0)
	{
		busy_wait2();
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
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

void busy_wait2()
{
	struct tms start_cpu;
	clock_t start_time = times(&start_cpu);

	struct tms end_cpu;
	clock_t end_time;

	int one_second = sysconf(_SC_CLK_TCK);

    do
    {
        end_time = times(&end_cpu);
    } while(end_time - start_time < one_second);
}
