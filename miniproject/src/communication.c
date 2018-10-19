#include "communication.h"
//#include "../lib/network/udp.h"
#include "udp.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define COM_COMMANDS	 7
#define COM_STRLEN		 8
#define COM_BUFSIZE		64

UDPConn* conn;

char sendBuf[COM_BUFSIZE];
char recvBuf[COM_BUFSIZE];

typedef enum {START, GET, GET_ACK, SET, STOP, SIGNAL, SIGNAL_ACK} cmd_t;
//char cmd_str[COM_COMMANDS][COM_STRLEN] = {"START", "GET", "GET_ACK", "SET", "STOP"};

#define com_send(str) udpconn_send(conn, str)
#define com_recv() udpconn_receive(conn, recvBuf, sizeof(recvBuf))

void com_cmd(cmd_t cmd, float val);

void com_init() {
    conn = udpconn_new("127.0.0.1", 9999);
    memset(recvBuf, 0, sizeof(recvBuf));
    com_cmd(START, 0);
}

inline void com_close() {
	com_cmd(STOP, 0);
	udpconn_delete(conn);
}

/* TODO: Consider moving away from optional argument val */
void com_cmd(cmd_t cmd, float val) {
	switch(cmd) {
		case START:
			sprintf(sendBuf, "START");
			break;
		case GET:
			sprintf(sendBuf, "GET");
			break;
		case STOP:
			sprintf(sendBuf, "STOP");
			break;
		case SET:
			sprintf(sendBuf, "SET:%3.3f", val);
			break;
		default:
		return;
	}
	printf("UDP sending: %s\n", sendBuf);
	com_send(sendBuf);
}

void com_set(float val) {
	com_cmd(SET, val);
}

void com_get(float* val) {
	com_cmd(GET, 0);
	com_recv();
	char* retval = recvBuf+8;
	*val = atof(retval);
	printf("y = %s\n", retval);
	//printf("%s -> %s -> %f\n", recvBuf, test, test2);
}
