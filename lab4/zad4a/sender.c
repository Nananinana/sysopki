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
int number_of_signals;
int received_signals;
int sigusr1_signal;
int sigusr2_signal;

void handler(int sig_no, siginfo_t *siginfo, void *ucontext)
{
    if (sig_no == sigusr1_signal)
    {
        received_signals++;
        printf("Sender: received so far %d signals from catcher \n", received_signals);
        if (mode == SIGQUEUE)
            printf("catcher index: %d\n", siginfo->si_value.sival_int);
    }
    else if (sig_no == sigusr2_signal)
    {
        printf("Sender received end signal. Number of signals received: %d, should have received: %d\n", received_signals, number_of_signals);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("wrong command, should be: catcher_PID number_of_signals_to_send mode (kill/sigqueque/sigrt) \n");
    }
    int catcher_PID = atoi(argv[1]);
    number_of_signals = atoi(argv[2]);
    char *mode_name = argv[3];

    if (strcmp("kill", mode_name) == 0)
    {
        mode = KILL;
        sigusr1_signal = SIGUSR1;
        sigusr2_signal = SIGUSR2;
    }
    else if (strcmp("sigqueue", mode_name) == 0)
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
        printf("wrong command, should be: catcher_PID number_of_signals_to_send mode (kill/sigqueque/sigrt) \n");
        exit(1);
    }

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, sigusr1_signal);
    sigdelset(&mask, sigusr2_signal);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
    {
        printf("unable to block signal \n");
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

    printf("sender has PID: %d\n", getpid());

    if (mode == KILL || mode == SIGRT)
    {
        for (int i = 0; i < number_of_signals; ++i)
        {
            kill(catcher_PID, sigusr1_signal);
            printf ("signal number %d sent \n", i);
        }
        kill(catcher_PID, sigusr2_signal);
    }
    else
    {
        union sigval value;
        value.sival_int = 0;
        for (int i = 0; i < number_of_signals; ++i)
            sigqueue(catcher_PID, sigusr1_signal, value);
        sigqueue(catcher_PID, sigusr2_signal, value);
    }
     while (1)
    {
        sleep(100);
    }
    return 0;
}