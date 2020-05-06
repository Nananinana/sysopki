#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#define MAX_ORDERS 5

int get_semaphore();
int get_shared_memory();

union semaphore_no {
    int value;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int get_semaphore() {
    key_t semaphore_key = ftok(getenv("HOME"), 0);
    int semaphore_id = semget(semaphore_key, 0, 0);
    if (semaphore_id < 0) {
        printf("Can't get semaphore, semaphor_id is less than 0\n");
        exit(-1);
    }
    return semaphore_id;
}

int get_shared_memory() {
    key_t memory_key = ftok(getenv("HOME"), 1);
    int memory_id = shmget(memory_key, 0, 0);
    if (memory_id < 0) {
        printf("Can't get shared memory, memory_id is less than 0\n");
        exit(-1);
    }
    return memory_id;
}

#endif