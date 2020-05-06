#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "common.h"

typedef struct sembuf operation;
int semaphore_id;
int memory_id;

void get_access_to_memory()
{
    operation *packing_operation = calloc(4, sizeof(operation));
    packing_operation[0].sem_num = packing_operation[1].sem_num = 0;
    packing_operation[0].sem_flg = packing_operation[1].sem_flg = 0;
    packing_operation[0].sem_op = 0;
    packing_operation[1].sem_op = 1;
    packing_operation[2].sem_num = 2;
    packing_operation[2].sem_flg = 0;
    packing_operation[2].sem_op = 1;
    packing_operation[3].sem_num = 3;
    packing_operation[3].sem_flg = 0;
    packing_operation[3].sem_op = -1;
    semop(semaphore_id, packing_operation, 4);
}

void close_memory_access()
{
    operation *stop_packing = calloc(2, sizeof(operation));
    stop_packing[0].sem_num = 0;
    stop_packing[0].sem_flg = 0;
    stop_packing[0].sem_op = -1;
    stop_packing[1].sem_num = 5;
    stop_packing[1].sem_flg = 0;
    stop_packing[1].sem_op = 1;
    semop(semaphore_id, stop_packing, 2);
}

void pack_order()
{
    get_access_to_memory();
    int *orders = calloc(MAX_ORDERS, sizeof(int));
    int order_idx = (semctl(semaphore_id, 2, GETVAL, NULL) - 1) % MAX_ORDERS;
    int to_prepare_no = semctl(semaphore_id, 3, GETVAL, NULL);
    int to_send_no = semctl(semaphore_id, 5, GETVAL, NULL) + 1;
    orders = shmat(memory_id, NULL, 0);   
    orders[order_idx] *= 2;
    printf("[%d %ld] Przygotowalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n", getpid(), time(NULL), orders[order_idx], to_prepare_no, to_send_no);
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
        if (semctl(semaphore_id, 3, GETVAL, NULL) > 0) 
            pack_order();
    }
    return 0;
}