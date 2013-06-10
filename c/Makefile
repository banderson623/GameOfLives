BINARY=game
CC=cc

game: game.c
	$(CC) game.c -o $(BINARY)

bmp.p : bmp.c
	$(CC) bmp.c -c bmp.o


run: game
	./$(BINARY)

clean: 
	rm -f bmp.o
	rm ./$(BINARY)
	