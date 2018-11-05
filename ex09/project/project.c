#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "utilities.h"


#define BUFFER_SIZE (size_t)1024
#define NUM_CLIENTS 4

static int server_priority = 5;
static int client_priorities[] = {2, 3, 6, 7};

void* client(void*);
void* server(void*);

int main(int argc, char *argv[]) {
	int channel_id = ChannelCreate(0);//_NTO_CHF_FIXED_PRIORITY);

	printf("INIT: Channel ID: %d\n", channel_id);
	printf("INIT: Process ID: %d\n", getpid());

	set_priority(server_priority);

	pthread_t client_threads[NUM_CLIENTS];
	pthread_t server_thread;

	int client_args[] = {channel_id};

	int i;
	for (i = 0; i < NUM_CLIENTS; i++) {
		pthread_create(&client_threads[i], NULL, &client, (void*)client_args);
	}

	/* Server */
	int server_args[] = {channel_id};
	pthread_create(&server_thread, NULL, &server, (void*)server_args);

	for (i = 0; i < NUM_CLIENTS; i++) {
		pthread_join(client_threads[i], NULL);
	}
	pthread_join(server_thread, NULL);

	return EXIT_SUCCESS;
}

void* client(void* args) {
	int channel_id = ((int*)args)[0];
	int client_id = pthread_self();
	printf("CLIENT%d: Hello, from client!\n", client_id);

	set_priority(client_priorities[client_id]);
	printf("CLIENT%d: Priority set to %d!\n",
			client_id, get_priority());

	char send_buffer[BUFFER_SIZE] = {0};
	char recv_buffer[BUFFER_SIZE] = {0};

	printf("CLIENT%d: Connecting to channel %d\n", client_id, channel_id);
	int connection_id = ConnectAttach(0, getpid(), channel_id, 0, 0);

	/* Send message to server, and await reply */
	sprintf(send_buffer, "Message for server from %d", client_id);
	int error;
	int attempts = 0;
	do {
		if (attempts < 5) {
			printf("CLIENT%d: Sending message to server: \"%s\"\n",
					client_id, send_buffer);
		}

		error = MsgSend(connection_id,
				send_buffer, BUFFER_SIZE,
				recv_buffer, BUFFER_SIZE);

		if (error && attempts < 5) {
			printf("CLIENT%d: MsgSend ERROR: %s(%d)\n",
					client_id, strerror(errno), errno);
		}

		if (attempts++ > 1000) {
			printf("CLIENT%d: Too many failed attempts. Giving up.\n",
					client_id);
			return (void*)-1;
		}
	} while (error);

	printf("CLIENT%d: Received reply from server: \"%s\"\n",
			client_id, recv_buffer);

	ConnectDetach(connection_id);

	printf("CLIENT%d: Detached the connection.\n",
			client_id);

	return NULL;
}

void* server(void* args) {
	printf("SERVER: Hello, from server!\n");
	int channel_id = ((int*)args)[0];
	char buffer[BUFFER_SIZE];
	struct _msg_info msg_info;

	while(1) {
		/* Wait for message from client */
		printf("\nSERVER(%d): Waiting for message\n", get_priority());
		int receive_id = MsgReceive(channel_id, buffer, BUFFER_SIZE, &msg_info);
		printf("SERVER(%d): Received message: \"%s\"\n",
				get_priority(), buffer);
		printf("SERVER: Process ID: %d\n", msg_info.pid);
		printf("SERVER: Thread ID: %d\n", msg_info.tid);

		/* Reply to client */
		sprintf(buffer, "%suck off, TID%d!", "F", msg_info.tid);
		printf("SERVER: Replying with: \"%s\"\n", buffer);

		MsgReply(receive_id, 0, buffer, BUFFER_SIZE);
	}

	return NULL;
}
