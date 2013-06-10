# Notes about C game of life

The modulus operator works differently in C than in Ruby (surprise :)

This is my first time using ncurses and I found a [nice tutorial](http://www.tldp.org/HOWTO/NCURSES-Programming-HOWTO/helloworld.html)

This is also my first time using a [SIGNAL handler](http://stackoverflow.com/questions/3189650/execute-c-program-till-ctrlc-hit-in-the-terminal), lots of fun in the c-world.

## To Build and Run

    make run
    
It will run in the size of your terminal. Optionally you can make and then call it with 1 argument which is an integer seed.

    make
    ./game <integer seed>

Have fun!