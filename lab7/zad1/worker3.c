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

#include "common.h"

typedef struct sembuf operation;
int semaphore_id;
int memory_id;

void get_access_to_memory()
{
    operation *sending_operation = calloc(4, sizeof(operation));
    sending_operation[0].sem_num = sending_operation[1].sem_num = 0;
    sending_operation[0].sem_flg = sending_operation[1].sem_flg = 0;
    sending_operation[0].sem_op = 0;
    sending_operation[1].sem_op = 1;
    sending_operation[2].sem_num = 4;
    sending_operation[2].sem_flg = 0;
    sending_operation[2].sem_op = 1;
    sending_operation[3].sem_num = 5;
    sending_operation[3].sem_flg = 0;
    sending_operation[3].sem_op = -1;
    semop(semaphore_id, sending_operation, 4);
}

void close_memory_access()
{
    operation *stop_sending = calloc(1, sizeof(operation));
    stop_sending[0].sem_num = 0;
    stop_sending[0].sem_flg = 0;
    stop_sending[0].sem_op = -1;
    semop(semaphore_id, stop_sending, 1);
}

void send_order()
{
    get_access_to_memory();
    int *orders = calloc(MAX_ORDERS, sizeof(int));
    int order_idx = (semctl(semaphore_id, 4, GETVAL, NULL) - 1) % MAX_ORDERS;
    int to_prepare_no = semctl(semaphore_id, 3, GETVAL, NULL);
    int to_send_no = semctl(semaphore_id, 5, GETVAL, NULL);
    orders = shmat(memory_id, NULL, 0);
    orders[order_idx] *= 3;
    printf("[%d %ld] Wyslalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n", getpid(), time(NULL), orders[order_idx], to_prepare_no, to_send_no);
    orders[order_idx] = 0;
    shmdt(orders);
    close_memory_access();
}

int main()
{
    srand(time(NULL));
    semaphore_id = get_semaphore_id();
    memory_id = get_memory_id();
    while (1) {
        usleep(500000);
        if (semctl(semaphore_id, 5, GETVAL, NULL) > 0)
            send_order();
    }
    return 0;
}