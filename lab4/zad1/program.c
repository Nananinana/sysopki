//#define _POSIX_C_SOURCE 1
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int stop = 0;

void sigint_handler(int sig_no)
{
    printf("SIGINT signal received \n");
    exit(0);
}

void sigstp_handler(int sig_no)
{
    if (stop)
    {
        stop = 0;
        return;
    }
    printf("Waiting for CTRL+Z - continuation or CTRL+C - end of the program \n");
    stop = 1;
}

int main()
{
    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    signal(SIGSTP, sigstp_handler);
    while (1)
    {
        if (!stop)
        {
            system("ls -l");
            sleep(1);
        }
        else
            pause();
    }
    return 0;
}
