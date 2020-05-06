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

int semaphore_id;
int memory_id;

void get_access_to_memory() { 
    srand(time(NULL));
    operation *receive_operation = calloc(3, sizeof(operation));
    receive_operation[0].sem_num = receive_operation[1].sem_num = 0;
    receive_operation[0].sem_flg = receive_operation[1].sem_flg = 0;
    receive_operation[0].sem_op = 0;
    receive_operation[1].sem_op = 1;
    receive_operation[2].sem_num = 1;
    receive_operation[2].sem_flg = 0;
    receive_operation[2].sem_op = 1;
    semop(semaphore_id,receive_operation, 3);
}

void close_memory_access() {
    operation *stop_receiving = calloc(2, sizeof(operation));
    stop_receiving[0].sem_num = 0;
    stop_receiving[0].sem_flg = 0;
    stop_receiving[0].sem_op = -1;    
    stop_receiving[1].sem_num = 3;
    stop_receiving[1].sem_flg = 0;
    stop_receiving[1].sem_op = 1;
    semop(semaphore_id, stop_receiving, 2);
}

void receive_order() {
    get_access_to_memory();
    int *orders = calloc(MAX_ORDERS, sizeof(int));
    int order_idx = (semctl(semaphore_id, 1, GETVAL, NULL) - 1) % MAX_ORDERS;
    int order_value = rand()%100;
    int to_prepare_no = semctl(semaphore_id, 3, GETVAL, NULL) + 1;
    int to_send_no = semctl(semaphore_id, 5, GETVAL, NULL);
    orders = shmat(memory_id, NULL, 0);
    orders[order_idx] = order_value;
    printf("[%d %ld] Dodalem liczbe: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n", getpid(), time(NULL), order_value, to_prepare_no, to_send_no);
    shmdt(orders);
    close_memory_access();
}

int main() {
    srand(time(NULL));
    semaphore_id = get_semaphore_id();
    memory_id = get_memory_id();
    while (1) {
        usleep(500000);
        if (semctl(semaphore_id, 3, GETVAL, NULL) + semctl(semaphore_id, 5, GETVAL, NULL) < MAX_ORDERS)
            receive_order();
    }
}