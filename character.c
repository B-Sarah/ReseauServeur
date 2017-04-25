#include "character.h"

/* xdr_Character */

bool_t xdr_character(XDR* pt_xdr, Character* character){
	char* pPseudo = character->pseudo;
	char** ppPseudo = &pPseudo;
    char* pskin = character->skin;
    char** ppSkin = &pskin;
	int* hp = &(character->hp);
	int* x = &(character->x);
	int* y = &(character->y);
	long* id = &(character->id);

	return(xdr_string(pt_xdr, ppPseudo, 30) &&
            xdr_string(pt_xdr, ppSkin, 2) &&
			xdr_int(pt_xdr, hp) &&
			xdr_int(pt_xdr, x) &&
			xdr_int(pt_xdr, y) &&
            xdr_long(pt_xdr, id));

}

long randomCID(){
    return rand() % LONG_MAX;
}

Character* createCharacter(char* name, char* skin, int hp, int x, int y){
    Character* ch = (Character*)malloc(sizeof(Character));
    strcpy(ch->pseudo, name);
    strcpy(ch->skin, skin);
    ch->hp = hp;
    ch->x = x;
    ch->y = y;
    ch->id = randomCID();
    return ch;
}

void copyCharacter(Character* src, Character* trg){
    strcpy(trg->pseudo, src->pseudo);
    strcpy(trg->skin, src->skin);
    trg->hp = src->hp;
    trg->x = src->x;
    trg->y = src->y;
    trg->id = src->id;
}
