BINARY=game
CC=cc

all: game

game: game.c
	$(CC) game.c -lncurses -o $(BINARY)

run: game
	./$(BINARY)

clean: 
	rm -f *.o
	rm ./$(BINARY)
	
