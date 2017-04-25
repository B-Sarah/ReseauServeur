#include "serveur.h"

void initServer(Server* server){
	if(server == NULL) exit(1);

	// creation de la socket de RV
	if((server->socket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(2);
	}

	// preparation de l'adresse locale
	server->port = (unsigned short)PORT;

	server->address.sin_family = AF_INET;
	server->address.sin_port = htons(server->port);
	server->address.sin_addr.s_addr = htonl(INADDR_ANY);
	server->addressLength = sizeof(server->address);

	//attachement de la socket a l'adresse locale
	if(bind(server->socket, (struct sockaddr*)(&(server->address)), server->addressLength) == -1){
		perror("bind");
		exit(3);
	}

	//declaration d'ouverture du service
	if(listen(server->socket, MAX_QUEUE_SIZE) == -1){
		perror("listen");
		exit(4);
	}
	server->isRunning = FALSE;
}



void startServer(Server* server){
	if(server == NULL) exit(1);

	int pthread_return;
	pthread_t thread;

	server->isRunning = TRUE;

	//creation de thread d'acceptation de clients
	pthread_return = pthread_create(&thread, NULL, acceptClients, (void*)server);

	if(pthread_return){
		printf("ERROR; return code from pthread_create() is %d\n", pthread_return);
		exit(6);
	}

}

void* acceptClients(void* server_){
	Server* server = (Server*) server_;
	Client* client = NULL;

	//boucle d'attente de connexion
	while(server->isRunning){
		printf("Waiting for client...\n");
		fflush(stdout);

		client = (Client*) malloc(sizeof(Client));
		if(client == NULL) continue;

		client->addressLength = sizeof(client->address);
		client->socket = accept(server->socket, (struct sockaddr*)(&(client->address)), &(client->addressLength));

		if(client->socket == -1 && errno == EINTR){
			//Petite erreur
			continue;
		}

		if(client->socket == -1){
			//erreur grave
			server->isRunning = FALSE;
			pthread_exit((void*)-1);
		}

		startHandlingClient(client);

	}

	pthread_exit((void*)0);
}

void stopServer(Server* server){
	if(server == NULL) exit(1);

	printf("Shutting down server...\n");
	fflush(stdout);

	server->isRunning = FALSE;

	close(server->socket);
    //shutdown(server->socket, );

	printf("Server has stopped.\n");
	fflush(stdout);
}

void handleCommands(Server* server){
    char read[256];
	while(server->isRunning){
        printf("$");
        scanf("%s", read);

        if(strcmp(read, "shutdown") == 0){
            stopServer(server);
        }
        else if(strcmp(read, "status") == 0){
            printf("******* Status *******\n");
            printf("Number of clients : %d\n", getNumberConnected());
            printf("**********************\n");
        }
        else if(strcmp(read, "connected") == 0){
            printf("******* Players connected *******\n");
            displayClients();
            printf("*********************************\n");
        }
        else if(strcmp(read, "characters") == 0){
            printf("******* Characters *******\n");
            displayCharacters();
            printf("**************************\n");
        }
        else if(strcmp(read, "help") == 0){
            printf("status - get server current infos\n");
            printf("connected - list the connected players\n");
            printf("characters - list the characters in game\n");
            printf("shutdown - quit the server\n");
        }
        else{
            printf("Invalid command (help to see commands)\n");
        }

	}

}



int main(int argc, char** argv){

    time_t t;
    srand((unsigned)time(&t));

    Server server;

	//initialisation du serveur
	initServer(&server);

	//lancement du serveur
	startServer(&server);
    handleCommands(&server);

	return 0;
}

