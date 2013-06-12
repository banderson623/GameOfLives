# C Game of life *Threaded*

I wanted to try and make a threaded game of life. 

By carefully selecting how to parallelize the problem, we can avoid extra locking issues.

1. The Game of life can be parallelized per row, since each evolution is a copy operation, the last generation data could be shared by many threads.

2. And since each thread is only writing to "its" row (a pointer to an array of integers), you don't need to lock during write either.

## Pools

At first I thought I would just build a bunch of threads each evolution step, have them do their work and then clean them up before starting the next evolution. This would be a lot of thread maintenance. Instead I decided to create n-number of worker-threads, and create a pool of threads.

Then, use a posix pipe to "feed" the worker threads. Since freads() are blocking, n-number of worker threads could block on the pipe, using it as a queue to send data down to the work threads.

    typedef struct {
        int id;
        int rowIndex;
        Board* current;
        Board* next;
    }  Task;


Each work request is a struct that contains some basic information row number, current board state, and future board state.

Of course I need another pipe, in the reverse direction, to send messages for the thread-controller to know when all the 
workers have completed their tasks. This will be a simple struct relaying an ID and status.

    typedef struct {
        int id;
        int status;
    }  Response;

## Workers

The worker threads themselves are pretty basic. They take a row index of the current board, walk each element....


## To Build and Run

    make run
    
It will run in the size of your terminal. Optionally you can make and then call it with 1 argument which is an integer seed.

    make
    ./game <integer seed>

Have fun!