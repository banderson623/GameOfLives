BINARY=game
CC=cc

all: game

game: bmp.o game.c
	$(CC) bmp.o game.c -lncurses -o $(BINARY)

bmp.o: bmp.c
	$(CC) -c -Wall bmp.c

run: game
	./$(BINARY)

clean: 
	rm -f *.o
	rm ./$(BINARY)
	rm -f images/*.bmp
	
