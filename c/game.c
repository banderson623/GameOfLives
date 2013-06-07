#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_WIDTH 10
#define DEFAULT_HEIGHT 10

#define TRUE 1
#define FALSE 0

#define OK 0
#define FAIL 1

// Nice way to keep the data together.
typedef struct {
  int height;
  int width;
  int** tiles;  
} Board;


// Release the board
int releaseBoard(Board* board){
    for (int row = 0; row < board->height; ++row){
        // Walk DOWN the board releasing
        free(board->tiles[row]);  
    }  
    free(board->tiles);
    board->tiles = 0;
    board->width = 0;
    board->height = 0;
    return OK;
}

// Print the board
void printBoard(Board* board){
    for(int row = 0; row < board->height ; ++row){
        for(int column = 0; column < board->width; ++column){
            printf(" %c",(board->tiles[row][column] == 1) ? '*' : ' ');
        }
        printf("\n");
    }
}

Board* allocateBoardTiles(int height, int width){

    // Create a new board
    Board* board = (Board*) malloc(sizeof(Board));
    board->height = height;
    board->width = width;
        
    if(board->width > 0 && board->height > 0){
        // allocate the space, for the rows, to hold an array of integers
        board->tiles = (int**) malloc(board->height * sizeof(int*));
            
        for(int row = 0; row < board->height; ++row){
            
            // Allocate enough size in this board for each row
            board->tiles[row] = (int*) malloc(board->width * sizeof(int));
        }        
    }
    
    return board;
}

// Generates a board with width and height dimensions
// in the 2d array board
// the seed, seeds the random number generator
void generateLifeOn(Board* board, int seed) {
    // seed the random number generator
    srand(seed);
    printf("generating life...\n");
    
    for(int row = 0; row < board->height ; ++row){
        for(int column = 0; column < board->width; ++column){
            // 1 is alive, 0 is dead
            board->tiles[row][column] = rand() % 2;
        }
    }
}

int neighborCount(int column, int row, Board* board){

    int count = 0;
    
    // Here we are going to deviate from how we did this in Ruby.
    // Because C's modulus operator is, arguably, mathematically more correct
    // see: http://stackoverflow.com/questions/7594508/modulo-operator-with-negative-values
    // in how it behaves.
    // -----------------------------------------------------------------------------------
    // For example if we have a board height of 3. 
    // When the row offset is calculating for -1 that would be above the current row.
    // and the current row is 0, then 0 + (-1) % 3 will return -1. Which obviously is
    // not a correct array index. (segfault!)
    // 
    // What we wanted in this case was that it would be 2. So how do we do this?
    // Lets add total height to the row. This will always return a postive number
    // and it is the correct value. Because multiples of the height are ignored
    // by the modulus operator we can safely do this to get us positive, without
    // concern of altering the value returned.
    //
    //     example: (3 + 0 +(-1)) % 3 = 2 
    
    // printf("Checking row: %d and column: %d\n", row, column);
    
    for(int row_offset = -1; row_offset <= 1; row_offset++){
        
        for(int column_offset = -1; column_offset <= 1; column_offset++){
            // printf("Checking row offset: %d and column offset: %d | for row: %d and column: %d\n", row_offset, column_offset, row, column);
            if (!(row_offset == 0 && 
                  column_offset == 0)){
                  int found = board->tiles[((board->height + row + row_offset) % board->height)]
                                  [((board->width + column + column_offset) % board->width)];
                  if(found > 0){
                      // printf("   found neighbor at offset r:%d,c:%d\n",row_offset,column_offset);
                  }
                  count+= found;
            }
        }
    }
    
    return count;
}

Board* evolve(Board* board){
    
    // Build the 2d array
    Board* newBoard = allocateBoardTiles(board->height, board->width);
        
    for(int row = 0; row < board->height ; ++row){
        for(int column = 0; column < board->width; ++column){
            int numberOfNeighbors = neighborCount(column,row,board);
            // printf("neighbors: %d, ",numberOfNeighbors);
            if(numberOfNeighbors < 2 || numberOfNeighbors > 3){
                // Unable to live without neighbors, but not too many ;)
                newBoard->tiles[row][column] = 0;
            } else if (numberOfNeighbors == 3 && board->tiles[row][column] == 0){
                // any dead cell with exactly 3 live neighbours becomes a live cell
                newBoard->tiles[row][column] = 1;
            } else {
                // There is no change:
                //   any live cell with 2 or 3 neighbours lives to next generation
                //   or the cell stays dead
                // printf(".");
                newBoard->tiles[row][column] = board->tiles[row][column];
            }
        }
    }
    releaseBoard(board);
    return newBoard;
}

int main (int argc, char const *argv[]){    
    int height = DEFAULT_HEIGHT;
    int width =  DEFAULT_WIDTH;
    
    Board* board = allocateBoardTiles(height,width);
    
    // generate the board
    generateLifeOn(board, 0);
    
    // print the current board
    printBoard(board);
    
    for(int i = 0; i < 15; i++){
        board = evolve(board);
        printf("Generation: %d\n",i);
        printBoard(board);
        sleep(1);
    }


    
    // Now release the memory
    releaseBoard(board);
    
    return OK;
}
