//
//  simplePipe.c
//  GameThreads
//
//  Created by Brian Anderson on 6/12/13.
//  Copyright (c) 2013 Brian Anderson. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>


#define THREAD_COUNT 10

typedef struct {
    int pipeFileDescriptor[2];
} QueueControl;

QueueControl* queue;


void* worker(void* threadId){
    long thisThreadID = (long)threadId;
    
//    printf("Started %ld\n",thisThreadID);
    int running = 1;
    while(running){
//        printf(".");
//        printf("[Thread %2ld] - waiting for input...\n",thisThreadID);
        int rowNumber;
        size_t nbytes = read(queue->pipeFileDescriptor[0], &rowNumber, sizeof(rowNumber));
//        printf("[Thread %2ld] - Read %d (read was %ld long)\n",thisThreadID,rowNumber,nbytes);
        if(nbytes > 0){
            if(rowNumber == -1){
                running = 0;
            } else {
                printf("%2d, ",rowNumber);
            }
        }
        
    }

    printf("Exiting thread %ld.\n", thisThreadID);
    pthread_exit(NULL);
}

int main (int argc, char const *argv[]){
    // Initialize the queue
    
    queue = (QueueControl*)malloc(sizeof(QueueControl));
    pipe(queue->pipeFileDescriptor);
    
    // Start up the thread pool
    pthread_t threads[THREAD_COUNT];
    int returnedValue;
    for(long threadIndex = 0; threadIndex < (THREAD_COUNT); threadIndex++){
        returnedValue = pthread_create(&threads[threadIndex], NULL, worker, (void *)threadIndex);

        if (returnedValue){
            printf("ERROR; return code from pthread_create() is %d\n", returnedValue);
            exit(-1);
        }
    }
    
    for(int i = 0; i < 100; i++){
        write(queue->pipeFileDescriptor[1], &i, sizeof(int));
        usleep(1000*10);
        
    }
    // write this for each thread. time to end.
    int stopRequest = -1;
    printf("\n");
    for(int thread = 0; thread < (THREAD_COUNT); thread++){
        write(queue->pipeFileDescriptor[1], &stopRequest, sizeof(int));
    }
    
    sleep(2);
    free(queue);
}
