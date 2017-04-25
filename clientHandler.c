#include "clientHandler.h"

Client* clients[MAX_CLIENT];

void startHandlingClient(Client* client){
	int pthread_return;
	pthread_t thread;

	//creation de thread d'acceptation de clients
	pthread_return = pthread_create(&thread, NULL, handleClient, (void*)client);

	if(pthread_return){
		printf("ERROR; return code from pthread_create() is %d\n", pthread_return);
		return;
	}
}

void* handleClient(void* client_){
    char received[MAX_MSG_SIZE] = {0};
    XDR xdr_decode;

    xdrmem_create(&xdr_decode, received + 1, MAX_MSG_SIZE-1, XDR_DECODE);

	Client* client = (Client*)client_;
    client->isRunning = FALSE;
	//Un client est la !!
	printf("Un client vient de se connecter a la socket %d\n", client->socket);
	fflush(stdout);

    int slot = findFreeClientSlot();
    if(slot != -1){
        client->isRunning = TRUE;
        clients[slot] = client;
    }
    else{
        printf("No slot left, force client disconnect... \n");
        disconnectClient(client);
    }
    printf("Client placed on slot %d\n", slot);

    int readLength = 0;
    //TODO condition a changer: si player s'est deconnecté, ou si deconnexion forcé
    while(client->isRunning){
	xdr_setpos(&xdr_decode, 0);
        if((readLength = readClientInput(client, &received, MAX_MSG_SIZE)) <= 0){
            break;
        }

        if(handleClientInput(client, &xdr_decode, received, readLength) != 0){
			break;
		}
    }

    disconnectClient(client);

	pthread_exit(NULL);
}



int readClientInput(Client* client, char (*buffer)[], int bufferSize){
    memset(buffer, 0, MAX_MSG_SIZE);

    int length = 0;

    if((length = read(client->socket, *buffer, bufferSize)) < 0){
        perror("read");
        return -1;
    }
    return length;
}

int handleClientInput(Client* client, XDR* data, char* rawData, int rawDataLength){
//	int decodeResult = -1;
    Player pDecoded = {0};
    Character cDecoded = {0};
    Object oDecoded = {0};

    //printf("size player %ld, sizedecode %ld\n", sizeof(Player), sizeof(rawData));

    //decodeResult = testDecode(data, &pDecoded, &cDecoded, &oDecoded);
    //printf("Decode result %d\n", decodeResult);

//	printf("rawData %c\n",rawData[0]);

	switch(rawData[0]){
	      //received a character : update stored and notify all
      case 'c':
        //printf("Reception character\n");
	    xdr_character(data, &cDecoded);
//	    printf("character : %s %s\n",cDecoded.pseudo, cDecoded.skin);
            handleCharacterInput(client, &cDecoded);
            break;
            //received an object : update stored and notify all
      case 'o':
            //printf("Reception object\n");
            break;
            //received a player : login check
      case 'p':
        //printf("Reception player\n");
            xdr_player(data, &pDecoded);
//	    printf("player received : %s %d\n",pDecoded.name, pDecoded.logged);
            handlePlayerInput(client, &pDecoded);
            break;
            //received another statement
      case NOT_ENCODED:
            //printf("not encoded data");
            handleNotEncoded(client, rawData);
            break;
	}

	

	//TODO ajouter traitement
    return 0;
}

int handlePlayerInput(Client* client, Player* player){
      char sent[MAX_MSG_SIZE];
      XDR xdr_encode;
      Player* retrieved = (Player*)malloc(sizeof(Player));
      int returned = 0;
      client->character = NULL;

      xdrmem_create(&xdr_encode, sent + 1, MAX_MSG_SIZE-1, XDR_ENCODE);

	client->player = player;

      //new account requested
      if(player->newAccount){
            printf("account creation...\n");
            returned = storePlayer(player, 0);
            if(returned == 0){
                player->newAccount = 1;
                player->logged = 1;
                printf("account creation succeed\n");
                Character* c = createCharacter(player->name, "@", 100, 0, 0);
                player->character_id = c->id;
                client->player = player;
                storeCharacter(c);
                storePlayer(player, 1);
                free(c);
                printf("created a new character\n");
            }
            else{
                player->newAccount = 1;
                player->logged = 0;
		client->player = player;
                printf("account creation failed\n");
            }
      }
      else{
            returned = retrievePlayer(player->name, retrieved);
            //correct username
            if(returned){
                  //incorrect password
                if(passwordValid(player, retrieved->password) == 1){
                        retrieved->logged = 1;
                        retrieved->newAccount = 0;
			copyPlayer(retrieved,  client->player);

                        client->character = (Character*)malloc(sizeof(Character));
                        retrieveCharacter(player->character_id, client->character);
            		//printf("Connexion succes, renvoi du player avec logged 1 et character %ld\n", client->character->id);
                } //password ok, login succeed
                else {
                    printf("Incorrect password\n");
                }
            } else {
                printf("Nom d'utilisateur inexistant");
            }
      }

      if(!xdr_player(&xdr_encode, client->player)){
            return 0;
      }

      sent[0] = 'p'; 
      sendMessageTo(client, sent, xdr_getpos(&xdr_encode)+1);


      if(client->player->newAccount == 1){
          client->isRunning = 0;

      }

      if(client->character != NULL){
   	  //printf("Renvoi d'un character après login\n");
          memset(sent, '\0', MAX_MSG_SIZE);
          xdr_setpos(&xdr_encode, 0);
	  sent[0] = 'c'; 

          if(!xdr_character(&xdr_encode, client->character)){
                return 0;
          }

          sendMessageTo(client, sent, xdr_getpos(&xdr_encode)+1);
          sendMessageToAll(client, sent, xdr_getpos(&xdr_encode)+1);
	  int i;
	  for(i = 0; i < MAX_CLIENT; i++){
	      if(clients[i] != client && clients[i] != NULL && clients[i]->character != NULL){
		  
		   	  //printf("Renvoi d'un character après login\n");
			  memset(sent, '\0', MAX_MSG_SIZE);
			  xdr_setpos(&xdr_encode, 0);
			  sent[0] = 'c'; 

			  if(!xdr_character(&xdr_encode, clients[i]->character)){
				return 0;
			  }

			  sendMessageTo(client, sent, xdr_getpos(&xdr_encode)+1);
		}
	  }
      }
      return 1;
}

