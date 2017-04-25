serveur: storage.o serveur.o player.o object.o config.h clientHandler.o character.o
	gcc -o serveur storage.o serveur.o player.o object.o clientHandler.o character.o -lpthread
storage.o: storage.c storage.h
serveur.o: serveur.c serveur.h
player.o: player.c player.h
object.o: object.c object.h
clientHandler.o: clientHandler.c clientHandler.h
character.o: character.c character.h

clean:
	@rm *.o
