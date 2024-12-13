#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

//code from ls.c
void find(char* path, char* filename) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
//O_RDONLY wasn't working so i replaced it with the 0x000 it extends to
    if((fd = open(path, 0x000)) < 0) {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0) {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }
//here we are checking it's a directory 
    if(st.type != T_DIR) {
        printf("this is not a directory, try again");
        close(fd);
        return;
    }
   
//ls code
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)) {
        // here we are comparing the . and .. of the file directories
        if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;
            //ls code
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0) {
            fprintf(2, "find: cannot stat %s\n", buf);;
            continue;
        }

        switch (st.type)
        {
        case T_FILE:
            if(strcmp(de.name, filename) == 0) {
                printf("%s\n", buf);
            }
            break;
        
        case T_DIR:
            find(buf, filename);
        }
    }
    close(fd);
}

int
main(int argc, char *argv[])
{
    //if argc is less than 2 then it doesnt have enough for a file directory
  if(argc < 2){
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}