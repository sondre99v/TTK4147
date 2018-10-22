#include "communication.h"
//#include "../lib/network/udp.h"
#include "lib/network/udp.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define GET_ACK_VALUE_OFFSET 8
#define COM_BUFSIZE		64

#ifndef RUN_ON_LOCALHOST
#define SERVER_IP "192.168.0.1"
#else
#define SERVER_IP "127.0.0.1"
#endif
#define SERVER_PORT 9999

#define UDP_SEND(str) udpconn_send(conn, (str))
#define UDP_RECV() udpconn_receive(conn, recvBuf, sizeof(recvBuf))


static UDPConn* conn;

static char sendBuf[COM_BUFSIZE];
static char recvBuf[COM_BUFSIZE];


void com_init() {
	printf("Server: %s:%d\n", SERVER_IP, SERVER_PORT);
    conn = udpconn_new(SERVER_IP, SERVER_PORT);
    memset(recvBuf, 0, sizeof(recvBuf));
    com_send_command(START, 0);
}

void com_close() {
	com_send_command(STOP, 0);
	udpconn_delete(conn);
}

com_cmd_t com_receive_command(float* value) {
	UDP_RECV();

	//printf("Received: %s\n", recvBuf);

	switch(recvBuf[0]) {
		case 'S': 
			return SIGNAL;
		case 'G': 
			*value = atof(&recvBuf[GET_ACK_VALUE_OFFSET]);
			return GET_ACK;
		default: 
			return ERROR;
	}
}

void com_send_command(com_cmd_t cmd, float value) {
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
		case SIGNAL_ACK:
			sprintf(sendBuf, "SIGNAL_ACK");
			break;
		case SET:
			snprintf(sendBuf, COM_BUFSIZE, "SET:%f", value);
			break;
		default:
			return;
	}

	//printf("Sending: %s\n", sendBuf);
	UDP_SEND(sendBuf);
}
