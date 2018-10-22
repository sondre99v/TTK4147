

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "udp.h"


UDPConn* udpconn_new(char* ip, short port){

    UDPConn* conn = malloc(sizeof(*conn));
    memset(conn, 0, sizeof(*conn));
    
    conn->serverAddr.sin_family = AF_INET;
    conn->serverAddr.sin_port = htons(port);
    inet_aton(ip, &conn->serverAddr.sin_addr);
    
    conn->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    return conn;
}

int udpconn_send(UDPConn* conn, char* str){
    return sendto(conn->sock, str, strlen(str)+1, 0, (struct sockaddr*)&(conn->serverAddr), sizeof(conn->serverAddr));
}

int udpconn_receive(UDPConn* conn, char* buf, int buflen){
    int res = recvfrom(conn->sock, buf, buflen, 0, NULL, 0);
    return res;
}

void udpconn_delete(UDPConn* conn){
    close(conn->sock);
    free(conn);
}
