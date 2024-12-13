
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]) {
    //set first to 2 bc thats the range we are starting at 
    int first = 2;
    //_pipe[0][0] and _pipe[0][1] are the read and write ends of the first pipe.
    //_pipe[1][0] and _pipe[1][1] are the read and write ends of the second pipe.

    int pipes[2][2];
    //create the first pipe that the parent process will use
    pipe(pipes[0]);
    // set index to 0 so we can toggle between 0 and 1 
    int index = 0;
    //int to store values we read from the 
    int num;
    //populate the first pipe with 2-35
    for (int i = first; i <= 35; ++i) {
        write(pipes[0][1], &i, 4);
    }
    //close write ends of the first parent pipe
    close(pipes[0][1]);

    // beginning forking and child processes since we have 2-35 in the parent pipe
    while (fork() == 0) {
        // first process reads from the parent and take the first int to print as a prime
        // in this case it'll be 2 first 
        if (read(pipes[index][0], &first, 4) != 0) {
            printf("prime %d\n", first);
        } else {
            // it will exit the while loop if there's no more numbers
            exit(0);
        }
        //once we have the first number printed then we can start a second pipe
        // that'll we use to write the filtered numbers to
        pipe(pipes[index ^ 1]);

        // second while loop will read thru the first pipe to filter 
        while (read(pipes[index][0], &num, 4) != 0) {
            // will see if theres a remainder with the first num, if not write to the second pipe
            if (num % first) {
                // write to the second pipe by toggling zero to one so we are writing 
                // to the second pipe we created above on line 38
                write(pipes[index ^ 1][1], &num, 4);
            }
        }
        // close the pipe [1][1] ,the write of the second pipe 
        close(pipes[index ^ 1][1]);
        // change index back to 0 to continue looping
        index = index ^ 1;
    }
    // close the pipe [0][0] read of the first pipe
    close(pipes[index][0]);
    // wait to allow child processs to go thru 
    wait(0);

    exit(0);
}
