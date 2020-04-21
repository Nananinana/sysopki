#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if(argc <2){
        printf("Wrong number of arguments");
        return 1;
    }
    char command[64];
    sprintf(command, "cat %s | sort", argv[1]);
    FILE *file_to_sort = popen(command, "w");
    pclose(file_to_sort);
}