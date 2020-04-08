#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

typedef enum Mode
{
    KILL,
    SIGQUEUE,
    SIGRT
} Mode;

Mode mode;
int sigusr1_signal;
int sigusr2_signal;
int received_signals = 0;

void handler(int sig_no, siginfo_t *siginfo, void *ucontext)
{
    if (sig_no == sigusr1_signal)
    {
        received_signals++;
        printf("Catcher: received so far %d signals from sender \n", received_signals);
    }
    else if (sig_no == sigusr2_signal)
    {
        if (mode == KILL || mode == SIGRT)
        {
            for (int i = 0; i < received_signals; ++i)
                kill(siginfo->si_pid, sigusr1_signal);
            kill(siginfo->si_pid, sigusr2_signal);
        }
        else
        {
            union sigval value;
            for (int i = 0; i < received_signals; ++i)
            {
                value.sival_int = i;
                sigqueue(siginfo->si_pid, sigusr1_signal, value);
            }
            sigqueue(siginfo->si_pid, sigusr2_signal, value);
        }
        printf("catcher received end signal. Number of signs received: %d \n", received_signals);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("wrong command, should be: mode (kill/sigqueque/sigrt) \n");
        exit(1);
    }
    
    printf("catcher PID is: %d\n", getpid());
    char *mode_name = argv[1];
    if (strcmp("kill", mode_name) == 0)
    {
        mode = KILL;
        sigusr1_signal = SIGUSR1;
        sigusr2_signal = SIGUSR2;
    }
    else if (strcmp("queue", mode_name) == 0)
    {
        mode = SIGQUEUE;
        sigusr1_signal = SIGUSR1;
        sigusr2_signal = SIGUSR2;
    }
    else if (strcmp("sigrt", mode_name) == 0)
    {
        mode = SIGRT;
        sigusr1_signal = SIGRTMIN + 1;
        sigusr2_signal = SIGRTMAX - 1;
    }
    else
    {
        printf("wrong command, should be: mode (kill/sigqueque/sigrt) \n");
        exit(1);
    }
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, sigusr1_signal);
    sigdelset(&mask, sigusr2_signal);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
    {
        printf("unable to block signal\n");
        exit(1);
    }
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, sigusr1_signal);
    sigaddset(&act.sa_mask, sigusr2_signal);
    sigaction(sigusr1_signal, &act, NULL);
    sigaction(sigusr2_signal, &act, NULL);

     while (1)
    {
        usleep(100);
    }
}