int handleCharacterInput(Client *client, Character *character){
    char sent[MAX_MSG_SIZE];
    XDR xdr_encode;

    xdrmem_create(&xdr_encode, sent+1, MAX_MSG_SIZE - 1, XDR_ENCODE);
    //printf("handle character !!\n");

    if(character->x == -1){
        //printf("x !! %d\n", character->x);
        client->character = character;
        retrieveCharacter(client->character->id, client->character);

        if(!xdr_character(&xdr_encode, client->character)){
              return 0;
        }
        sent[0] = 'c';
        sendMessageTo(client, sent, xdr_getpos(&xdr_encode)+1);
    }
    /*else if(character->hp == 0){
        sent[0] = 'c';
        if(!xdr_character(&xdr_encode, character)){
          return 0;
        }
        sendMessageTo(client, sent, xdr_getpos(&xdr_encode)+1);
    }*/
    else{
        if(!xdr_character(&xdr_encode, client->character)){
              return 0;
        }

        copyCharacter(character, client->character);
        storeCharacter(client->character);
        sent[0] = 'c';
	printf("Update d'un character après mouvement\n");
        sendMessageToAll(client, sent, xdr_getpos(&xdr_encode)+1);
    }


    xdr_destroy(&xdr_encode);

    return 1;
}

int handleNotEncoded(Client* client, char* rawData){
    //NOT USED
    /*printf("... received : %s\n", rawData);
    if(strcmp(rawData, "disconnect") == 0){
        disconnectClient(client);
    }*/
    return 0;
}

int testDecode(XDR* decoder, Player* pEncoded, Character* cEncoded, Object* oEncoded){
      if(xdr_player(decoder, pEncoded)) return DECODED_PLAYER;
      else if(xdr_character(decoder, cEncoded)) return DECODED_CHARACTER;

      else if(xdr_object(decoder, oEncoded)) return DECODED_OBJECT;
      else return NOT_ENCODED;
}



void sendMessageTo(Client* client, char* msg, int msgSize){
  usleep(200);
    //printf("envoi de message...\n");
	if(send(client->socket, msg, msgSize, 0) != msgSize){
		perror("write");
	}
}

void sendMessageToAll(Client* sender, char* msg, int msgSize){
	int i;
	for(i = 0; i < MAX_CLIENT; i++){
        if(clients[i] != NULL && clients[i] != sender){
			sendMessageTo(clients[i], msg, msgSize);
		}
	}
}

int findFreeClientSlot(){
      int i;
      for(i = 0; i < MAX_CLIENT; i++){
            if(clients[i] == NULL)
                  return i;
      }
      return -1;
}

int findClientSlot(Client* client){
    int i;
    for(i = 0; i < MAX_CLIENT; i++){
          if(clients[i] == client)
                return i;
    }
    return -1;
}

void disconnectClient(Client* client){
    if(client == NULL) return;

    int slot = findClientSlot(client);
    printf("Un client vient de se deconnecter de la socket %d au slot %d\n", client->socket, slot);

    removePlayerFromGame(client);

    fflush(stdout);
    client->isRunning = FALSE;
    close(client->socket);
    client->socket = 0;
    if(client != NULL) free(client);
    client = NULL;
    if(slot != -1) clients[slot] = NULL;
}

void removePlayerFromGame(Client* client){
    if(client->player != NULL){
        //free(client->player);
        client->player = NULL;
    }
    if(client->character != NULL){
        //free(client->character);
        client->character = NULL;
    }
    //TODO notify all that disconnected
}

void displayClients(){
    int i;
    for(i = 0; i < MAX_CLIENT; i++){
        if(clients[i] != NULL){
            if(clients[i]->player != NULL)
                printf("Client[%d] : socket(%d) username(%s) character_id(%ld)\n", i, clients[i]->socket, clients[i]->player->name, clients[i]->player->character_id);
            else
                printf("Client[%d] : socket(%d) not logged\n", i, clients[i]->socket);
        }
    }
}

void displayCharacters(){
    int i;
    for(i = 0; i < MAX_CLIENT; i++){
        if(clients[i] != NULL){
            if(clients[i]->character != NULL)
                printf("Client[%d] : pseudo(%s) pos(%d,%d) hp(%d) cid(%ld)\n", i, clients[i]->character->pseudo, clients[i]->character->x, clients[i]->character->y, clients[i]->character->hp, clients[i]->character->id);
        }
    }
}

int getNumberConnected(){
    int i;
    int number = 0;
    for(i = 0; i < MAX_CLIENT; i++){
          if(clients[i] != NULL)
                number++;
    }
    return number;
}
