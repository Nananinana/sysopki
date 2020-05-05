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

typedef struct sembuf sembuf;

void set_sembuf()
{
    srand(time(NULL));
    sembuf *received_order = calloc(3, sizeof(sembuf));
    received_order[0].sem_num = received_order[1].sem_num = 0;
    received_order[0].sem_op = 0;
    received_order[0].sem_flg = 0;

    received_order[1].sem_num = 0;
    received_order[1].sem_op = 1;
    received_order[1].sem_flg = 0;

    received_order[2].sem_num = 1;
    received_order[2].sem_op = 1;
    received_order[2].sem_flg = 0;

    semop(semaphore_id,received_order, 3);
}

set_back_sembuf()
{
    sembuf *back = calloc(2, sizeof(sembuf));

    back[0].sem_num = 0;
    back[0].sem_op = -1;
    back[0].sem_flg = 0;

    back[1].sem_num = 3;
    back[1].sem_op = 1;
    back[1].sem_flg = 0;

    semop(semaphore_id, back, 2);
}

void receive_order()
{
    set_sembuf();
    int *orders = calloc(MAX_ORDERS, sizeof(int));
    int order_idx = (semctl(semaphore_id, 1, GETVAL, NULL) - 1) % MAX_ORDERS;
    int order_value = rand()%100;
    int to_prepare_no = semctl(semaphore_id, 3, GETVAL, NULL) + 1;
    int to_send_no = semctl(semaphore_id, 5, GETVAL, NULL);
    orders = shmat(memory_id, NULL, 0);
    orders[order_idx] = order_value;
    printf("[%d %ld] Dodalem liczbe: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n", getpid(), time(NULL), order_value, to_prepare_no, to_send_no);
    shmdt(orders);
}

int main()
{
    srand(time(NULL));
    semaphore_id = get_semaphore();
    memory_id = get_shared_memory();

    while (1) {
        usleep(500000);
        if (semctl(semaphore_id, 3, GETVAL, NULL) + semctl(semaphore_id, 5, GETVAL, NULL) < MAX_ORDERS)
            receive_order();
    }
}