#ifndef CHARACTER_H
#define CHARACTER_H

#include <stdio.h>
#include <rpc/xdr.h>
#include <limits.h>
#include <string.h>

typedef struct{
	char pseudo[30];
    char skin[2];
	int hp;
	int x;
	int y;
	long id;
}Character;

bool_t xdr_character(XDR* pt_xdr, Character* character);

long randomCID();
Character* createCharacter(char* name, char* skin, int hp, int x, int y);
void copyCharacter(Character* src, Character* trg);

#endif

