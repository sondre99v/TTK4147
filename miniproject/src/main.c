#include "time.h"
#include "communication.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define REPS 50

/**************************************************
 *
 * Task A
 * 
 * - Set setpoint to 1
 * - Keep 1 second (should be stable after 0.3)
 * - Setpoint 0
 * - Keep another 1 second
 *
 *************************************************/

void* controller(void* args);

int main (int argc, char *argv[]) {
	com_init();
	
	pthread_t controller_thread;

	if (pthread_create(&controller_thread, NULL, controller, NULL)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}

	if (pthread_join(controller_thread, NULL)) {
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}
	
	com_close();

	return 0;
}

float error, integral, derivative, prev_error;

#define Kp 10
#define Ki 800
#define Kd 0
#define dt 1

float pid(float y, float reference) {
	error       = reference - y;
	//integral    += error * dt;
	integral = integral + error * dt;
	derivative  = (error - prev_error) / dt;
	prev_error  = error;    

	return  Kp * error      +
	        Ki * integral   +
	        Kd * derivative;
}

void* controller(void* args) {
	struct timespec waketime;
	struct timespec period = {.tv_sec = 0, .tv_nsec = 5*1000};
	clock_gettime(CLOCK_REALTIME, &waketime);

	float y;
	for (int i = 0; i < REPS; i++){
		com_get(&y);
		com_set(pid(y, 1));

		waketime = timespec_add(waketime, period);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
	}

	return NULL;
}
