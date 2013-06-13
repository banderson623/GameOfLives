#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
// for timing
#include <sys/time.h>
#include <math.h>

#define EGYPTIAN_COTTON 4
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

// Used to queue a thread to process a row on the game
typedef struct {
    int rowIndex;
    short isDone;
    Board* current;
    Board* next;
} QueuedTask;


// Holds the two pipes used to communicate
// between tasks and control
typedef struct {
    int taskPipe[2];
    int responsePipe[2];
} QueuePipes;


// -------- GLOBAL STATE -----------------------------------
// Has to be global to share between all the threads
// this gets malloced in main
QueuePipes* queue;

// To catch the control c signal
int canExecute;

// A collection of task structures
// to use during the communication with the threads
// This is a pointer to an array of QueuedTasks
QueuedTask** taskCollection;
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

// Release the board's memory
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
    // printf("Neighbor count row: %d, col: %d\n",row,column);
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
    
    
    int h,w;
    h = board->height;
    w = board->width;
    int** tiles = board->tiles;
    
    for(int row_offset = -1; row_offset <= 1; row_offset++){
        int rowOffsetIndex = ((h + row + row_offset) % h);

        for(int column_offset = -1; column_offset <= 1; column_offset++){
            if (!(row_offset == 0 && column_offset == 0)){
//                count+= board->tiles[((board->height + row + row_offset) % board->height)]
//                [((board->width + column + column_offset) % board->width)];
                count+= tiles[rowOffsetIndex][((w + column + column_offset) % w)];
            }
        }
    }
    return count;
}

// This will take the current board, examine the row passed in
// and then add that row to the nextGeneration board.
// Both boards should be initialized and allocated
void* evolveRowWorker(void* threadId){
    long thisThreadId = (long)threadId;
    
    // Malloc this once, and use this as a space to hold the incoming task
    QueuedTask* task = 0;
    
    // Simple way to keep things moving
    int running = 1;
    int row = 0;
    // how much data is read from the pipe
    size_t bytesRead;
    
    while(running){
        bytesRead = 0;
        
        // -- Block until something is in the pipe ---------------------------
        bytesRead = read(queue->taskPipe[0], &task, sizeof(QueuedTask*));
        if(bytesRead == sizeof(QueuedTask*)){
            
            row = task->rowIndex;
            
            if(row >= 0){
                // -- Process this Row's game of life rules -----------
                for(int cell = 0; cell < task->current->width; cell++){
                    // get the neighbor count
                    int neighbors = neighborCount(cell, row, task->current);
                    // and set the future state
                    int futureState = liveOrDie(neighbors,task->current->tiles[row][cell]);
                    task->next->tiles[row][cell] = futureState;
                }
                
                // -- Send a message back -----------------------------
                // Now send a response that we have this row completed...
                // QueuedResponse* response = (QueuedResponse*) malloc(sizeof(QueuedResponse));
                // response->id = rowOperatingOn;
                task->isDone = 1;
                //                printf("Responding to task: 0x%x\n",task);
                write(queue->responsePipe[1], &task, sizeof(QueuedTask*));
                
            } else {
                // -- End the thread was requested --------------------
                running = 0;
            }
        } else {
            printf("wrong size read: %zd\n",bytesRead);
        }
    }
    printf("Exiting thread %ld ...\n", thisThreadId);
    pthread_exit(NULL);
}


void evolve(Board* board, Board* newBoard){
    
    int tasksSent = 0;
    for(int row = 0; row < board->height; row++){
        // The task is created here...
        QueuedTask* task = taskCollection[row]; // = (QueuedTask*)malloc(sizeof(QueuedTask));
        task->rowIndex = row;
        task->isDone = 0;
        task->current = board;
        task->next = newBoard;
        //                          +----------- address to allocated task
        //                          v      v---- the size of a pointer for this architecture
        write(queue->taskPipe[1], &task, sizeof(QueuedTask*));
        tasksSent++;
    }
    
    

    QueuedTask* response = 0;
    while(tasksSent > 0){
        size_t bytesRead = read(queue->responsePipe[0],&response, sizeof(QueuedTask*));
        if(bytesRead == sizeof(QueuedTask*)){
            // free(response);
            tasksSent--;
        }
    }
    
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
    canExecute = 0;
}


int main (int argc, char const *argv[]){
    
    int seed = 100;
    // if you have an argument, it will be used as the seed
    if (argc > 1){
        seed = atoi(argv[1]);
    }
    
    ScreenSize size = determineScreenSize();
    // size.width = size.height = 5000;
    
    // Preallocate all the QueuedTask's, they will be recycled so that
    // each evolution doesn't incure the cost of row-number of malloc()s and free()s
    // ------------------------------------------------------------------------------
    // Allocate enough space for a pointer for each task for each row
    // ....so this is the size of a pointer, times the number of rows.
    // *taskCollection = malloc(sizeof(QueuedTask*) * size.height);
    taskCollection = malloc(sizeof(QueuedTask*) * size.height);
    for(int i = 0; i < size.height; i++){
        // Malloc each task in the queue
        taskCollection[i] = (QueuedTask*) malloc(sizeof(QueuedTask));
    }
    
    // ncurses initialization
    initscr();
    
    // Build the board
    Board* board1 = allocateBoardTiles(size.height,size.width);
    Board* board2 = allocateBoardTiles(size.height,size.width);
    
    Board* current = board1;
    Board* nextGeneration = board2;
    
    // Initialize the main queue
    queue = (QueuePipes*) malloc(sizeof(QueuePipes));
    
    // Now initialize the pipes
    pipe(queue->taskPipe);
    pipe(queue->responsePipe);
    
    
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
    generateLifeOn(current, seed);
    
    // Counter for the generation
    int generation = 0;
    
    // set up the signal listening for control-c
    signal(SIGINT, &trap);
    
    // Start it off as able to run ;)
    canExecute = 1;
    
    // Declare the timers
    struct timeval startClock;
    struct timeval endClock;
    
    while(canExecute == 1) {
        // Get time of the evolution start
        gettimeofday(&startClock,NULL);
        
        // Evolve will operate on the current generation
        //      and overwrite nextGeneration with the results
        evolve(current,nextGeneration);

        // Get time of the evolution end        
        gettimeofday(&endClock,NULL);
        
        printWithCurses(nextGeneration);
        
        
        // Swap current and next generation after each evolution
        if(current == board1){
            current = board2;
            nextGeneration = board1;
        } else {
            current = board1;
            nextGeneration = board2;
        }
        
        // Timing calculations...
        double t0 = (double)startClock.tv_sec * 1000.0 * 1000.0 + (double)startClock.tv_usec;
        double t1 = (double)endClock.tv_sec * 1000.0 * 1000.0 + (double)endClock.tv_usec;
        float timeDiff = (t1 - t0) / 1000;//000.0;
        // printf("Generation: %d, evolution time: %2.2fs\n",generation++, timeDiff);
        
        
        
        mvprintw(0, 0, "Window: [%dx%d] - Seed: %d - Generation: %d, time:%2.f ms",size.height, size.width, seed, generation++, timeDiff);
        refresh();
        usleep(1000*20);
        // sleep(1);
    }
    
    // Now release the memory of the allocated boards
    releaseBoard(board1);
    releaseBoard(board2);

    // Clean up all the tasks
    for(int i = 0; i < size.height; i++){
        free(taskCollection[i]);
    }
    free(taskCollection);
    
    
    signal(SIGINT, SIG_DFL);
    
    // Finally clean up after ncurses
    endwin();
    printf("bye...\n");
    return 0;
}
