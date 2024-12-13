
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
    int main(int argc, char *argv[]){
        //create array for pipe
        int tube[2];
        //reader
        char buffer[10];
        //make pipe
        pipe(tube);
        // int for PID
        int pid;
        //fork and get PID's 
        pid = fork();
        //if PID is 0 that means we're in the child process, but parent is executed first, 
        //so we need to write to the pipe in parent 
        if(pid == 0) {
            //write to tube, so that the parent will read that later
            write(tube[1], "pong", 10);
            //read what parent sent
            read(tube[0], buffer, sizeof(buffer));
            //print what parent sent
            printf("%d: received %s\n",getpid(),buffer);
            //return to parent 
            exit(0);
        
	    }else{
           //write to pipe so that child will read this 
           write(tube[1],"ping", 10);
           //let child execute
           wait(0);
           //read what child sent back in pipe
           read(tube[0],buffer, sizeof(buffer));
           //print what child sent
           printf("%d: received %s\n",getpid(),buffer);
           //close the pipe
           close(tube[0]);
           close(tube[1]);
        }
        exit(0);
}
    
