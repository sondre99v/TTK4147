/*
 * utilities.h
 *
 *  Created on: 5. nov. 2018
 *      Author: larshb
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <pthread.h>


inline int set_priority(int priority) {
    int policy;
    struct sched_param param;

    if(priority < 1 || priority > 63){
        return -1;
    }

    pthread_getschedparam(pthread_self(), &policy, &param);
    param.sched_priority = priority;
    return pthread_setschedparam(pthread_self(), policy, &param);
}

inline int get_priority() {
    int policy;
    struct sched_param param;
    pthread_getschedparam(pthread_self(), &policy, &param);
    return param.sched_curpriority;
}

#endif /* UTILITIES_H_ */
