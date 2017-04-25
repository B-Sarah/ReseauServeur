
#include "storage.h"

int retrievePlayer(char* name, Player* player){
      FILE* file;
      char* readLine = NULL;
      size_t lineSize;
      int returned = 1;
      int fullSize = strlen("players/") + strlen(name);
      //get full file name
      char path[100] = {"players/"};
      strcat(path, name);
      path[fullSize + 1] = '\0';

      //if player does not exists
      if( (file = fopen(path, "r")) == NULL){
          perror("fopen");
            return -2;
      }

      player->logged = 0;
      player->newAccount = 0;

      //1st is name
      returned = getline(&readLine, &lineSize, file);
      removeChars(readLine, '\n');
      strcpy(player->name, readLine);

      //2nd is pwd
      returned = returned && getline(&readLine, &lineSize, file);
      removeChars(readLine, '\n');
      strcpy(player->password, readLine);

      //3rd is character id
      returned = returned && getline(&readLine, &lineSize, file);
      removeChars(readLine, '\n');
      player->character_id = strtol(readLine, NULL, 10);

      fclose(file);

      return returned;
}

void removeChars(char *s, char c){
    int writer = 0, reader = 0;

    while (s[reader])
    {
        if (s[reader]!=c)
        {
            s[writer++] = s[reader];
        }
        reader++;
    }
    s[writer]=0;
}


int storePlayer(Player* player, int forceOverride){
      FILE* file;

      //get full file name
      char dir[] = "players/";
      char path[100] = {0};
      snprintf(path, sizeof path, "%s%s", dir, player->name);

      if(!forceOverride && access(path, F_OK) != -1) return -1;

      //if player does not exists
      if( (file = fopen(path, "w")) == NULL){
            return -1;
      }

      fprintf(file, "%s\n", player->name);
      fprintf(file, "%s\n", player->password);
      fprintf(file, "%ld\n", player->character_id);

      fclose(file);
      return 0;
}

int retrieveCharacter(long cid, Character* character){
    FILE* file;
    char* readLine = NULL;
    size_t lineSize;
    int returned = 1;

	printf("retrieve : cid(%ld)\n", cid);

    char id[100] = {0};
    sprintf(id, "%ld", cid);
    int fullSize = strlen("characters/") + strlen(id);
    //get full file name
    char path[100] = {"characters/"};
    strcat(path, id);
    path[fullSize + 1] = '\0';

    //if player does not exists
    if( (file = fopen(path, "r")) == NULL){
        perror("fopen");
          return -2;
    }

    //1st is name
    returned = getline(&readLine, &lineSize, file);
    removeChars(readLine, '\n');
    strcpy(character->pseudo, readLine);

	printf("retrieve : pseudo(%s)\n", character->pseudo);

    //2nd is skin
    returned = returned && getline(&readLine, &lineSize, file);
    removeChars(readLine, '\n');
    strcpy(character->skin, readLine);

    //3rd is hp
    returned = returned && getline(&readLine, &lineSize, file);
    character->hp = atoi(readLine);

    //4th is x
    returned = returned && getline(&readLine, &lineSize, file);
    character->x = atoi(readLine);

    //5th is y
    returned = returned && getline(&readLine, &lineSize, file);
    character->y = atoi(readLine);

    //finally id is passed
    character->id = cid;

    fclose(file);
    return returned;
}
int storeCharacter(Character* character){
    FILE* file;

    //get full file name
    char dir[] = "characters/";
    char path[100] = {0};
    snprintf(path, sizeof path, "%s%ld", dir, character->id);

    //if error opening file
    if( (file = fopen(path, "w")) == NULL){
          return -1;
    }

    fprintf(file, "%s\n", character->pseudo);
    fprintf(file, "%c\n", character->skin[0]);
    fprintf(file, "%d\n", character->hp);
    fprintf(file, "%d\n", character->x);
    fprintf(file, "%d\n", character->y);
    fprintf(file, "%ld\n", character->id);

    fclose(file);
    return 0;
}
