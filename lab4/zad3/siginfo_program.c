#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

void handler(int signal, siginfo_t *siginfo, void *ucontext){
    printf("\nSignal %d is in sa_sigaction handler\n",signal);
    
    if(signal==SIGCHLD)
        printf("Status of child process: %d\nReal user ID: %d\nCode of signal: %d\n",siginfo->si_status,siginfo->si_uid, siginfo->si_code );
   else
        printf("Address of fault: %p\nErrno: %d\nCode of signal: %d\n",siginfo->si_addr,siginfo->si_errno, siginfo->si_code );
   
   if(signal==SIGINT)
   exit(0);
}


int main(int argc, char **argv)
{
    struct sigaction act;
    act.sa_sigaction=handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags=SA_SIGINFO;

    sigaction(SIGUSR1,&act,NULL);
    sigaction(SIGFPE,&act,NULL);
    sigaction(SIGALRM,&act,NULL);
    sigaction(SIGCHLD,&act,NULL);
    sigaction(SIGINT,&act,NULL);
    
    raise(SIGUSR1);
    raise(SIGFPE);
    raise(SIGALRM);
    int child_pid=fork();
    if(child_pid==0)
        exit(144);
    wait(NULL);
    raise(SIGINT);
    return 0;
}