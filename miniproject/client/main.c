#include "lib/time/time.h"
#include "communication.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void* listener_thread_function(void* args);
void* controller_thread_function(void* args);

#define CLOCK_SOURCE CLOCK_REALTIME

#define Kp 2e1 // [1]
#define Ki 1e3 // [s^-1]
#define Kd 1e-2 // [s]
#define period_ns (3ULL * 1000 * 1000) // [ns]

static sem_t controller_semaphore;
static float y_recieved = 0;

int main (int argc, char* argv[]) {
	com_init();

	sem_init(&controller_semaphore, 0, 0);
	
	pthread_t listener_thread;
	pthread_t controller_thread;

	if (pthread_create(&listener_thread, NULL, listener_thread_function, NULL) ||
		pthread_create(&controller_thread, NULL, controller_thread_function, NULL))
	{
		fprintf(stderr, "Error creating one or more threads!\n");
		return 1;
	}

	/* Wait until controller thread returns. */
	pthread_join(controller_thread, NULL);


	/* Since the listener thread locks on UDP receive, and no timeout
	 * is spesified, the listener thread will never return.
	 * Therefore, we don't join it, and let the OS mop it up for us.
	 */

	return 0;
}


/** LISTENER THREAD **/

void* listener_thread_function(void* args) {
	while (1) {
		float value;
		switch(com_receive_command(&value)) {
			case SIGNAL:
				com_send_command(SIGNAL_ACK, 0);
				break;
			case GET_ACK:
				y_recieved = value;
				sem_post(&controller_semaphore);
				break;
			default:
				break;
		}
	}

	return NULL; /* Suppress compiler warning */
}


/** CONTROLLER THREAD **/

#define dt (1e-9 * period_ns) // [s]
#define iterations_per_second (unsigned int)(1.0 / dt) // [Hz]

float pid (float error) {
	static float prev_error = 0;
	static float integral = 0;

	integral += error * dt;

	float derivative = (error - prev_error) / dt;

	prev_error = error;

	return Kp * error + Ki * integral + Kd * derivative;
}

void* controller_thread_function(void* args) {
	struct timespec waketime;
	struct timespec period = {.tv_sec = 0, .tv_nsec = period_ns};
	clock_gettime(CLOCK_SOURCE, &waketime);

	unsigned int i;
	for (i = 0; i < 2ULL * iterations_per_second; i++) {
		waketime = timespec_add(waketime, period);

		float reference = (i < iterations_per_second) ? 1.0 : 0.0;

		/* Request new system measurement, only if no data is yet unhandled */
		int sem_val;
		sem_getvalue(&controller_semaphore, &sem_val);
		if (sem_val == 0) {
			com_send_command(GET, 0);
		}
		
		/* Wait until listener thread receives a system measurement */
		sem_timedwait(&controller_semaphore, &waketime);
		float y = y_recieved;
		float u = pid(reference - y);

		com_send_command(SET, u);

		clock_nanosleep(CLOCK_SOURCE, TIMER_ABSTIME, &waketime, NULL);
	}

	com_close();

	return NULL;
}
