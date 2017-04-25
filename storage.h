#ifndef STORAGE_H
#define STORAGE_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

#include "player.h"
#include "character.h"

int retrievePlayer(char* name, Player* player);
int storePlayer(Player* player, int forceOverride);

int retrieveCharacter(long cid, Character* character);
int storeCharacter(Character* character);

void removeChars(char *s, char c);

#endif // STORAGE_H
