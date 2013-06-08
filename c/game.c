#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>

// #define DEFAULT_WIDTH 100
// #define DEFAULT_HEIGHT 30

#define TRUE 1
#define FALSE 0


// Nice way to keep the data together.
typedef struct {
  int height;
  int width;
  int** tiles;  
} Board;

// Structure to hold the screen size coordinates
typedef struct {
  int height;
  int width;
} ScreenSize;


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
    return 0;
}

// Print the board
void printBoard(Board* board){
    for(int row = 0; row < board->height ; ++row){
        // printf("|");
        for(int column = 0; column < board->width; ++column){
            printf("|%c",(board->tiles[row][column] == 1) ? '*' : ' ');
        }
        printf("|\n");
    }
}

void printWithCurses(Board* board){
    for(int row = 0; row < board->height ; ++row){
        for(int column = 0; column < board->width; ++column){
            char ch = (board->tiles[row][column] == 1) ? '*' : ' ';
            mvaddch(row,column,ch);
        }
    }
    
}

// Print the board and highlight a row and column
void printBoardWithHighlight(Board* board, int rowToHighlight, int columnToHighlight){
    for(int row = 0; row < board->height ; ++row){
        printf("|");
        for(int column = 0; column < board->width; ++column){
            if(row == rowToHighlight && column == columnToHighlight){
                printf("%s",(board->tiles[row][column] == 1) ? "[*]" : "[ ]");
            } else {
                printf(" %c ",(board->tiles[row][column] == 1) ? '*' : ' ');
            }
        }
        printf("|\n");
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
        
    for(int row_offset = -1; row_offset <= 1; row_offset++){
        
        for(int column_offset = -1; column_offset <= 1; column_offset++){
            if (!(row_offset == 0 && column_offset == 0)){
                  count+= board->tiles[((board->height + row + row_offset) % board->height)]
                                  [((board->width + column + column_offset) % board->width)];
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

// Used to get the screen size
ScreenSize determineScreenSize(){
    // Used to get the screen size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    ScreenSize window;
    window.height = w.ws_row;
    window.width = w.ws_col;
    return window;
}

// To catch the control c signal
int execute;

// Small function that is called when SIGINT is received
void trap(int signal){ 
    execute = 0; 
}

int main (int argc, char const *argv[]){    
    
    int seed = 100;
    if (argc > 1){
        seed = atoi(argv[1]);
    }
    
    ScreenSize size = determineScreenSize();
    
    // ncurses initialization
    initscr();
        
    // Build the board
    Board* board = allocateBoardTiles(size.height,size.width);
    
    // generate the board
    generateLifeOn(board, seed);

    // Counter for the generation
    int generation = 0;
    
    //set up the signal listening
    signal(SIGINT, &trap);
    execute = 1;
    while(execute == 1){
        board = evolve(board);
        printWithCurses(board);
        mvprintw(0, 0, "Window: [%dx%d] - Seed: %d - Generation: %d",size.height, size.width, seed, generation++);
        refresh();
        usleep(1000*10);
    }

    // Now release the memory of the last board
    releaseBoard(board);
    
    signal(SIGINT, SIG_DFL);
    
    // Finally clean up after ncurses
    endwin();
    
    return 0;
}
