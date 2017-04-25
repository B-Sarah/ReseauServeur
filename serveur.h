#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <ctype.h>

#include "clientHandler.h"

#define MAX_QUEUE_SIZE 100
#define PORT 3000





typedef struct{
	int socket;
	struct sockaddr_in address;
	int addressLength;
	unsigned short port;
	struct hostent* host;
	int isRunning;
}Server;



void initServer(Server* server);
void startServer(Server* server);
void* acceptClients(void* server_);
void stopServer(Server* server);

void handleCommands(Server* server);
char* readline();

#endif
