BINARY=game
CC=cc

game: game.c
	$(CC) game.c -lncurses -o $(BINARY)

run: game
	./$(BINARY)

clean: 
	rm ./$(BINARY)