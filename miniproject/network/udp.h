
#pragma once

#include <arpa/inet.h>

typedef struct UDPConn UDPConn;
struct UDPConn {
	int sock;
	struct sockaddr_in serverAddr;
};


UDPConn* udpconn_new(char* ip, short port);
void udpconn_delete(UDPConn* udp);

int udpconn_send(UDPConn* udp, char* str);
int udpconn_receive(UDPConn* udp, char* buf, int buflen);


/* Example:

    UDPConn* conn = udpconn_new("127.0.0.1", 9999)
    
    char sendBuf[64];
    char recvBuf[64];    
    memset(recvBuf, 0, sizeof(recvBuf));
    
    sprintf(sendBuf, "Message:%d", 17);    
    udpconn_send(conn, sendBuf);
    
    udpconn_recv(conn, recvBuf, sizeof(recvBuf));
    
    udpconn_delete(conn);
*/
