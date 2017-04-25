#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

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
#include <time.h>
#include <rpc/xdr.h>

#define MAX_MSG_SIZE 1024
#define MAX_CLIENT 50

#include "config.h"
#include "player.h"
#include "character.h"
#include "object.h"

#include "storage.h"

#define DECODED_PLAYER 0
#define DECODED_CHARACTER 1
#define DECODED_OBJECT 2
#define NOT_ENCODED 3


typedef struct{
	int socket;
	struct sockaddr_in address;
	int addressLength;
    int isRunning;
    Player* player;
    Character* character;
}Client;

extern Client* clients[MAX_CLIENT];


void startHandlingClient(Client* client);
void* handleClient(void* client_);

int readClientInput(Client* client, char (*buffer)[], int bufferSize);
int handleClientInput(Client* client, XDR* data, char *rawData, int rawDataLength);

int handlePlayerInput(Client* client, Player* player);
int handleCharacterInput(Client* client, Character* character);
int handleNotEncoded(Client* client, char* rawData);

int testDecode(XDR* decoder, Player* pEncoded, Character* cEncoded, Object* oEncoded);

void sendMessageToAll(Client *sender, char* msg, int msgSize);
void sendMessageTo(Client* client, char* msg, int msgSize);

int findFreeClientSlot();
int findClientSlot(Client* client);

void disconnectClient(Client* client);
void removePlayerFromGame(Client* client);


int getNumberConnected();
void displayClients();
void displayCharacters();
#endif
