#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <sys/mman.h>
#include <rtdk.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

#define TP(x,y) x ## y
#define TP2(x, y) TP(x,y)

#define rt_printf(str) \
	struct rt_task_info TP2(temp, __LINE__); \
	rt_task_inquire(NULL, &TP2(temp, __LINE__)); \
	rt_printf("(%i, %i) " str, TP2(temp, __LINE__).bprio, TP2(temp, __LINE__).cprio)

void thread_l(void *arg);
void thread_m(void *arg);
void thread_h(void *arg);
void busy_wait_us(unsigned long delay);

RT_TASK tasks[3];
int task_arguments[] = {1, 2, 3};

RT_SEM sem;
RT_MUTEX mutex;

int main(int argc, char* argv[])
{
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);
	
	rt_sem_create(&sem, "Xenomai-Semaphore", 0, 0);
	rt_mutex_create(&mutex, "Resource");
	
	rt_task_shadow(NULL, "MainTask", 99, T_CPU(0));

	rt_task_create(&tasks[0], "TestTaskL", 0, 80, T_CPU(0));
	rt_task_create(&tasks[1], "TestTaskM", 0, 81, T_CPU(0));
	rt_task_create(&tasks[2], "TestTaskH", 0, 82, T_CPU(0));

	rt_printf("start two tasks\n");
	rt_task_start(&tasks[0], thread_l, &task_arguments[0]);
	rt_task_start(&tasks[1], thread_m, &task_arguments[1]);
	rt_task_start(&tasks[2], thread_h, &task_arguments[2]);
	
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

void thread_l(void *arg) 
{
	rt_printf("-> Low priority thread started\n");
	rt_mutex_acquire(&mutex, TM_INFINITE);
	rt_printf("-> Low priority thread locked resource\n");
	busy_wait_us(300ULL * 1000);
	rt_printf("-> Low priority thread releases resource\n");
	rt_mutex_release(&mutex);
	rt_printf("-> Low priority thread done\n");
}

void thread_m(void *arg) 
{
	rt_printf("-> Medium priority thread started\n");
	//int channel = ((int*)arg)[0];
	rt_task_sleep(100ULL * 1000 * 1000);
	rt_printf("-> Medium priority thread starts busy-waiting\n");
	busy_wait_us(500ULL * 1000);
	rt_printf("-> Medium priority thread done\n");
}

void thread_h(void *arg) 
{
	rt_printf("-> High priority thread started\n");
	//int channel = ((int*)arg)[0];

	rt_task_sleep(200ULL * 1000 * 1000);
	rt_mutex_acquire(&mutex, TM_INFINITE);
	rt_printf("-> High priority thread locked resource\n");

	busy_wait_us(200ULL * 1000);
	rt_printf("-> High priority thread releases resource\n");
	rt_mutex_release(&mutex);
	rt_printf("-> High priority thread done\n");
}

int set_cpu(int cpu_number)
{
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number, &cpu);

	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void busy_wait_us(unsigned long delay)
{
	for (; delay > 0; delay--)
	{
		rt_timer_spin(1000);
	}
}

