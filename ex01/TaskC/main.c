#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <sched.h>

#include "timespec.h"

#define MILLION * 1000000

void testA();
void testB();
void testC();

static __inline__ unsigned long __rdtsc()
{
	unsigned a, d;
	asm("cpuid");
	asm volatile("rdtsc" : "=a" (a), "=d" (d));

	return (((unsigned long)a) | (((unsigned long)d) << 32));
}

int main(int argc, char* argv[])
{
	if (strcmp(argv[1], "-A") == 0)
	{
		testA();
	}
	else if (strcmp(argv[1], "-B") == 0)
	{
		testB();
	}
	else if (strcmp(argv[1], "-C") == 0)
	{
		testC();
	}

	return 0;
}

void testA()
{
	int ns_max = 300;
	int histogram[ns_max];
	memset(histogram, 0, sizeof(int) * ns_max);

	for(int i = 0; i < 10 * 1000 * 1000; i++)
	{
		unsigned long t1 = __rdtsc();
		sched_yield();
		unsigned long t2 = __rdtsc();

		int ns = (int)((t2 - t1) * 1000.0 / 2659.829);

		if(ns >= 0 && ns < ns_max){
			histogram[ns]++;
		}
	}

	for(int i = 0; i < ns_max; i++){
		printf("%d\n", histogram[i]);
	}
}

void testB()
{
	int ns_max = 300;
	int histogram[ns_max];
	memset(histogram, 0, sizeof(int) * ns_max);

	struct timespec t1;
	struct timespec t2;

	for(int i = 0; i < 10 * 1000 * 1000; i++)
	{
	    clock_gettime(CLOCK_MONOTONIC, &t1);
		sched_yield();
	    clock_gettime(CLOCK_MONOTONIC, &t2);

		int ns = (int)(t2.tv_nsec - t1.tv_nsec);

		if(ns >= 0 && ns < ns_max){
			histogram[ns]++;
		}
	}

	for(int i = 0; i < ns_max; i++){
		printf("%d\n", histogram[i]);
	}
}

void testC()
{
	int ns_max = 50;
	int histogram[ns_max];
	memset(histogram, 0, sizeof(int) * ns_max);

	struct tms cpu_times;
	clock_t t1;
	clock_t t2;

	int oneSecond = sysconf(_SC_CLK_TCK);

	for(int i = 0; i < 10 * 1000 * 1000; i++)
	{
		t1 = times(&cpu_times);
		sched_yield();
		t2 = times(&cpu_times);

		int ns = (int)((double)(t2 - t1) * 1e9 / oneSecond);

		if(ns >= 0 && ns < ns_max){
			histogram[ns]++;
		}
	}

	for(int i = 0; i < ns_max; i++){
		printf("%d\n", histogram[i]);
	}
}
