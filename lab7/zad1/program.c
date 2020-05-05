#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "common.h"

#define WORKER1_NO 3
#define WORKER2_NO 3
#define WORKER3_NO 3

int semaphore_id;
int memory_id;
pid_t worker_pids[WORKER1_NO + WORKER2_NO + WORKER3_NO];

/*void sig_handler(int signal_no) {
    int workers_no = WORKER1_NO + WORKER2_NO + WORKER3_NO;
    for (int i = 0; i < workers_no; i++)
        kill(worker_pids[i], SIGINT);
    semctl(semaphore_id, 0, IPC_RMID, NULL);
    shmctl(memory_id, IPC_RMID, NULL);
    system("make clean");
    exit(0);
}*/

void create_shared_memory() {
    key_t memory_key = ftok(getenv("HOME"), 1);
    memory_id = shmget(memory_key, sizeof(orders), IPC_CREAT | 0666);
    if (memory_id < 0) {
        printf("Can't create shared memory, memory_id is less than 0\n");
        exit(-1);
    }
}

void create_semaphore() {
    key_t semaphore_key = ftok(getenv("HOME"), 0);
    semaphore_id = semget(semaphore_key, 6, IPC_CREAT | 0666);
    if (semaphore_id < 0) {
        printf("Can't create semaphore, semaphor_id is less than 0\n");
        exit(-1);
    }
    union semaphore_no sem_no;
    sem_no.value = 0;
    for (int i = 0; i < 6; i++)
        semctl(semaphore_id, i, SETVAL, sem_no);
}

void start_workers() {
    for (int i = 0; i < WORKER1_NO; i++) {
        pid_t new_worker1_pid = fork();
        if (new_worker1_pid == 0)
            execlp("./worker_1", "worker_1", NULL);
        worker_pids[i] = new_worker1_pid;
    }
    for (int i = 0; i < WORKER2_NO; i++) {
        pid_t new_worker2_pid = fork();
        if (new_worker2_pid == 0)
            execlp("./worker_2", "worker_2", NULL);
        worker_pids[i + WORKER1_NO] = new_worker2_pid;
    }
    for (int i = 0; i < WORKER3_NO; i++) {
        pid_t new_worker3_pid = fork();
        if (new_worker3_pid == 0)
            execlp("./worker_3", "worker_3", NULL);
        worker_pids[i + WORKER1_NO + WORKER2_NO] = new_worker3_pid;
    }
    for (int i = 0; i < WORKER1_NO + WORKER2_NO + WORKER3_NO; i++)
        wait(NULL);
}

int main()
{
    //printf("????");
    //signal(SIGINT, sig_handler);
    create_semaphore();
    create_shared_memory();
    start_workers();
    semctl(semaphore_id, 0, IPC_RMID, NULL);
    shmctl(memory_id, IPC_RMID, NULL);
    //system("make clean");
    return 0;
}