#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <pthread.h>
#include <unistd.h>

dispatch_t              *dpp;
resmgr_attr_t           resmgr_attr;
dispatch_context_t      *ctp;
resmgr_connect_funcs_t  connect_funcs;
resmgr_io_funcs_t       io_funcs;
iofunc_attr_t           io_attr;

int io_read(resmgr_context_t* ctp, io_read_t* msg, iofunc_ocb_t* ocb);
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);


static struct {
	enum {
		COUNT_STOP,
		COUNT_UP,
		COUNT_DOWN
	} mode;

	int value;
	pthread_mutex_t mode_mutex;
	pthread_t thread;
} counter = {
		.mode = COUNT_STOP,
		.value = 0
};

void* counter_function(void* args);

void error(char* s){
	perror(s);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	printf("Start resource manager\n");

	// create dispatch.
	if (!(dpp = dispatch_create())){
		error("Create dispatch");
	}

	// initialize resource manager attributes.
	memset(&resmgr_attr, 0, sizeof(resmgr_attr));
	resmgr_attr.nparts_max = 1;
	resmgr_attr.msg_max_size = 2048;

	// set standard connect and io functions.
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);
	iofunc_attr_init(&io_attr, S_IFNAM | 0666, 0, 0);

	// override functions
	io_funcs.read = io_read;
	io_funcs.write = io_write;

	// establish resource manager
	if (resmgr_attach(dpp, &resmgr_attr, "/dev/myresource", _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &io_attr) < 0)
		error("Resmanager attach");
	ctp = dispatch_context_alloc(dpp);


	pthread_mutex_init(&counter.mode_mutex, NULL);
	pthread_create(&counter.thread, NULL, &counter_function, NULL);


	// wait forever, handling messages.
	while(1){
		if (!(ctp = dispatch_block(ctp))){
			error("Dispatch block");
		}
		dispatch_handler(ctp);
	}

	exit(EXIT_SUCCESS);
}


void* counter_function(void* args) {

	while(1) {
		pthread_mutex_lock(&counter.mode_mutex);

		switch(counter.mode) {
			case COUNT_UP:
				counter.value++;
				break;
			case COUNT_DOWN:
				counter.value--;
				break;
			default:
				break;
		}

		pthread_mutex_unlock(&counter.mode_mutex);

		usleep((int)1e5); // 10ms
	}
}

static char read_buffer[64];

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb){
	volatile int counter_value;

	pthread_mutex_lock(&counter.mode_mutex);
	counter_value = counter.value;
	pthread_mutex_unlock(&counter.mode_mutex);

	int n_bytes = sprintf(read_buffer, "Value: %d\n", counter_value);

	if(!ocb->offset){
		// set data to return
		SETIOV(ctp->iov, read_buffer, n_bytes);
		_IO_SET_READ_NBYTES(ctp, n_bytes);

		// increase the offset (new reads will not get the same data)
		ocb->offset = 1;

		// return
		return (_RESMGR_NPARTS(1));
	} else {
		// set to return no data
		_IO_SET_READ_NBYTES(ctp, 0);

		// return
		return (_RESMGR_NPARTS(0));
	}
}

int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
	char buf[64];

    int status;

    if ((status = iofunc_write_verify(ctp, msg, ocb, NULL)) != EOK)
        return (status);

    if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
        return(ENOSYS);

    _IO_SET_WRITE_NBYTES (ctp, msg->i.nbytes);

    resmgr_msgread(ctp, buf, msg->i.nbytes, sizeof(msg->i));
    buf[msg->i.nbytes] = '\0'; /* just in case the text is not NULL terminated */
    printf ("Received %d bytes = '%s'\n", msg -> i.nbytes, buf);

    if (strcmp(buf, "stop\n") == 0) {
		pthread_mutex_lock(&counter.mode_mutex);
		counter.mode = COUNT_STOP;
		pthread_mutex_unlock(&counter.mode_mutex);
	}
    else if (strcmp(buf, "up\n") == 0) {
		pthread_mutex_lock(&counter.mode_mutex);
		counter.mode = COUNT_UP;
		pthread_mutex_unlock(&counter.mode_mutex);
	}
	else if (strcmp(buf, "down\n") == 0) {
		pthread_mutex_lock(&counter.mode_mutex);
		counter.mode = COUNT_DOWN;
		pthread_mutex_unlock(&counter.mode_mutex);
	}
	else if (strcmp(buf, "reset\n") == 0) {
		pthread_mutex_lock(&counter.mode_mutex);
		counter.mode = COUNT_STOP;
		counter.value = 0;
		pthread_mutex_unlock(&counter.mode_mutex);
	}
	else {
		printf("Unknown command \"%s\"\n", buf);
	}

    if (msg->i.nbytes > 0)
        ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

    return (_RESMGR_NPARTS (0));
}
