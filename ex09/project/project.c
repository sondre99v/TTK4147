#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE (size_t)1024

void* client(void*);
void* server(void*);

int main(int argc, char *argv[]) {
	int channel_id = ChannelCreate(0);

	printf("INIT: Channel ID: %d\n", channel_id);
	printf("INIT: Process ID: %d\n", getpid());

	pthread_t client_thread;

	int client_args[] = {channel_id};
	pthread_create(&client_thread, NULL, &client, (void*)client_args);

	/* Server */
	int server_args[] = {channel_id};
	server((void*)server_args);

	pthread_join(client_thread, NULL);

	return EXIT_SUCCESS;
}

void* client(void* args) {
	printf("CLIENT: Hello, from client!\n");
	int channel_id = ((int*)args)[0];
	char send_buffer[BUFFER_SIZE] = {0};
	char recv_buffer[BUFFER_SIZE] = {0};

	printf("CLIENT: Connecting to channel %d\n", channel_id);
	int connection_id = ConnectAttach(0, getpid(), channel_id, 0, 0);

	/* Send message to server, and await reply */
	strcpy(send_buffer, "Message for server");
	int error;
	int attempts = 0;
	do {
		if (attempts < 5) {
			printf("CLIENT: Sending message to server: \"%s\"\n", send_buffer);
		}

		error = MsgSend(connection_id,
				send_buffer, BUFFER_SIZE,
				recv_buffer, BUFFER_SIZE);

		if (error && attempts < 5) {
			printf("CLIENT: MsgSend ERROR: %s(%d)\n", strerror(errno), errno);
		}

		if (attempts++ > 1000) {
			printf("CLIENT: Too many failed attempts. Giving up.\n");
			return (void*)-1;
		}
	} while (error);

	printf("CLIENT: Received reply from server: \"%s\"\n", recv_buffer);

	return NULL;
}

void* server(void* args) {
	printf("SERVER: Hello, from server!\n");
	int channel_id = ((int*)args)[0];
	char buffer[BUFFER_SIZE];

	while(1) {
		/* Wait for message from client */
		printf("SERVER: Waiting for message\n");
		int receive_id = MsgReceive(channel_id, buffer, BUFFER_SIZE, NULL);
		printf("SERVER: Received message: \"%s\"\n", buffer);

		/* Reply to client */
		sprintf(buffer, "%suck off!", "F");
		printf("SERVER: Replying with: \"%s\"\n", buffer);

		MsgReply(receive_id, 0, buffer, BUFFER_SIZE);
	}

	return NULL;
}

/* Utilities (copyable code) */
/*
int set_priority(int priority){
    int policy;
    struct sched_param param;

    if(priority < 1 || priority > 63){
        return -1;
    }

    pthread_getschedparam(pthread_self(), &policy, &param);
    param.sched_priority = priority;
    return pthread_setschedparam(pthread_self(), policy, &param);
}

int get_priority(){
    int policy;
    struct sched_param param;
    pthread_getschedparam(pthread_self(), &policy, &param);
    return param.sched_curpriority;
}
*/
