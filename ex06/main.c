#include <native/task.h>
#include <native/timer.h>
#include <sys/mman.h>
#include <rtdk.h>
#include "io.h"
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

// CONFIGURATION

#define BUSY_WAIT
//#define PERIODIC

#define NUM_DISTURBANCE_THREADS 10

// END CONFIGURATION

void brttResponder(void *arg);
void* disturb (void* args);

RT_TASK tasks[4];
int task_arguments[] = {1, 2, 3, 4};

pthread_t disturb_threads[NUM_DISTURBANCE_THREADS];

int main(int argc, char* argv[])
{
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);
	
	io_init();

	rt_task_create(&tasks[0], "TestTask1", 0, 99, T_CPU(0));
	rt_task_create(&tasks[1], "TestTask2", 0, 99, T_CPU(1));
	rt_task_create(&tasks[2], "TestTask3", 0, 99, T_CPU(2));
	rt_task_create(&tasks[3], "TestTask4", 0, 99, T_CPU(3));

	for (long int i = 0; i < NUM_DISTURBANCE_THREADS; i++) {
        	pthread_create(&disturb_threads[i], NULL, disturb, NULL);
    	}

	rt_task_start(&tasks[0], brttResponder, &task_arguments[0]);
	rt_task_start(&tasks[1], brttResponder, &task_arguments[1]);
	rt_task_start(&tasks[2], brttResponder, &task_arguments[2]);
	rt_task_start(&tasks[3], brttResponder, &task_arguments[3]);

	while(1) {sleep(-1);}
	
	return 0;
}

void brttResponder(void *arg)
{
	//int channel = ((int*)arg)[0];

	unsigned long duration = 10ULL * 1000 * 1000 * 1000;  // 10 second timeout
	unsigned long endTime = rt_timer_read() + duration;
	
#ifdef PERIODIC
	rt_task_set_periodic(NULL, TM_NOW, 1ULL * 1000 * 1000);
#endif
	
	//io_write(channel, 1);	

	while(1)
	{	
		/*if (!io_read(channel))
		{
			io_write(channel, 0);
			//rt_timer_spin(5000);
			io_write(channel, 1);
		}*/
		
		

		if(rt_timer_read() > endTime)
		{
			rt_printf("Time expired\n");
			rt_task_delete(NULL);
		}
		//if(rt_task_yield())
		//{
		//	rt_printf("Task failed to yield\n");
		//	rt_task_delete(NULL);
		//}
		
#ifdef PERIODIC
		rt_task_wait_period(NULL);
#endif
	}
}

int set_cpu(int cpu_number)
{
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void* disturb (void* args)
{
	set_cpu(0);
	
	while (1)
	{
		asm volatile("" ::: "memory");
	}
}

