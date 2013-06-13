BINARY=game
CC=cc

all: game

game: game.c
	$(CC) game.c -lncurses -lpthread -o $(BINARY)

pipe: pipe_example.c
	$(CC) pipe_example.c -lpthread -o pipe

run: game
	./$(BINARY)

clean: 
	rm -f *.o
	rm ./$(BINARY)
	rm ./pipe
	
