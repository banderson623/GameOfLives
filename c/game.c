#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_WIDTH 50
#define DEFAULT_HEIGHT 10

#define TRUE 1
#define FALSE 0

#define OK 0
#define FAIL 1

// I've been using C++ too long :)
typedef struct {
  int height;
  int width;
  int** board;  
} Board;

int releaseBoard(Board board);
void printBoard(Board board);
Board generateBoard(int width, int height, int seed);

int main (int argc, char const *argv[]){    
    int height = DEFAULT_HEIGHT;
    int width =  DEFAULT_WIDTH;
    
    // generate the board
    Board board = generateBoard(width, height, 0);
    
    // print the current board
    printBoard(board);
    
    // Now release the memory
    releaseBoard(board);
    
    return OK;
}


// Release the board of height (rows)
int releaseBoard(Board board){
    for (int row = 0; row < board.height; ++row){
        // Walk DOWN the board releasing
        free(board.board[row]);  
    }  
    free(board.board);
    return OK;
}

// Print the board
void printBoard(Board board){
    for(int row = 0; row < board.height ; ++row){
        for(int column = 0; column < board.width; ++column){
            printf("%c",(board.board[row][column] == 1) ? '*' : ' ');
        }
        printf("\n");
    }
}

// Generates a board with width and height dimensions
// in the 2d array board
// the seed, seeds the random number generator
Board generateBoard(int width, int height, int seed) {
    // seed the random number generator
    srand(seed);
    
    Board board;
    board.width = width;
    board.height = height;
        
    // allocate the space, for the rows, to hold an array of integers
    board.board = (int**) malloc(height * sizeof(int*));
    
    for(int row = 0; row < height ; ++row){

        // Allocate enough size in this board for each row
        board.board[row] = (int*) malloc(width * sizeof(int));

        for(int column = 0; column < width; ++column){
            // 1 is alive, 0 is dead
            board.board[row][column] = rand() % 2;
        }
    }
    
    return board;
}
