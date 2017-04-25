#include "player.h"

/* xdr_Player */

bool_t xdr_player(XDR* pt_xdr, Player* player){
	char* pName = player->name;
	char* pPwd = player->password;
	char** ppName = &pName;
	char** ppPwd = &pPwd;
	int* logged = &player->logged;
	int* newAccount = &player->newAccount;
	long* charId = &player->character_id;

	return(xdr_string(pt_xdr, ppName, 30) &&
			xdr_string(pt_xdr, ppPwd, 20) &&
                  xdr_long(pt_xdr, charId) &&
                  xdr_int(pt_xdr, logged) &&
                  xdr_int(pt_xdr, newAccount));

}

int passwordValid(Player* player, char* password){
      return strcmp(player->password, password) == 0;
}

void copyPlayer(const Player* src, Player* dst){
	strcpy(dst->name, src->name) ;
	strcpy(dst->password, src->password);
	dst->character_id = src->character_id;
	dst->logged = src->logged;
	dst->newAccount = src->newAccount;
}
