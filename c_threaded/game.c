#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define EGYPTIAN_COTTON 5
#define THREAD_COUNT EGYPTIAN_COTTON
// That was cute.

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

// This holds
typedef struct {
    Board* current;                 // pointer to the current board
    Board* next;                    // this is the one to write too...
    short running;                  // 0 for stop request, 1 for running
    sem_t* lock;                    // semaphore for locking this structure
    sem_t* evolutionDone;           // Used to signal when an evolution is done
    int rowsRemainingToProcess;     // Number of rows (also their indices of 
                                    // the rows remaining to process).

    int rowsProcessed;              // Count of the rows processed
    int totalRowCount;              // 
} ThreadControl;

// -------- GLOBAL STATE -----------------------------------
// Set up the thread control structure
// sorry it is global
ThreadControl* state;

// To catch the control c signal
int execute;
// ---------------------------------------------------------

// The rules of the game go here,
// in fact this could be a function pointer and
// then we could swap in different rule engines...
int liveOrDie(int numberOfNeighbors, int currentState){
    // die is 0 and live is 1
    int futureState = currentState;
    
    if(numberOfNeighbors < 2 || numberOfNeighbors > 3){
    // Unable to live without neighbors, but not too many ;)
        futureState = 0;
    
    } else if (numberOfNeighbors == 3 && currentState == 0){
    // any dead cell with exactly 3 live neighbours becomes a live cell
        futureState = 1;
    }
    return futureState;
}

// NOT THREAD SAFE
int getNextRow(){
    int returnValue = -1;
    if(state->rowsRemainingToProcess >= 0){
        returnValue = state->rowsRemainingToProcess;
        state->rowsRemainingToProcess--;
    }
    return returnValue;
}

void markRowDone(int row){
    sem_wait(state->lock);          // Lock the state
    state->rowsProcessed++;
    if(state->rowsProcessed >= state->totalRowCount){
        sem_post(state->evolutionDone);
    }
    sem_post(state->lock);      // Release the lock on the state          
}

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

// Print with ncurses, lots of fun!
void printWithCurses(Board* board){
    for(int row = 0; row < board->height ; ++row){
        for(int column = 0; column < board->width; ++column){
            char ch = (board->tiles[row][column] == 1) ? '*' : ' ';
            // print a character at the row and column
            mvaddch(row,column,ch);
        }
    }
    
}

// Returns a new board of height and width
// no life will be present on it though...
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
            // board->tiles[row][column] = 1;
        }
    }
}

int neighborCount(int column, int row, Board* board){
    int count = 0;
    printf("Neighbor count row: %d, col: %d\n",row,column);
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

// This will take the current board, examine the row passed in
// and then add that row to the nextGeneration board.
// Both boards should be initialized and allocated 
void* evolveRowWorker(void* threadId){
    long myId = (long)threadId;
    
    while(state->running){
        sem_wait(state->lock);          // Lock the state
        int canProcess = state->current != NULL && state->next != NULL;
        int row = getNextRow() - 1;
        sem_post(state->lock);      // Release the lock on the state        
        
        if(canProcess && row >= 0){ 
            printf("Processing row:%d, thread: %ld\n",row, myId);
              
            // for each cell in this row
            for(int cell = 0; cell < state->current->width; cell++){
                printf("[t: %ld] -> cell:%d\n", myId,cell);
                // get the neighbor count
                int neighbors = neighborCount(cell, row, state->current);
                // and set the future state
                int futureState = liveOrDie(neighbors,state->current->tiles[row][cell]);
                state->next->tiles[row][cell] = futureState;
            }
        }
    }
    printf("Exiting thread %ld ...\n", myId);
    pthread_exit(NULL);
}


Board* evolve(Board* board){
    // Build the 2d array
    Board* newBoard = allocateBoardTiles(board->height, board->width);
    
    // wait until I can set up a new board
    sem_wait(state->lock);
    
    state->current = board;
    state->next = newBoard;
    state->rowsProcessed = 0;
    state->rowsRemainingToProcess = board->height;
    state->totalRowCount = board->height;
    sem_post(state->lock);
    
    printf("Waiting for evolution...\n");
    // sleep(1);
    sem_wait(state->evolutionDone);
    printf(" evolution complete.\n");
    
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

// Small function that is called when SIGINT is received
void trap(int signal){ 
    execute = 0;
    // ask the threads to shut down
    sem_wait(state->lock);
    state->running = 0;
    sem_post(state->lock);
}

int main (int argc, char const *argv[]){    
    
    int seed = 100;
    if (argc > 1){
        seed = atoi(argv[1]);
    }
    
    ScreenSize size = determineScreenSize();
    
    // ncurses initialization
    // initscr();
        
    // Build the board
    Board* board = allocateBoardTiles(size.height,size.width);
    
    // Initialize the thread controller
    state = (ThreadControl*) malloc(sizeof(ThreadControl));
    
    // Initialize the lock
    state->lock = sem_open("lock",O_CREAT);
    state->evolutionDone = sem_open("done",O_CREAT);
    
    
    int lockValue = 0;
    int evolutionDoneValue = 0;
    sem_getvalue(state->lock,&lockValue);
    sem_getvalue(state->evolutionDone,&evolutionDoneValue);
    
    printf("lock: %d, done: %d\n",lockValue,evolutionDoneValue);
    
    // sem_init(state->lock, 0, 1);            // Initialized the semaphore unlocked...
    // sem_init(state->evolutionDone, 0, 0);   // this is locked/blocked
    
    // Let them run, but there are no rows to process yet
    // until evolve is called.
    state->running = 1;
    state->rowsRemainingToProcess = 0;
    state->rowsProcessed = 0;
    state->totalRowCount = 0;
    
    // Start up the thread pool
    pthread_t threads[THREAD_COUNT]; 
    int returnedValue;
    for(long threadIndex = 0; threadIndex < (THREAD_COUNT); threadIndex++){
        printf("Starting up thread %ld\n", threadIndex);
        returnedValue = pthread_create(&threads[threadIndex], NULL, evolveRowWorker, (void *)threadIndex);
        if (returnedValue){
            printf("ERROR; return code from pthread_create() is %d\n", returnedValue);
            exit(-1);
        }
    }
    
    // generate the board
    generateLifeOn(board, seed);
    
    // Counter for the generation
    int generation = 0;
    
    // set up the signal listening
    signal(SIGINT, &trap);
    execute = 1;
        
    
    // while(execute == 1){
        board = evolve(board);

        // printWithCurses(board);

        // mvprintw(0, 0, "Window: [%dx%d] - Seed: %d - Generation: %d",size.height, size.width, seed, generation++);
        // refresh();
        // usleep(1000*20);
    // }

    // Now release the memory of the last board
    releaseBoard(board);
    
    signal(SIGINT, SIG_DFL);
    
    // Finally clean up after ncurses
    endwin();
    
    return 0;
}
