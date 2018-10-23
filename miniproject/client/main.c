#include "lib/time/time.h"
#include "communication.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void* listener_thread_function(void* args);
void* controller_thread_function(void* args);

static sem_t ctrl_sem;
float argf = 0;

int main (int argc, char *argv[]) {
	if (argc > 1) {
		printf("Optional argument: %s\n", argv[1]);
		argf = atof(argv[1]);
	}

	com_init();
	
	pthread_t listener_thread;
	pthread_t responder_thread;
	pthread_t controller_thread;

	sem_init(&ctrl_sem, 0, 0);

	if (pthread_create(&listener_thread, NULL, listener_thread_function, NULL) ||
		pthread_create(&controller_thread, NULL, controller_thread_function, NULL))
	{
		fprintf(stderr, "Error creating one or more threads!\n");
		return 1;
	}

	if (pthread_join(listener_thread, NULL) || 
		pthread_join(controller_thread, NULL))
	{
		fprintf(stderr, "Error joining one or more threads!\n");
		return 2;
	}

	return 0;
}

/** LISTENER THREAD **/

float global_value_variable = 0;

void* listener_thread_function(void* args) {
	while (1) {
		float value;
		switch(com_receive_command(&value)) {
			case SIGNAL:
				com_send_command(SIGNAL_ACK, 0);
				break;
			case GET_ACK:
				global_value_variable = value;
				sem_post(&ctrl_sem);
				break;
			default:
				printf("Received unknown\n");
				break;
		}
	}
}


/** CONTROLLER THREAD **/

#define Kp  10 // [1]
#define Ki 800 // [s^-1]
#define Kd   0 // [s]
#define dt_ns (4ULL * 1000 * 1000) // [ns]
#define dt (1e-9 * dt_ns) // [s]
#define freq (unsigned int)(1.0/dt)

float pid (float error) {
	static float prev_error = 0;
	static float integral = 0;

	integral += error * dt;
	float derivative = (error - prev_error) / dt;
	prev_error = error;
	return Kp * error + Ki * integral + Kd * derivative;
}

void* controller_thread_function(void* args) {
	printf("Kp = %d, ", Kp);
	printf("Ki = %d, ", Ki);
	printf("Kd = %d\n", Kd);
	printf("dt = %f\n", dt);
	printf("Steps/sec = %u\n", freq);

	struct timespec waketime;
	struct timespec period = {.tv_sec = 0, .tv_nsec = dt_ns};
	clock_gettime(CLOCK_REALTIME, &waketime);

	float y;
	unsigned int i;
	for (i = 0; i < freq*2; i++){
		waketime = timespec_add(waketime, period);

		com_send_command(GET, 0);
		sem_timedwait(&ctrl_sem, &waketime);

		y = global_value_variable;
		float reference = i < freq ? 1 : 0;
		float u = pid(reference - y);

		com_send_command(SET, u);

		#ifdef WINDOWS
		usleep(dt_ns/1000); // Disregarding PID delay
		#else
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
		#endif
	}

	com_close();
	
	exit(0); /* FIXME */

	return NULL;
}
