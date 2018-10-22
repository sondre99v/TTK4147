#ifndef COMMUNICATION_H__
#define COMMUNICATION_H__

typedef enum { 
	ERROR, 
	START, 
	GET, 
	GET_ACK, 
	SET, 
	STOP, 
	SIGNAL, 
	SIGNAL_ACK
} com_cmd_t;


void com_init();

void com_close();

com_cmd_t com_receive_command(float* value);

void com_send_command(com_cmd_t cmd, float val);

#endif /* end of include guard: COMMUNICATION_H__ */
