#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <sys/mman.h>
#include <rtdk.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

void thread_fn(void *arg);

RT_TASK tasks[2];
int task_arguments[] = {1, 2};

RT_SEM sem;

int main(int argc, char* argv[])
{
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);
	
	rt_sem_create(&sem, "Xenomai-Semaphore", 0, 0);
	
	rt_task_shadow(NULL, "MainTask", 99, T_CPU(0));

	rt_task_create(&tasks[0], "TestTask1", 0, 81, T_CPU(0));
	rt_task_create(&tasks[1], "TestTask2", 0, 80, T_CPU(0));

	rt_printf("start two tasks\n");
	rt_task_start(&tasks[0], thread_fn, &task_arguments[0]);
	rt_task_start(&tasks[1], thread_fn, &task_arguments[1]);
	
	rt_printf("sleep 100ms\n");
	usleep(100ULL * 1000);
	
	rt_printf("broadcast semaphore\n");
	rt_sem_broadcast(&sem);
	
	rt_printf("sleep 100ms\n");
	usleep(100ULL * 1000);
	
	rt_sem_delete(&sem);
	
	rt_printf("exit program\n");
	return 0;
}

void thread_fn(void *arg)
{
	int channel = ((int*)arg)[0];
	rt_printf("-> Thread %d is waiting\n", channel);
	rt_sem_p(&sem, TM_INFINITE);
	rt_printf("-> Thread %d ran\n", channel);
}

int set_cpu(int cpu_number)
{
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

