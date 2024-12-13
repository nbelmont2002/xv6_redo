#include "kernel/types.h"
#include "user.h"
#include "kernel/param.h"
int main(int argc, char *argv[])
{   
    //we will hold the arguements here, MAXARG is the max number of arguements
    //as defined in the kernel we can have
    char *arguements[MAXARG];
    
    int index ;
    //buffer 
    char buf[512];

    //here we are adding the command and the arguements into arguements array
    // so we can later execute them
    for (index = 1; index < argc; index++) {
        //the first run is arguements[0] = argv[1] which is the command to exec
        //the next ones add the arguements into the rest of the arguements array 
        arguements[index - 1] = argv[index];
    }

    int n = 0;   
    // here we will begin to add new arguements starting after the last 
    //entry from the upper loop^ 
    int arg_index = index - 1; 
    //while we have characters to read we continue
    while ((n = read(0, buf, sizeof(buf))) > 0) {
        // in a for loop we are checking the chars for the new line
        for (int j = 0; j < n; j++) {
            if (buf[j] == '\n' ) {
                //if we find a \n
                buf[j] = 0; 
                    //into the arguements index we will add what's in the buffer
                    arguements[arg_index] = buf; 
                    //here we begin the execution of the arguements
                    if (fork() == 0) {
                        //argv 1 is the command and arguements are the arguements
                        exec(argv[1], arguements);
                    }
                    // we call wait so we can let the child finish
                    wait(0);
            }
        }
    }

    exit(0);
}