#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

void handler(int sig_no)
{
    printf("Handler received a signal\n");
}

void pending_handler(int sig_no){
    sigset_t waiting_signals;
    sigemptyset(&waiting_signals);
    sigpending(&waiting_signals);
    int result=sigismember(&waiting_signals,SIGUSR1);
    if(result==1)
        printf("SIGUSR1 is pending");
    else if(result==0)
        printf("SIGUSR1 is not visible from this proccess\n");
    else
        printf("Something went wrong in pending \n"); 

}

int main(int argc, char **argv)
{
    printf("Printing from exec function \n");
    if (strcmp(argv[1], "pending") == 0)
        pending_handler(SIGUSR1);
    
    signal(SIGUSR1,handler);

    if (strcmp(argv[1], "pending") != 0)
        raise(SIGUSR1, handler);

    return 0;
}