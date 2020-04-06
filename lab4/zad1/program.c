//#define _POSIX_C_SOURCE 1
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int loop = 1;

void sigint_handler(int sig_no)
{
    printf("SIGINT signal received \n");
    exit(0);
}

void sigtstp_handler(int sig_no)
{
    if (loop)
    {
        loop = 0;
        printf("Waiting for CTRL+Z - continuation or CTRL+C - end of the program \n");
    }
    else  
        loop = 1;
}

int main()
{
    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    signal(SIGTSTP, sigtstp_handler);
    while (1)
    {
        if (loop)
        {
            system("ls -l");
            sleep(1);
        }
        else
            pause();
    }
    return 0;
}
