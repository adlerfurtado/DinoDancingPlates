PASTINHA_CONJUNTA=../
CC = $(PASTINHA_CONJUNTA)\MINGW\bin\gcc
PATH_ALLEGRO=$(PASTINHA_CONJUNTA)\allegro-5.0.10-mingw-4.7.0
LIBS=-lallegro-5.0.10-monolith-mt

all: game.exe
	
game.exe: game.o
	$(CC) -O2 -o game.exe game.o -L $(PATH_ALLEGRO)\lib $(LIBS)

game.o: game.c
	$(CC) -O2 -I $(PATH_ALLEGRO)\include -c game.c 
