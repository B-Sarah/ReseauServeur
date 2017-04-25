#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <string.h>
#include <rpc/xdr.h>

typedef struct{
	char name[30];
	char password[20];
	long character_id;
	int logged;
	int newAccount;
}Player;

int passwordValid(Player* player, char* password);
void copyPlayer(const Player* src, Player* dst);
bool_t xdr_player(XDR* pt_xdr, Player* player);

#endif
