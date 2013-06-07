#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_WIDTH 50
#define DEFAULT_HEIGHT 10

#define TRUE 1
#define FALSE 0

#define OK 0
#define FAIL 1

int releaseBoard(int height, int** board);
void printBoard(int width, int height, int** board);
int** generateBoard(int width, int height, int seed);

int main (int argc, char const *argv[]){    
    int height = DEFAULT_HEIGHT;
    int width =  DEFAULT_WIDTH;
    
    // generate the board
    int** board = generateBoard(width, height, 0);
    
    // print the current board
    printBoard(width,height,board);
    
    // Now release the memory
    releaseBoard(height,board);
    
    return OK;
}


// Release the board of height (rows)
int releaseBoard(int height, int** board){
    for (int row = 0; row < height; ++row){
        // Walk DOWN the board releasing
        free(board[row]);  
    }  
    free(board);
    return OK;
}

// Print the board
void printBoard(int width, int height, int** board){
    for(int row = 0; row < height ; ++row){
        for(int column = 0; column < width; ++column){
            printf("%c",(board[row][column] == 1) ? '*' : ' ');
        }
        printf("\n");
    }
}

// Generates a board with width and height dimensions
// in the 2d array board
// the seed, seeds the random number generator
int** generateBoard(int width, int height, int seed) {
    // seed the random number generator
    srand(seed);
    
    // allocate the space, for the rows, to hold an array of integers
    int** board = (int**) malloc(height * sizeof(int*));
    
    for(int row = 0; row < height ; ++row){

        // Allocate enough size in this board for each row
        board[row] = (int*) malloc(width * sizeof(int));

        for(int column = 0; column < width; ++column){
            // 1 is alive, 0 is dead
            board[row][column] = rand() % 2;
        }
    }
    
    return board;
}
