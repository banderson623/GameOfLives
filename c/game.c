#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_WIDTH 4
#define DEFAULT_HEIGHT 4

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
    
    //Set up the board
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

int neighborCount(int column, int row, Board* board){
    int count = 0;
    
    // Here we are going to deviate from how we did this in ruby.
    // Because C's modulus operator is arguably more mathematically more correct
    // see: http://stackoverflow.com/questions/7594508/modulo-operator-with-negative-values
    // for how it behaves.
    //
    // For example if we have a board height of 3. 
    //  When the row offset is calculating for -1 that would be above the current row.
    //  and the current row is 0, then 0 + (-1) % 3 will return -1. Which obviously is
    //  not a correct array index. (segfault!)
    // 
    //  what we wanted in this case was that it would be 2. So how do we do this?
    //  Lets add total height to the row. This will always return a postive number
    //  and it is the correct value. Because multiples of the height are ignored
    //  by the modulus operator we can safely do this to get us positive, without
    //  concern of altering the value returned.
    //
    //      example: (3 + 0 +(-1)) % 3 = 2 
    // To cop
    
    for(int row_offset = -1; row_offset <= 1; row_offset++){
        for(int column_offset = -1; column_offset <= 1; column_offset++){
            // printf("Checking row offset: %d and column offset: %d | for row: %d and column: %d\n", row_offset, column_offset, row, column);
            if (!(row_offset == 0 && column_offset == 0)){
                  count+= board->board[((board->height + row + row_offset) % board->height)]
                                      [((board->width + column + column_offset) % board->width)];
            }
        }
    }
    
    return count;
}

void evolve(Board* board){
    
    Board newBoard;
    for(int row = 0; row < board->height ; ++row){
        for(int column = 0; column < board->width; ++column){
            printf("Neighbors of %2d,%2d : %d\n",row,column,
                                                 neighborCount(column,row,board));
        }
    }
    // releaseBoard(*board);
}

int main (int argc, char const *argv[]){    
    int height = DEFAULT_HEIGHT;
    int width =  DEFAULT_WIDTH;
    
    // generate the board
    Board board = generateBoard(width, height, 0);
    
    // print the current board
    printBoard(board);
    
    evolve(&board);
    
    // Now release the memory
    releaseBoard(board);
    
    return OK;
}
