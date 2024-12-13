#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

int main(int argc, char *argv[]){
if (argc != 2 ){
        printf("Error: Please enter a number when you compile\n");
        
    }else{
        int t = atoi(argv[1]);
        sleep(t);
        exit (0);

    }
        exit (0);

}