#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

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
    if(argc<2){
        printf("Wrong command\n");
        return 1;
    }

    else if (strcmp(argv[1], "handler") == 0)
        signal(SIGUSR1, handler);
    else if (strcmp(argv[1], "ignore") == 0)
        signal(SIGUSR1, SIG_IGN);
    else if (strcmp(argv[1], "pending") == 0 || strcmp(argv[1], "mask") == 0)
    {
        sigset_t newmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &newmask, NULL) < 0)
            perror("Unable to block signal");
        if (strcmp(argv[1], "pending") == 0)
        pending_handler(SIGUSR1);
    }
    else
    {
        printf("Wrong command\n");
        return 1;
    }
    raise(SIGUSR1);
    sigset_t newmask;

    if (strcmp(argv[1], "pending") == 0)
        pending_handler(SIGUSR1);
    if (strcmp(argv[2], "exec") == 0)
        execl("./exec", "./exec", argv[1], NULL);
    else
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            printf("Printing from child process\n");
            if (strcmp(argv[1], "pending") != 0)
                if(strcmp(argv[1], "handler") != 0) 
                    signal(SIGUSR1, handler);
                raise(SIGUSR1);
            else
                pending_handler(SIGUSR1);
        }
    }
    return 0;
}